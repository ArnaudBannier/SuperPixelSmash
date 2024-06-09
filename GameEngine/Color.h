/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "Settings.h"
#include "Common.h"

struct Color
{
    Color();
    Color(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255);
    Color(SDL_Color color);

    void Set(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255);
    void Set(SDL_Color color);
    void SetAsRenderColor() const;

    static Color Lerp(Color color0, Color color1, float t);
    SDL_Color ToSDL() const;

    Uint8 r, g, b, a;

};

inline void Color::Set(Uint8 rIn, Uint8 gIn, Uint8 bIn, Uint8 aIn)
{
    r = rIn; g = gIn; b = bIn; a = aIn;
}

inline void Color::Set(SDL_Color color)
{
    r = color.r; g = color.g; b = color.b; a = color.a;
}

inline void Color::SetAsRenderColor() const
{
    SDL_SetRenderDrawColor(g_renderer, r, g, b, a);
}

inline Color operator + (const Color& colorA, const Color& colorB)
{
    return Color(
        colorA.r + colorB.r,
        colorA.g + colorB.g,
        colorA.b + colorB.b,
        colorA.a + colorB.a
    );
}

inline Color operator - (const Color& colorA, const Color& colorB)
{
    return Color(
        colorA.r - colorB.r,
        colorA.g - colorB.g,
        colorA.b - colorB.b,
        colorA.a - colorB.a
    );
}

inline Color operator * (const Color& colorA, const Color& colorB)
{
    return Color(
        colorA.r * colorB.r,
        colorA.g * colorB.g,
        colorA.b * colorB.b,
        colorA.a * colorB.a
    );
}

inline Color operator * (float s, const Color& color)
{
    return Color(
        (Uint8)(s * color.r),
        (Uint8)(s * color.g),
        (Uint8)(s * color.b),
        (Uint8)(s * color.a)
    );
}

