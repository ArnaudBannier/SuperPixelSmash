/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "Settings.h"

extern SDL_Renderer *g_renderer;
extern SDL_Window *g_window;

/// @brief Initialise les librairies utilisées par le jeu.
/// @param sdlFlags les flags pour la librairie SDL.
/// @param imgFlags les flags pour la librairie SDL UIImage.
/// @param mixFlags les flags pour la librairie SDL Mixer.
/// @param channelCount le nombre de canaux audio.
int Game_Init(Uint32 sdlFlags, Uint32 imgFlags, Uint32 mixFlags, int channelCount);

/// @brief Quitte les librairies utilisées par le jeu.
void Game_Quit();
