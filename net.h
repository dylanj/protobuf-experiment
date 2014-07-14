#ifndef NET_H
#define NET_H

#include "client.h"
#include "protobufs.h"

#include "SDL2/SDL_net.h"

void _net_send_message(int type, ENetPeer *p, void *data);

// server functions
void net_handle_message(ENetEvent event);
void _net_handle_handshake(ENetPeer *p, HandshakeMessage *msg);
void _net_handle_input(client_t *c, InputMessage *msg);
void _net_handle_pong(client_t *c, PongMessage *msg);

/* void _net_send_ping(ENetPeer *pc, int ping); */
/* void net_send_pings(); */

// client functions
void net_send_handshake(ENetPeer *p, char *name, char *country);
void net_send_input(ENetPeer *p, int key, int press);
void net_client_handle_message(ENetEvent e);

void _net_client_handle_ping(ENetPeer *p, PingMessage *msg);
void _net_client_send_pong(ENetPeer *p, int pong);

#endif
