#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_net.h"
#include "protobufs.h"
#include "client.h"
#include "net.h"
#include "game.h"

client_t **g_clients; //[MAX_CLIENTS];
int g_client_ids = 0;
int g_client_count = 0;
int g_last_ping = 0;

int main(int argc, char **argv) {
	ENetEvent event;
	ENetAddress address;
	ENetHost * server;
	int quit = 0;

	g_init();
	address.host = ENET_HOST_ANY;
	address.port = 1234;
	server = enet_host_create(
		&address,
		32, // 32 connections
		2, // channels?
		0, // inc bw?
		0  // out bw
  );

	if (server == NULL) {
		fprintf(stderr, "An error occurred while trying to create an ENet server host.\n");
		exit(EXIT_FAILURE);
	}

	while (!quit) {

		if (enet_host_service(server, &event, 10)) {
			int ticks = SDL_GetTicks();
			switch (event.type) {
				case ENET_EVENT_TYPE_CONNECT:
					printf ("A new client connected from %x:%u.\n", 
						event.peer->address.host,
						event.peer->address.port
					);

					event.peer->data = "Client information";
					break;

				case ENET_EVENT_TYPE_RECEIVE:
					net_handle_message(event);

					enet_packet_destroy (event.packet);
					break;

				case ENET_EVENT_TYPE_DISCONNECT:
					printf("%s disconnected.\n", event.peer->data);

					event.peer->data = NULL;
					break;
			}
		}
	}

	enet_host_destroy(server);

	return EXIT_SUCCESS;
}
