#ifndef SERVER_CONN_H
#define SERVER_CONN_H

typedef struct server_conn_s {
	char *host;
	int port;
	IPaddress ip;
	UDPsocket socket;
} server_conn_t;

server_conn_t *server_conn_create(char *host, int port);

#endif
