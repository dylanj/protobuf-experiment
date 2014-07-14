#ifndef CONNECTION_H
#define CONNECTION_H

#include "SDL2/SDL_net.h"

typedef struct connection_s {
	IPaddress ip;
	UDPsocket socket;
	char *host;
	int port;
} connection_t;

connection_t *connection_create(UDPsocket sd, UDPpacket *p);
connection_t *connection_create_from_addr(char *host, int port);
#endif
