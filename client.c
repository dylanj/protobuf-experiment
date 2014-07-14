#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "protobufs.h"
#include "client.h"
#include "game.h"
#include "net.h"
#include "connection.h"
#include "window.h"

#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>

client_t **g_clients;
SDL_Event e;

int g_client_ids = 0;
int g_client_count = 0;

void g_client_init();
void g_client_init() {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		printf("Failed to init sdl.\n");
		exit(EXIT_FAILURE);
	}

	printf("Loaded SDL.");
}

int do_input( connection_t *s ) {
	int quit = 0;
	while( SDL_PollEvent( &e ) != 0 ) {
		if( e.type == SDL_QUIT ) {
			quit = 1;
		} else if( e.type == SDL_KEYDOWN ) {
			net_send_input(s, e.key.keysym.sym, 1);
			printf("key down: %d\n", e.key.keysym.sym);
		} else if( e.type == SDL_KEYUP ) {
			net_send_input(s, e.key.keysym.sym, 0);
			printf("key up: %d\n", e.key.keysym.sym);
		}
	}

	return quit;
}


int main(int argc, char **argv) {
	UDPpacket *p;
	if (!(p = SDLNet_AllocPacket(1025))) {
		fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}
	g_init();
	g_client_init();

	connection_t *c = connection_create_from_addr("127.0.0.1", 1234);

	if ( c == NULL ) {
		fprintf(stderr, "Couldn't establish connection\n");
		exit(EXIT_FAILURE);
	}

	net_send_handshake(c, "phishtopher", "Canada");
	net_send_input(c, 4, 1);

	window_t *w = window_create();

	if ( w == NULL ) {
		goto cleanup;
	}

	while(1) {
		if ( do_input(c) ) {
			printf("closing\n");
			break;
		}

		if (SDLNet_UDP_Recv(c->socket, p)) {
			net_client_handle_message(c, p);
		}

		glClear ( GL_COLOR_BUFFER_BIT );
		SDL_GL_SwapWindow(w->window);
	}
	SDLNet_FreePacket(p);

cleanup:
	window_close(w);
	g_quit();

	return 0;
}
