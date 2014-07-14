#include "net.h"
#include "input.h"
#include "protobufs.h"

/* extern int g_last_ping; */

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
			case WRAPPER_MESSAGE__TYPE__PONG:
				_net_handle_pong(c, msg->pong_message); break;
		}
	}

	wrapper_message__free_unpacked(msg, NULL);
}


void _net_handle_handshake(ENetPeer *peer, HandshakeMessage* msg) {
	client_t *c = client_create();
	c->name = malloc(strlen(msg->name));
	c->country = malloc(strlen(msg->country));

	// client
	strcpy(c->name, msg->name);
	strcpy(c->country, msg->country);

	// network
	c->peer = peer;

	printf("handshake: \n");
	client_print_info(c);
}

/* void net_send_pings(host) { */
/* 	int ticks = SDL_GetTicks(); */
/* 	if (g_last_ping + 5000 < ticks) { */
/* 		printf("sending pings\n"); */
/* 		for( int i = 0; i < MAX_CLIENTS; i++ ) { */
/* 			if (g_clients[i] == NULL) { */
/* 				continue; */
/* 			} */
/* 			_net_send_ping(g_clients[i]->onnection, ticks); */
/* 		} */

/* 		g_last_ping = ticks; */
/* 	} */
/* } */

void _net_handle_pong(client_t *c, PongMessage* msg) {
	printf("PONG: %d\n", msg->pong);
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
		case WRAPPER_MESSAGE__TYPE__PING:
			_net_client_handle_ping(event.peer, msg->ping_message); break;
	}

	wrapper_message__free_unpacked(msg, NULL);
}

void _net_client_handle_ping(ENetPeer *peer, PingMessage *msg) {
	printf("PING: %d\n", msg->ping);
	_net_client_send_pong(peer, msg->ping);
}

void _net_client_send_pong(ENetPeer *peer, int pong) {
	PongMessage msg = PONG_MESSAGE__INIT;

	msg.pong = pong;

	_net_send_message(WRAPPER_MESSAGE__TYPE__PONG, peer, &msg);
}

// sends a protobuf message to the server
// int type: WRAPPER_MESSAGE_TYPE
// connection_t: connectionection_structs
// data: protobuf message of type specified
void _net_send_message(int type, ENetPeer *peer, void *data) {
	WrapperMessage msg = WRAPPER_MESSAGE__INIT;
	ENetPacket *packet;
	int packed_size;
	void *packet_data;

	msg.type = type;

	switch(type) {
		case WRAPPER_MESSAGE__TYPE__HANDSHAKE:
			printf("sending a handshake!!!!!111\n");
			msg.handshake_message = data; break;
		case WRAPPER_MESSAGE__TYPE__INPUT:
			msg.input_message = data; break;
		case WRAPPER_MESSAGE__TYPE__PING:
			msg.ping_message = data; break;
		case WRAPPER_MESSAGE__TYPE__PONG:
			msg.pong_message = data; break;
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
	printf("got a peer %p\n", peer);
	enet_peer_send(peer, 0, packet);

	free(packet_data);
}

/* void _net_send_ping(ENetPeer *peer, int ping) { */
/* 	PingMessage msg = PING_MESSAGE__INIT; */

/* 	msg.ping = ping; */

/* 	_net_send_message(WRAPPER_MESSAGE__TYPE__PING, peer, &msg); */
/* } */

// sends a handshake message to the server to connect
// connection_t *s: server connection struct
// char *name: users name
// char *country: users country
void net_send_handshake(ENetPeer *peer, char *name, char *country) {
	HandshakeMessage msg = HANDSHAKE_MESSAGE__INIT;

	msg.name = name;
	msg.country = country;

	_net_send_message(WRAPPER_MESSAGE__TYPE__HANDSHAKE, peer, &msg);
}

// sends input info to server
// connection_t *s: server connection struct
// int key: keysym pressed
// int press: 1 press, 0 release
void net_send_input(ENetPeer *peer, int action, int press) {
	InputMessage msg = INPUT_MESSAGE__INIT;

	msg.action = action;
	msg.press = press;

	_net_send_message(WRAPPER_MESSAGE__TYPE__INPUT, peer, &msg);
}

