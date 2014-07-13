#include "net.h"
#include "protobufs.h"

void _net_handle_handshake(UDPsocket sd, UDPpacket *p, HandshakeMessage* msg) {
	client_t *c = client_create();
	c->name = malloc(strlen(msg->name));
	c->country = malloc(strlen(msg->country));

	// client
	strcpy(c->name, msg->name);
	strcpy(c->country, msg->country);

	// network
	c->ip.host = p->address.host;
	c->ip.port = p->address.port;
	c->socket = sd;

	printf("handshake: \n");
	client_print_info(c);
}

void _net_handle_input(client_t *c, InputMessage* msg) {
	printf("input: \n");

	client_print_info(c);
	printf("key: %d\n", msg->key);
	printf("press: %d\n", msg->press);

	printf("%s is moving!\n", c->name);
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
		}
	}

	wrapper_message__free_unpacked(msg, NULL);
}

void _net_send_message(int type, server_conn_t *s, void *data) {
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
	}

	wrapper_message__pack(&msg, p->data);
	p->len = wrapper_message__get_packed_size(&msg);

	p->address.host = s->ip.host;
	p->address.port = s->ip.port;

	if (SDLNet_UDP_Send(s->socket, -1, p) < 0) {
		fprintf(stderr, "SDLNet_UDP_Send: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}

	SDLNet_FreePacket(p);
}

void net_send_handshake(server_conn_t *s, char *name, char *country) {
	HandshakeMessage msg = HANDSHAKE_MESSAGE__INIT;

	msg.name = name;
	msg.country = country;

	_net_send_message(WRAPPER_MESSAGE__TYPE__HANDSHAKE, s, &msg);

	printf("sent a handshake\n");
}

void net_send_input(server_conn_t *s, int key, int press) {
	InputMessage msg = INPUT_MESSAGE__INIT;

	msg.key = key;
	msg.press = press;

	_net_send_message(WRAPPER_MESSAGE__TYPE__INPUT, s, &msg);
}

