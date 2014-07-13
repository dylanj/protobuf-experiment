#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_net.h"
#include "../protobufs/handshake.pb-c.h"
#include "../protobufs/input.pb-c.h"
#include "../protobufs/wrapper.pb-c.h"

int g_init() {
	if ( SDLNet_Init() < 0 ) {
		fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}
}

int handle_handshake(HandshakeMessage* msg) {
	printf("handshake: \n");

	printf("name: %s\n", msg->name);
	printf("country: %s\n", msg->country);
}

int handle_input(InputMessage* msg) {
	printf("input: \n");

	printf("key: %d\n", msg->key);
	printf("press: %d\n", msg->press);
}

int main(int argc, char **argv) {
	WrapperMessage *msg;
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
			printf("UDP Packet incoming\n");
			printf("\tChan:			%d\n", p->channel);
			printf("\tLen:			%d\n", p->len);
			printf("\tMaxlen:		%d\n", p->maxlen);
			printf("\tStatus:		%d\n", p->status);
			printf("\tAddress:	%x %x\n", p->address.host, p->address.port);

			msg = wrapper_message__unpack(NULL, p->len, p->data);

			if ( msg == NULL ) {
				fprintf(stderr, "Error decoding message\n");
				exit(EXIT_FAILURE);
			}

			switch(msg->type) {
				case WRAPPER_MESSAGE__TYPE__HANDSHAKE:
					handle_handshake(msg->handshake_message); break;
				case WRAPPER_MESSAGE__TYPE__INPUT:
					handle_input(msg->input_message); break;
			}

			wrapper_message__free_unpacked(msg, NULL);
		}
	}

	SDLNet_FreePacket(p);
	SDLNet_Quit();
	return EXIT_SUCCESS;
}
