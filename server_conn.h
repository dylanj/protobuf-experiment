#ifndef SERVER_CONN_H
#define SERVER_CONN_H

#include "SDL2/SDL_net.h"

typedef struct server_conn_s {
	char *host;
	int port;
	IPaddress ip;
	UDPsocket socket;
} server_conn_t;

server_conn_t *server_conn_create(char *host, int port);

#endif
