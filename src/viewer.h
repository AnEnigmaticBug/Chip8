#ifndef VIEWER_H
#define VIEWER_H

#include <stdbool.h>
#include <stdint.h>
#include <SDL2/SDL.h>

typedef struct Viewer
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
} Viewer;

void viewer_init(Viewer *self, int wnd_wd, int wnd_ht, int tex_wd, int tex_ht);
bool viewer_process_input(Viewer *self, uint8_t *keypad);
void viewer_update(Viewer *self, uint32_t *buffer);
void viewer_free(Viewer *self);

#endif
