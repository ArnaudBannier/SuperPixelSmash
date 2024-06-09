/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "Renderer.h"

SDL_Rect Rect_Set(const SDL_Rect *rect, Anchor anchor)
{
    SDL_Rect res = *rect;

    switch (anchor)
    {
    default:
    case Anchor::NORTH_WEST:
        break;
    case Anchor::WEST:
        res.y -= res.h / 2;
        break;
    case Anchor::SOUTH_WEST:
        res.y -= res.h;
        break;
    case Anchor::NORTH:
        res.x -= res.w / 2;
        break;
    case Anchor::CENTER:
        res.x -= res.w / 2;
        res.y -= res.h / 2;
        break;
    case Anchor::SOUTH:
        res.x -= res.w / 2;
        res.y -= res.h;
        break;
    case Anchor::NORTH_EAST:
        res.x -= res.w;
        break;
    case Anchor::EAST:
        res.x -= res.w;
        res.y -= res.h / 2;
        break;
    case Anchor::SOUTH_EAST:
        res.x -= res.w;
        res.y -= res.h;
        break;
    }

    return res;
}

SDL_FRect FRect_Set(const SDL_FRect *rect, Anchor anchor)
{
    SDL_FRect res = *rect;

    switch (anchor)
    {
    case Anchor::NORTH_WEST:
        break;
    case Anchor::WEST:
        res.y -= res.h / 2;
        break;
    case Anchor::SOUTH_WEST:
        res.y -= res.h;
        break;
    case Anchor::NORTH:
        res.x -= res.w / 2;
        break;
    case Anchor::CENTER:
        res.x -= res.w / 2;
        res.y -= res.h / 2;
        break;
    case Anchor::SOUTH:
        res.x -= res.w / 2;
        res.y -= res.h;
        break;
    case Anchor::NORTH_EAST:
        res.x -= res.w;
        break;
    case Anchor::EAST:
        res.x -= res.w;
        res.y -= res.h / 2;
        break;
    case Anchor::SOUTH_EAST:
        res.x -= res.w;
        res.y -= res.h;
        break;
    default:
        break;
    }

    return res;
}

int RenderCopy(
    SDL_Renderer *renderer, SDL_Texture *texture,
    const SDL_Rect *srcRect, const SDL_Rect *dstRect, Anchor anchor)
{
    SDL_Rect dst = Rect_Set(dstRect, anchor);
    return SDL_RenderCopy(renderer, texture, srcRect, &dst);
}

int RenderCopyEx(
    SDL_Renderer *renderer, SDL_Texture *texture,
    const SDL_Rect *srcRect, const SDL_Rect *dstRect, Anchor anchor,
    const double angle, b2Vec2 center, SDL_RendererFlip flip)
{
    SDL_Rect dst = Rect_Set(dstRect, anchor);
    SDL_Point sdlCenter = {
        (int)(dstRect->w * center.x),
        (int)(dstRect->h * (1.f - center.y))
    };
    return SDL_RenderCopyEx(renderer, texture, srcRect, &dst, angle, &sdlCenter, flip);
}

int RenderCopyF(
    SDL_Renderer *renderer, SDL_Texture *texture,
    const SDL_Rect *srcRect, const SDL_FRect *dstRect, Anchor anchor)
{
    SDL_FRect dst = FRect_Set(dstRect, anchor);
    return SDL_RenderCopyF(renderer, texture, srcRect, &dst);
}

int RenderCopyExF(
    SDL_Renderer *renderer, SDL_Texture *texture,
    const SDL_Rect *srcRect, const SDL_FRect *dstRect, Anchor anchor,
    const double angle, b2Vec2 center, SDL_RendererFlip flip)
{
    SDL_FRect dst = FRect_Set(dstRect, anchor);
    SDL_FPoint sdlCenter = {
        dstRect->w * center.x,
        dstRect->h * (1.f - center.y)
    };
    return SDL_RenderCopyExF(renderer, texture, srcRect, &dst, angle, &sdlCenter, flip);
}

int RenderCopyBordersF(
    SDL_Renderer *renderer, SDL_Texture *texture,
    const SDL_Rect *srcRect, const SDL_FRect *dstRect, Anchor anchor,
    int l, int r, int t, int b, float scale)
{
    SDL_FRect dstA = FRect_Set(dstRect, anchor);

    int srcW[3] = { l, srcRect->w - l - r, r };
    int srcH[3] = { t, srcRect->h - t - b, b };
    int srcX[3] = { srcRect->x, srcRect->x + l, srcRect->x + srcRect->w - r };
    int srcY[3] = { srcRect->y, srcRect->y + t, srcRect->y + srcRect->h - b };
    float dstL = scale * (float)l;
    float dstR = scale * (float)r;
    float dstT = scale * (float)t;
    float dstB = scale * (float)b;

    float dstW[3] = { dstL, dstA.w - dstL - dstR, dstR};
    float dstH[3] = { dstT, dstA.h - dstT - dstB, dstB};
    float dstX[3] = { dstA.x, dstA.x + dstL, dstA.x + dstA.w - dstR };
    float dstY[3] = { dstA.y, dstA.y + dstT, dstA.y + dstA.h - dstB };

    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            SDL_Rect src = { 0 };
            src.x = srcX[i];
            src.y = srcY[j];
            src.w = srcW[i];
            src.h = srcH[j];

            SDL_FRect dst = { 0 };
            dst.x = dstX[i];
            dst.y = dstY[j];
            dst.w = dstW[i];
            dst.h = dstH[j];

            SDL_RenderCopyF(renderer, texture, &src, &dst);
        }
    }

    return 0;
}
