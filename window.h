#ifndef WINDOWS_H
#define WINDOWS_H

typedef struct window_s {
	SDL_Window *window;
	SDL_GLContext *context;
	int width;
	int height;
	int bpp;
	int fullscreen;
	int clear_color;

	char* title;
} window_t;

window_t *window_create();
void window_close(window_t *w);
void window_set_size(window_t *w, int width, int height);
void window_set_title(window_t *w, char *title);
void window_set_fullscreen(window_t *w, int fullscreen);

#endif
