#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "protobufs.h"
#include "client.h"
#include "game.h"
#include "net.h"
#include "window.h"

#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>

client_t **g_clients;
SDL_Event e;

int g_client_ids = 0;
int g_client_count = 0;
int g_last_ping = 0;

void g_client_init();
void g_client_init() {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		printf("Failed to init sdl.\n");
		exit(EXIT_FAILURE);
	}

	printf("Loaded SDL.");
}

int do_input( ENetPeer *peer ) {
	int quit = 0;
	int keypress = -1;
	int action = -1;
	while( SDL_PollEvent( &e ) != 0 ) {
		keypress = -1;
		if( e.type == SDL_QUIT ) {
			quit = 1;
		} else if( e.type == SDL_KEYDOWN ) {
			keypress = 1;
			printf("key down: %d\n", e.key.keysym.sym);
		} else if( e.type == SDL_KEYUP ) {
			keypress = 0;
			printf("key up: %d\n", e.key.keysym.sym);
		}

		if ( keypress > -1 ) {
			switch(e.key.keysym.sym) {
				case SDLK_w:
					action = INPUT_MESSAGE__ACTION__FORWARD;
					break;
				case SDLK_a:
					action = INPUT_MESSAGE__ACTION__STRAFE_LEFT;
					break;
				case SDLK_d:
					action = INPUT_MESSAGE__ACTION__STRAFE_RIGHT;
					break;
				case SDLK_s:
					action = INPUT_MESSAGE__ACTION__BACK;
					break;
				case SDLK_SPACE:
					action = INPUT_MESSAGE__ACTION__JUMP;
					break;
			}

			if ( action >= 0 ) {
			  net_send_input(peer, action, keypress);
			}
		}
	}

	return quit;
}

int main(int argc, char **argv) {
	ENetAddress address;
	ENetEvent event;
	ENetPeer *peer;
	ENetHost *client;

	g_init();
	g_client_init();

	enet_address_set_host(&address, "127.0.0.1");
	address.port = 1234;

	if ( &address == NULL ) {
		fprintf(stderr, "Couldn't establish connection\n");
		exit(EXIT_FAILURE);
	}

	client = enet_host_create(
		NULL /* create a client host */,
    1 /* only allow 1 outgoing connection */,
    2 /* allow up 2 channels to be used, 0 and 1 */,
		57600 / 8 /* 56K modem with 56 Kbps downstream bandwidth */,
		14400 / 8 /* 56K modem with 14 Kbps upstream bandwidth */
	);

	if (client == NULL) {
    fprintf (stderr, "An error occurred while trying to create an ENet client host.\n");
    exit (EXIT_FAILURE);
	}

	peer = enet_host_connect(client, &address, 2, 0);

	if (peer == NULL) {
		fprintf (stderr, "No available peers for initiating an ENet connection.\n");
		exit (EXIT_FAILURE);
	}

	window_t *w = window_create();

	if ( w == NULL ) {
		goto cleanup;
	}

	while(1) {
		if ( do_input(peer) ) {
			printf("closing\n");
			break;
		}

		if (enet_host_service(client, &event, 10) ){
			switch (event.type) {
				case ENET_EVENT_TYPE_CONNECT:
					printf ("A new client connected from %x:%u.\n",
						event.peer->address.host,
						event.peer->address.port
					);

					event.peer -> data = "Client information";
					net_send_handshake(event.peer, argv[1], "Canada");
					break;

				case ENET_EVENT_TYPE_RECEIVE:
					net_client_handle_message(event);

					enet_packet_destroy(event.packet);
					break;

				case ENET_EVENT_TYPE_DISCONNECT:
					printf ("%s disconnected.\n", event.peer -> data);
					event.peer -> data = NULL;
					break;
			}
		}

		//render
		glClear ( GL_COLOR_BUFFER_BIT );
		SDL_GL_SwapWindow(w->window);
	}

cleanup:
	window_close(w);
	g_quit();

	return 0;
}
