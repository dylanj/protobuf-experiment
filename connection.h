#ifndef CONNECTION_H
#define CONNECTION_H

#define MAX_PINGS 3

#include "SDL2/SDL_net.h"

typedef struct connection_s {
	IPaddress ip;
	UDPsocket socket;
	char *host;
	int port;
	int missed_pings;
	int last_pong;
} connection_t;

connection_t *connection_create(UDPsocket sd, UDPpacket *p);
connection_t *connection_create_from_addr(char *host, int port);
#endif
