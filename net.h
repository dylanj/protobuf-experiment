#ifndef NET_H
#define NET_H

#include "client.h"
#include "protobufs.h"
#include "server_conn.h"
#include "SDL2/SDL_net.h"

// server functions
void net_handle_message(UDPsocket sd, UDPpacket *p);

void _net_handle_handshake(UDPsocket sd, UDPpacket *p, HandshakeMessage* msg);
void _net_handle_input(client_t *c, InputMessage* msg);

// client functions
void net_send_handshake(server_conn_t *, char *name, char *country);
void net_send_input(server_conn_t *, int key, int press);

#endif
