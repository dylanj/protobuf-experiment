#include "connection.h"

connection_t *connection_create(UDPsocket sd, UDPpacket *p) {
	connection_t *c = malloc(sizeof(connection_t));

	c->socket = NULL;

	if ( p != NULL ) {
		c->ip = p->address;
	}
	if ( sd != NULL ) {
		c->socket = sd;
	}

	return c;
}

connection_t *connection_create_from_addr(char *host, int port) {
	connection_t *c = connection_create(NULL, NULL);

	c->host = malloc(strlen(host));
	strcpy(c->host, host);

	c->port = port;

	if (!(c->socket = SDLNet_UDP_Open(0))) {
		fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
		return NULL;
	}

	if (SDLNet_ResolveHost(&c->ip, c->host, c->port) < 0) {
		fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());

		return NULL;
	}

	return c;
}

