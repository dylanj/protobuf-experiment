#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_net.h"
#include "protobufs/handshake.pb-c.h"
#include "protobufs/input.pb-c.h"
#include "protobufs/wrapper.pb-c.h"

IPaddress server_ip;

void net_send_message(int type, UDPsocket s, void *data) {
	WrapperMessage msg = WRAPPER_MESSAGE__INIT;
	UDPpacket *p;

	if (!(p = SDLNet_AllocPacket(1025))) {
		fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}

	msg.type = type;
	switch(type) {
		case WRAPPER_MESSAGE__TYPE__HANDSHAKE:
			msg.handshake_message = data; break;
		case WRAPPER_MESSAGE__TYPE__INPUT:
			msg.input_message = data; break;
	}

	wrapper_message__pack(&msg, p->data);
	p->len = wrapper_message__get_packed_size(&msg);

	p->address.host = server_ip.host;
	p->address.port = server_ip.port;

	if (SDLNet_UDP_Send(s, -1, p) < 0) {
		fprintf(stderr, "SDLNet_UDP_Send: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}

	SDLNet_FreePacket(p);
}

void net_msg_handshake(UDPsocket sd, char *name, char *country) {
	HandshakeMessage msg = HANDSHAKE_MESSAGE__INIT;
	void *buffer = malloc(512);

	msg.name = name;
	msg.country = country;

	net_send_message(WRAPPER_MESSAGE__TYPE__HANDSHAKE, sd, &msg);

	printf("sent a handshake\n");

	free(buffer);
}

void net_msg_input(UDPsocket sd, int key, int press) {
	InputMessage msg = INPUT_MESSAGE__INIT;
	void *buffer = malloc(512);

	msg.key = key;
	msg.press = press;

	net_send_message(WRAPPER_MESSAGE__TYPE__INPUT, sd, &msg);

	free(buffer);
}

void g_init() {
	if ( SDLNet_Init() < 0 ) {
		fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char **argv) {
	char *server_address = "127.0.0.1";
	int server_port = 1234;
	UDPsocket sd;

	g_init();

	if (!(sd = SDLNet_UDP_Open(0))) {
		fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}

	if (SDLNet_ResolveHost(&server_ip, server_address, server_port) < 0)
	{
		fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}

	net_msg_handshake(sd, "phishtopher", "Canada");
	net_msg_input(sd, 4, 1);

	SDLNet_Quit();
	return 0;
}
