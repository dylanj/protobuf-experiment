#ifndef NET_H
#define NET_H

#include "client.h"
#include "protobufs.h"

#include "SDL2/SDL_net.h"

#define NET_BROADCAST 73
#define NET_SEND 74

void _net_send_message(ENetPeer *peer, int type, void *data);
void _net_send_broadcast_message(ENetHost *host, int type, void *data);
void _net_send_message_packet(void *peer_or_host, int type, void *data, int broadcast);

// server functions
void net_handle_message(ENetEvent event);
void net_disconnect(ENetEvent event);

void _net_handle_handshake(ENetPeer *p, HandshakeMessage *msg);
void _net_handle_input(client_t *c, InputMessage *msg);
void _net_handle_lobby_chat(client_t *c, NET__LobbyChat *msg);

void _net_send_lobby_who(client_t *c);

void _net_broadcast_lobby_join(client_t *c);
void _net_broadcast_lobby_leave(client_t *c, char *reason);

// client functions
void net_client_handle_message(ENetEvent e);

void _net_client_send_handshake(ENetPeer *p, char *name, char *country);
void _net_client_send_input(ENetPeer *p, int key, int press);
void _net_client_send_lobby_chat(ENetPeer *p, int c_id, char *message);

void _net_client_handle_lobby_join(NET__LobbyJoin *msg);
void _net_client_handle_lobby_leave(NET__LobbyLeave *msg);
void _net_client_handle_lobby_chat(NET__LobbyChat *msg);
void _net_client_handle_lobby_who(NET__LobbyWho *msg);

#endif
