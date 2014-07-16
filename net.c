#include "net.h"
#include "input.h"
#include "protobufs.h"

void net_disconnect(ENetEvent event) {
	printf("%s disconnected.\n", event.peer->data);

	event.peer->data = NULL;
}

void net_handle_message(ENetEvent event) {
	WrapperMessage *msg;
	client_t *c = client_find(event.peer);

	if ( event.packet->dataLength == 0 ){
		printf("no len packet\n");
		return;
	}

	msg = wrapper_message__unpack(
		NULL,
		event.packet->dataLength,
		event.packet->data
	);

	if ( msg == NULL ) {
		fprintf(stderr, "Error decoding message\n");
		exit(EXIT_FAILURE);
	}

	if ( c == NULL ) {
		switch(msg->type) {
			case WRAPPER_MESSAGE__TYPE__HANDSHAKE:
				_net_handle_handshake(event.peer, msg->handshake_message); break;
		}
	} else {
		switch(msg->type) {
			case WRAPPER_MESSAGE__TYPE__INPUT:
				_net_handle_input(c, msg->input_message); break;
			case WRAPPER_MESSAGE__TYPE__LOBBY_CHAT:
				_net_handle_lobby_chat(c, msg->lobby_chat_message); break;
		}
	}

	wrapper_message__free_unpacked(msg, NULL);

	enet_packet_destroy (event.packet);
}

// client c has said something in the lobby
void _net_handle_lobby_chat(client_t *c, NET__LobbyChat *msg) {
	/* NET__LobbyChat msg = NET__LOBBY_CHAT__INIT; */

	/* msg.id = c_id; */
	/* msg.message = message; */

	_net_send_broadcast_message(c->peer->host, WRAPPER_MESSAGE__TYPE__LOBBY_CHAT, msg);
}

void _net_handle_handshake(ENetPeer *peer, HandshakeMessage* msg) {
	client_t *c = client_create();
	c->id = g_client_ids++;
	c->name = malloc(strlen(msg->name));
	c->country = malloc(strlen(msg->country));

	// client
	strcpy(c->name, msg->name);
	strcpy(c->country, msg->country);

	// network
	c->peer = peer;

	// let user know who's in the lobby
	_net_send_lobby_who(c);
}

void _net_send_lobby_who(client_t *c) {
	NET__LobbyWho msg = NET__LOBBY_WHO__INIT;

	client_t *tmp_client;
	int c_id = -1;

	printf("looping over %d clients\n", g_client_count);

	msg.n_clients = g_client_count;
	msg.clients = malloc(g_client_count * sizeof(NET__Client*));

	for(int i = 0, j = 0; i < g_client_count; i++) {
		tmp_client = g_clients[i];

		if ((tmp_client = g_clients[i]) == NULL) {
			continue;
		}

		c_id = j++;

		msg.clients[c_id] = malloc(sizeof(NET__Client));
		net__client__init(msg.clients[c_id]);
		msg.clients[c_id]->name = tmp_client->name;
		msg.clients[c_id]->id = tmp_client->id;

		printf("creating %d client\n", c_id);
	}

	_net_send_message(c->peer, WRAPPER_MESSAGE__TYPE__LOBBY_WHO, &msg);

	for(int i = 0; i < c_id; i++ ) {
		printf("freeing %d client\n", i+1);
		free(msg.clients[i]);
	}

	free(msg.clients);
}

void _net_broadcast_lobby_join(client_t *c) {
	NET__LobbyJoin msg = NET__LOBBY_JOIN__INIT;

	msg.id = c->id;

	_net_send_broadcast_message(c->peer->host, WRAPPER_MESSAGE__TYPE__LOBBY_JOIN, &msg);
}

void _net_broadcast_lobby_leave(client_t *c, char *reason) {
	NET__LobbyLeave msg = NET__LOBBY_JOIN__INIT;

	msg.id = c->id;
	if ( reason != NULL ) {
		msg.reason = reason;
	}

	_net_send_broadcast_message(c->peer->host, WRAPPER_MESSAGE__TYPE__LOBBY_LEAVE, &msg);
}

void _net_handle_input(client_t *c, InputMessage* msg) {
	input_handle_action(c, msg->action, msg->press);
}

