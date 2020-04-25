#include "viewer.h"
#include "chip8.h"

#define PITCH sizeof(uint32_t) * SCR_WD

void viewer_init(Viewer *self, int wnd_wd, int wnd_ht, int tex_wd, int tex_ht)
{
    SDL_Init(SDL_INIT_VIDEO);

    self->window = SDL_CreateWindow("Chip8 Emulator", 0, 0, wnd_wd, wnd_ht, SDL_WINDOW_SHOWN);
    self->renderer = SDL_CreateRenderer(self->window, -1, SDL_RENDERER_ACCELERATED);
    self->texture = SDL_CreateTexture(self->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, tex_wd, tex_ht);
}

bool viewer_process_input(Viewer *self, uint8_t *keypad)
{
    bool quit = false;

    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
        {
            quit = true;
            break;
        }
        case SDL_KEYDOWN:
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
            {
                quit = true;
                break;
            }
            case SDLK_x:
            {
                keypad[0] = 1;
                break;
            }
            case SDLK_1:
            {
                keypad[1] = 1;
                break;
            }
            case SDLK_2:
            {
                keypad[2] = 1;
                break;
            }
            case SDLK_3:
            {
                keypad[3] = 1;
                break;
            }
            case SDLK_q:
            {
                keypad[4] = 1;
                break;
            }
            case SDLK_w:
            {
                keypad[5] = 1;
                break;
            }
            case SDLK_e:
            {
                keypad[6] = 1;
                break;
            }
            case SDLK_a:
            {
                keypad[7] = 1;
                break;
            }
            case SDLK_s:
            {
                keypad[8] = 1;
                break;
            }
            case SDLK_d:
            {
                keypad[9] = 1;
                break;
            }
            case SDLK_z:
            {
                keypad[0xA] = 1;
                break;
            }
            case SDLK_c:
            {
                keypad[0xB] = 1;
                break;
            }
            case SDLK_4:
            {
                keypad[0xC] = 1;
                break;
            }
            case SDLK_r:
            {
                keypad[0xD] = 1;
                break;
            }
            case SDLK_f:
            {
                keypad[0xE] = 1;
                break;
            }
            case SDLK_v:
            {
                keypad[0xF] = 1;
                break;
            }
            }
            break;
        }
        case SDL_KEYUP:
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
            {
                quit = true;
                break;
            }
            case SDLK_x:
            {
                keypad[0] = 0;
                break;
            }
            case SDLK_1:
            {
                keypad[1] = 0;
                break;
            }
            case SDLK_2:
            {
                keypad[2] = 0;
                break;
            }
            case SDLK_3:
            {
                keypad[3] = 0;
                break;
            }
            case SDLK_q:
            {
                keypad[4] = 0;
                break;
            }
            case SDLK_w:
            {
                keypad[5] = 0;
                break;
            }
            case SDLK_e:
            {
                keypad[6] = 0;
                break;
            }
            case SDLK_a:
            {
                keypad[7] = 0;
                break;
            }
            case SDLK_s:
            {
                keypad[8] = 0;
                break;
            }
            case SDLK_d:
            {
                keypad[9] = 0;
                break;
            }
            case SDLK_z:
            {
                keypad[0xA] = 0;
                break;
            }
            case SDLK_c:
            {
                keypad[0xB] = 0;
                break;
            }
            case SDLK_4:
            {
                keypad[0xC] = 0;
                break;
            }
            case SDLK_r:
            {
                keypad[0xD] = 0;
                break;
            }
            case SDLK_f:
            {
                keypad[0xE] = 0;
                break;
            }
            case SDLK_v:
            {
                keypad[0xF] = 0;
                break;
            }
            }
            break;
        }
        }
    }

    return quit;
}

void viewer_update(Viewer *self, uint32_t *buffer)
{
    SDL_UpdateTexture(self->texture, NULL, buffer, PITCH);
    SDL_RenderClear(self->renderer);
    SDL_RenderCopy(self->renderer, self->texture, NULL, NULL);
    SDL_RenderPresent(self->renderer);
}

void viewer_free(Viewer *self)
{
    SDL_DestroyTexture(self->texture);
    SDL_DestroyRenderer(self->renderer);
    SDL_DestroyWindow(self->window);
    SDL_Quit();
}
