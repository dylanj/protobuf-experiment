#include "net.h"
#include "protobufs.h"

extern int g_last_ping;

void _net_handle_handshake(UDPsocket sd, UDPpacket *p, HandshakeMessage* msg) {
	client_t *c = client_create();
	c->name = malloc(strlen(msg->name));
	c->country = malloc(strlen(msg->country));

	// client
	strcpy(c->name, msg->name);
	strcpy(c->country, msg->country);

	// network
	c->connection->ip.host = p->address.host;
	c->connection->ip.port = p->address.port;
	c->connection->socket = sd;

	printf("handshake: \n");
	client_print_info(c);
}

void net_send_pings() {
	int ticks = SDL_GetTicks();
	if (g_last_ping + 5000 < ticks) {
		printf("sending pings\n");
		for( int i = 0; i < MAX_CLIENTS; i++ ) {
			if (g_clients[i] == NULL) {
				continue;
			}
			_net_send_ping(g_clients[i]->connection, ticks);
		}

		g_last_ping = ticks;
	}
}

void _net_handle_pong(client_t *c, PongMessage* msg) {
	printf("PONG: %d\n", msg->pong);

	c->connection->missed_pings--;
	c->connection->last_pong = msg->pong;
}

void _net_handle_input(client_t *c, InputMessage* msg) {
	printf("%s is moving! ", c->name);

	switch(msg->action) {
		case INPUT_MESSAGE__ACTION__FORWARD:
			printf("forward\n");
			break;
		case INPUT_MESSAGE__ACTION__STRAFE_LEFT:
			printf("left\n");
			break;
		case INPUT_MESSAGE__ACTION__STRAFE_RIGHT:
			printf("right\n");
			break;
		case INPUT_MESSAGE__ACTION__BACK:
			printf("backpedal\n");
			break;
		case INPUT_MESSAGE__ACTION__JUMP:
			printf("jumping\n");
			break;
	}
}

void net_handle_message(UDPsocket sd, UDPpacket *p) {
	WrapperMessage *msg;
	client_t *c = client_find(sd);

	msg = wrapper_message__unpack(NULL, p->len, p->data);

	if ( msg == NULL ) {
		fprintf(stderr, "Error decoding message\n");
		exit(EXIT_FAILURE);
	}

	if ( c == NULL ) {
		switch(msg->type) {
			case WRAPPER_MESSAGE__TYPE__HANDSHAKE:
				_net_handle_handshake(sd, p, msg->handshake_message); break;
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

void net_client_handle_message(connection_t *c, UDPpacket *p) {
	WrapperMessage *msg;

	msg = wrapper_message__unpack(NULL, p->len, p->data);

	if ( msg == NULL ) {
		fprintf(stderr, "Error decoding message\n");
		exit(EXIT_FAILURE);
	}

	switch(msg->type) {
		case WRAPPER_MESSAGE__TYPE__PING:
			_net_client_handle_ping(c, msg->ping_message); break;
	}

	wrapper_message__free_unpacked(msg, NULL);
}

void _net_client_handle_ping(connection_t *c, PingMessage *msg) {
	printf("PING: %d\n", msg->ping);
	_net_client_send_pong(c, msg->ping);
}

void _net_client_send_pong(connection_t *c, int pong) {
	PongMessage msg = PONG_MESSAGE__INIT;

	msg.pong = pong;

	_net_send_message(WRAPPER_MESSAGE__TYPE__PONG, c, &msg);
}

// sends a protobuf message to the server
// int type: WRAPPER_MESSAGE_TYPE
// connection_t: connectionection_structs
// data: protobuf message of type specified
void _net_send_message(int type, connection_t *c, void *data) {
	WrapperMessage msg = WRAPPER_MESSAGE__INIT;
	UDPpacket *p;

	if (!(p = SDLNet_AllocPacket(1025))) {
		fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}

	msg.type = type;
	switch(type) {
		case WRAPPER_MESSAGE__TYPE__HANDSHAKE:
			msg.handshake_message = data; break;
		case WRAPPER_MESSAGE__TYPE__INPUT:
			msg.input_message = data; break;
		case WRAPPER_MESSAGE__TYPE__PING:
			msg.ping_message = data; break;
		case WRAPPER_MESSAGE__TYPE__PONG:
			msg.pong_message = data; break;
	}

	wrapper_message__pack(&msg, p->data);
	p->len = wrapper_message__get_packed_size(&msg);

	p->address.host = c->ip.host;
	p->address.port = c->ip.port;

	if (SDLNet_UDP_Send(c->socket, -1, p) < 0) {
		fprintf(stderr, "SDLNet_UDP_Send: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}

	SDLNet_FreePacket(p);
}

void _net_send_ping(connection_t *c, int ping) {
	PingMessage msg = PING_MESSAGE__INIT;

	msg.ping = ping;
	c->missed_pings++;
	if ( c->missed_pings >= 4 ) {
		printf("client timedout\n");
	}

	_net_send_message(WRAPPER_MESSAGE__TYPE__PING, c, &msg);
}

// sends a handshake message to the server to connect
// connection_t *s: server connection struct
// char *name: users name
// char *country: users country
void net_send_handshake(connection_t *c, char *name, char *country) {
	HandshakeMessage msg = HANDSHAKE_MESSAGE__INIT;

	msg.name = name;
	msg.country = country;

	_net_send_message(WRAPPER_MESSAGE__TYPE__HANDSHAKE, c, &msg);

	printf("sent a handshake\n");
}

// sends input info to server
// connection_t *s: server connection struct
// int key: keysym pressed
// int press: 1 press, 0 release
void net_send_input(connection_t *c, int action, int press) {
	InputMessage msg = INPUT_MESSAGE__INIT;

	msg.action = action;
	msg.press = press;

	_net_send_message(WRAPPER_MESSAGE__TYPE__INPUT, c, &msg);
}

