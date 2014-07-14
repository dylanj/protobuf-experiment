#ifndef NET_H
#define NET_H

#include "client.h"
#include "protobufs.h"
#include "connection.h"

#include "SDL2/SDL_net.h"


void _net_send_message(int type, connection_t *c, void *data);

// server functions
void net_handle_message(UDPsocket sd, UDPpacket *p);

void _net_handle_handshake(UDPsocket sd, UDPpacket *p, HandshakeMessage *msg);
void _net_handle_input(client_t *c, InputMessage *msg);
void _net_handle_pong(client_t *c, PongMessage *msg);

void _net_send_ping(connection_t *c, int ping);

// client functions
void net_send_handshake(connection_t *, char *name, char *country);
void net_send_input(connection_t *, int key, int press);
void net_client_handle_message(connection_t *, UDPpacket *p);

void _net_client_handle_ping(connection_t *, PingMessage *msg);
void _net_client_send_pong(connection_t *, int pong);

#endif
