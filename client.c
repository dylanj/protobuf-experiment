#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_net.h"
#include "protobufs.h"
#include "client.h"
#include "net.h"
#include "server_conn.h"

client_t **g_clients; //[MAX_CLIENTS];

int g_client_ids = 0;
int g_client_count = 0;

server_conn_t *server_conn_create(char *host, int port) {
	server_conn_t *s = malloc(sizeof(server_conn_t));
	s->host = malloc(strlen(host));
	s->port = port;

	strcpy(s->host, host);

	if (!(s->socket = SDLNet_UDP_Open(0))) {
		fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}

	if (SDLNet_ResolveHost(&s->ip, s->host, s->port) < 0)
	{
		fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}

	return s;
}

void g_init() {
	if ( SDLNet_Init() < 0 ) {
		fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char **argv) {
	g_init();

	server_conn_t *s = server_conn_create("127.0.0.1", 1234);

	net_send_handshake(s, "phishtopher", "Canada");
	net_send_input(s, 4, 1);

	SDLNet_Quit();
	return 0;
}
