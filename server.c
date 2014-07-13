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

int main(int argc, char **argv) {
	UDPsocket sd;
	UDPpacket *p;
	int quit;

	g_init();

	if (!(sd = SDLNet_UDP_Open(1234))) {
		fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}

	if (!(p = SDLNet_AllocPacket(512))) {
		fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}

	quit = 0;
	while (!quit) {
		if (SDLNet_UDP_Recv(sd, p)) {
			/* int ticks = SDL_GetTicks(); */
			printf("UDP Packet incoming\n");
			printf("\tChan:			%d\n", p->channel);
			printf("\tLen:			%d\n", p->len);
			printf("\tMaxlen:		%d\n", p->maxlen);
			printf("\tStatus:		%d\n", p->status);
			printf("\tAddress:	%x %x\n", p->address.host, p->address.port);

			printf("socket %p\n", sd);

			net_handle_message(sd, p);
		}
	}

	SDLNet_FreePacket(p);
	SDLNet_Quit();
	return EXIT_SUCCESS;
}
