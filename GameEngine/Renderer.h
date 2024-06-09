/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "Settings.h"

enum class Anchor : uint32_t
{
    NORTH_WEST, NORTH_EAST, NORTH,
    EAST, CENTER, WEST,
    SOUTH_WEST, SOUTH, SOUTH_EAST
};

int RenderCopy(
    SDL_Renderer *renderer, SDL_Texture *texture,
    const SDL_Rect *srcRect, const SDL_Rect *dstRect, Anchor anchor
);

int RenderCopyF(
    SDL_Renderer *renderer, SDL_Texture *texture,
    const SDL_Rect *srcRect, const SDL_FRect *dstRect, Anchor anchor
);

int RenderCopyEx(
    SDL_Renderer *renderer, SDL_Texture *texture,
    const SDL_Rect *srcRect, const SDL_Rect *dstRect, Anchor anchor,
    const double angle, b2Vec2 center, SDL_RendererFlip flip
);

int RenderCopyExF(
    SDL_Renderer *renderer, SDL_Texture *texture,
    const SDL_Rect *srcRect, const SDL_FRect *dstRect, Anchor anchor,
    const double angle, b2Vec2 center, SDL_RendererFlip flip
);

int RenderCopyBordersF(
    SDL_Renderer *renderer, SDL_Texture *texture,
    const SDL_Rect *srcRect, const SDL_FRect *dstRect, Anchor anchor,
    int l, int r, int t, int b, float scale
);
