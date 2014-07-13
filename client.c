#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "protobufs.h"
#include "client.h"
#include "game.h"
#include "net.h"
#include "server_conn.h"

client_t **g_clients;

int g_client_ids = 0;
int g_client_count = 0;

int main(int argc, char **argv) {
	g_init();

	server_conn_t *s = server_conn_create("127.0.0.1", 1234);

	if ( s == NULL ) {
		printf("Couldn't establish connection\n");
		exit(EXIT_FAILURE);
	}

	net_send_handshake(s, "phishtopher", "Canada");
	net_send_input(s, 4, 1);

	SDLNet_Quit();
	return 0;
}