void net_client_handle_message(ENetEvent event) {
	WrapperMessage *msg;

	msg = wrapper_message__unpack(
		NULL,
		event.packet->dataLength,
		event.packet->data
	);

	if ( msg == NULL ) {
		fprintf(stderr, "Error decoding message\n");
		exit(EXIT_FAILURE);
	}

	switch(msg->type) {
		case WRAPPER_MESSAGE__TYPE__LOBBY_JOIN:
			_net_client_handle_lobby_join(msg->lobby_join_message); break;
		case WRAPPER_MESSAGE__TYPE__LOBBY_LEAVE:
			_net_client_handle_lobby_leave(msg->lobby_leave_message); break;
		case WRAPPER_MESSAGE__TYPE__LOBBY_CHAT:
			_net_client_handle_lobby_chat(msg->lobby_chat_message); break;
		case WRAPPER_MESSAGE__TYPE__LOBBY_WHO:
			_net_client_handle_lobby_who(msg->lobby_who_message); break;
	}

	wrapper_message__free_unpacked(msg, NULL);
}

void _net_client_handle_lobby_join(NET__LobbyJoin* msg) {
	client_t *c = client_find_by_id(msg->id);
	printf("%s: has joined the lobby\n", c->name);
}

void _net_client_handle_lobby_leave(NET__LobbyLeave* msg) {
	client_t *c = client_find_by_id(msg->id);
	printf("%s: has left the lobby\n", c->name);
}

void _net_client_handle_lobby_chat(NET__LobbyChat* msg) {
	client_t *c = client_find_by_id(msg->id);
	printf("%s: %s\n", c->name, msg->message);
}

void _net_client_handle_lobby_who(NET__LobbyWho* msg) {
	NET__Client **clients = msg->clients;
	NET__Client *c;
	printf("%d clients:\n", msg->n_clients);

	for (int i = 0; i < msg->n_clients; i++ ) {
		c = msg->clients[i];
		printf("%d|%s\n", c->id, c->name);
	}
}

void _net_send_message_packet(void *peer_or_host, int type, void *data, int broadcast) {
	WrapperMessage msg = WRAPPER_MESSAGE__INIT;
	ENetPacket *packet;
	int packed_size;
	void *packet_data;

	msg.type = type;

	switch(type) {
		case WRAPPER_MESSAGE__TYPE__HANDSHAKE:
			msg.handshake_message = data; break;
		case WRAPPER_MESSAGE__TYPE__INPUT:
			msg.input_message = data; break;
		case WRAPPER_MESSAGE__TYPE__LOBBY_JOIN:
			msg.lobby_join_message = data; break;
		case WRAPPER_MESSAGE__TYPE__LOBBY_LEAVE:
			msg.lobby_leave_message = data; break;
		case WRAPPER_MESSAGE__TYPE__LOBBY_CHAT:
			msg.lobby_chat_message = data; break;
		case WRAPPER_MESSAGE__TYPE__LOBBY_WHO:
			msg.lobby_who_message = data; break;
	}

	packed_size = wrapper_message__get_packed_size(&msg);
  packet_data = malloc(packed_size);
	wrapper_message__pack(&msg, packet_data);

	printf("packed_size: %d\n", packed_size);

	packet = enet_packet_create(
		packet_data,
 		packed_size,
		ENET_PACKET_FLAG_RELIABLE
	);

	printf("got a packet %p\n", packet);
	printf("got a peer %p\n", peer_or_host);

	if (broadcast == NET_BROADCAST) {
		enet_host_broadcast(peer_or_host, 0, packet);
	} else if ( broadcast == NET_SEND ) {
		enet_peer_send(peer_or_host, 0, packet);
	} else {
		printf("_net_send_message_packet receiving NULL value for broadcast\n");
	}

	free(packet_data);
}

void _net_send_message(ENetPeer *peer, int type, void *data) {
	_net_send_message_packet(peer, type, data, NET_SEND);
}

void _net_send_broadcast_message(ENetHost *host, int type, void *data) {
	_net_send_message_packet(host, type, data, NET_BROADCAST);
}

void _net_client_send_handshake(ENetPeer *peer, char *name, char *country) {
	HandshakeMessage msg = HANDSHAKE_MESSAGE__INIT;

	msg.name = name;
	msg.country = country;

	_net_send_message(peer, WRAPPER_MESSAGE__TYPE__HANDSHAKE, &msg);
}

void _net_client_send_input(ENetPeer *peer, int action, int press) {
	InputMessage msg = INPUT_MESSAGE__INIT;

	msg.action = action;
	msg.press = press;

	_net_send_message(peer, WRAPPER_MESSAGE__TYPE__INPUT, &msg);
}

void _net_client_send_lobby_chat(ENetPeer *peer, int c_id, char *message) {
	NET__LobbyChat msg = NET__LOBBY_CHAT__INIT;

	msg.id = c_id;
	msg.message = message;

	_net_send_message(peer, WRAPPER_MESSAGE__TYPE__LOBBY_CHAT, &msg);
}

