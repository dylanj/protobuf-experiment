#include "server_conn.h"

server_conn_t *server_conn_create(char *host, int port) {
	server_conn_t *s = malloc(sizeof(server_conn_t));
	s->host = malloc(strlen(host));
	s->port = port;

	strcpy(s->host, host);

	if (!(s->socket = SDLNet_UDP_Open(0))) {
		fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
		return NULL;
	}

	if (SDLNet_ResolveHost(&s->ip, s->host, s->port) < 0) {
		fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
		return NULL;
	}

	return s;
}

