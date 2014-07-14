#include "SDL2/SDL.h"
#include "client.h"
#include "game.h"

void g_init() {
	if (enet_initialize() != 0) {
		fprintf(stderr, "enet_initialize: %s\n", "failure initializing");
		exit(EXIT_FAILURE);

	}

	g_clients = malloc(MAX_CLIENTS * sizeof(client_t *));
}

void g_quit() {
	SDL_Quit();
}

