#include "input.h"
#include "protobufs.h"

void input_handle_action(client_t *c, int action, int release) {
	printf("%s %s ", c->name, (release==1)?"press":"release");
	switch(action) {
		case INPUT_MESSAGE__ACTION__FORWARD:
			c->y--;
			printf("forward\n");
			break;
		case INPUT_MESSAGE__ACTION__STRAFE_LEFT:
			c->x--;
			printf("left\n");
			break;
		case INPUT_MESSAGE__ACTION__STRAFE_RIGHT:
			c->x++;
			printf("right\n");
			break;
		case INPUT_MESSAGE__ACTION__BACK:
			c->y++;
			printf("backpedal\n");
			break;
		case INPUT_MESSAGE__ACTION__JUMP:
			printf("jumping\n");
			break;
	}

	printf("%d: %s (%d,%d)\n", c->id, c->name, c->x, c->y);
}
