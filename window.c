#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>

#include "window.h"

void window_set_size(window_t *w, int width, int height) {
	w->width = width;
	w->height = height;

	if (w->window) {
		SDL_SetWindowSize(w->window, w->width, w->height);
	}
}

void window_set_title(window_t *w, char *title) {
	if (w->title == NULL) {
		printf("title null, allocating memory\n");
		w->title = malloc(128);
	}

	strcpy(w->title, title);

	printf("title is %s\n", w->title);

	if (w->window) {
		SDL_SetWindowTitle(w->window, w->title);
	}
}

void window_set_fullscreen(window_t *w, int fullscreen) {
	w->fullscreen = fullscreen;
	SDL_SetWindowFullscreen(w->window, fullscreen);
}

window_t *window_init() {
	window_t *w = malloc(sizeof(window_t));
	w->width = 0;
	w->height = 0;
	w->bpp = 0;
	w->fullscreen = 0;
	w->clear_color = 0;

	w->title = NULL;
	w->window = NULL;
	w->context = NULL;

	return w;
}

window_t *window_create() {
	window_t *w = window_init();

	window_set_size(w, 640, 480);
	window_set_title(w, "foobar");
	window_set_fullscreen(w, 0);

	w->window = SDL_CreateWindow(
		w->title,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		w->width,
		w->height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
	);

	if ( w->window == NULL ) {
		fprintf(stderr, "Couldn't created window.\n");
		window_close(w);
		return NULL;
	}

	w->context = SDL_GL_CreateContext(w->window);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetSwapInterval(1);

	glClearColor(0, 0, 0, 1);

	return w;
}

void window_close(window_t *w) {
	if ( w == NULL ) {
		return;
	}

	if ( w->context != NULL ) {
		SDL_GL_DeleteContext(w->context);
	}
	if ( w->window != NULL ) {
		SDL_DestroyWindow(w->window);
	}

	free(w);
}
