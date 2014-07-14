#include "SDL2/SDL.h"
#include "SDL2/SDL_net.h"
#include "client.h"
#include "game.h"

void g_init() {
	if ( SDLNet_Init() < 0 ) {
		fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}

	g_clients = malloc(MAX_CLIENTS * sizeof(client_t *));
}

void g_quit() {
	SDL_Quit();
	SDLNet_Quit();
}

