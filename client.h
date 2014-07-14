#ifndef CLIENT_H
#define CLIENT_H

#include "connection.h"
#include "SDL2/SDL_net.h"

#define MAX_CLIENTS 32

typedef struct client_s {
	connection_t *connection;

	char *name;
	char *country;

	int score;
	int x;
	int y;
	int id;
} client_t;

extern client_t **g_clients;
extern int g_client_ids;
extern int g_client_count;

client_t *client_find(UDPsocket);
client_t *client_create();
void client_print_info(client_t *);

#endif
