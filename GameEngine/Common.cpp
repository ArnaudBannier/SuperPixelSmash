/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "Common.h"

SDL_Renderer *g_renderer(nullptr);
SDL_Window *g_window(nullptr);

int Game_Init(Uint32 sdlFlags, Uint32 imgFlags, Uint32 mixFlags, int channelCount)
{
    // Initialise la SDL2
    if (SDL_Init(sdlFlags) < 0)
    {
        printf("ERROR - SDL_Init %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    // Initialise la SDL2 image
    if (IMG_Init(imgFlags) != imgFlags)
    {
        printf("ERROR - IMG_Init %s\n", IMG_GetError());
        return EXIT_FAILURE;
    }

    // Initialise la SDL2 ttf
    if (TTF_Init() == -1)
    {
        printf("ERROR - TTF_Init %s\n", TTF_GetError());
        return EXIT_FAILURE;
    }

    // Initialise la SDL2 Mixer
    if (Mix_Init(mixFlags) != mixFlags)
    {
        printf("ERROR - Mix_Init %s\n", Mix_GetError());
        return EXIT_FAILURE;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0)
    {
        printf("ERROR - Mix_OpenAudio %s\n", Mix_GetError());
        return EXIT_FAILURE;
    }
    Mix_AllocateChannels(channelCount);

    return EXIT_SUCCESS;
}

void Game_Quit()
{
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}
