/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "Color.h"

Color::Color() : r(255), g(255), b(255), a(255)
{
};

Color::Color(Uint8 r, Uint8 g, Uint8 b, Uint8 a) :
    r(r), g(g), b(b), a(a)
{
}

Color::Color(SDL_Color color) :
    r(color.r), g(color.g), b(color.b), a(color.a)
{
}

Color Color::Lerp(Color color0, Color color1, float t)
{
    float s = 1.f - t;
    return Color(
        (Uint8)(s * color0.r + t * color1.r),
        (Uint8)(s * color0.g + t * color1.g),
        (Uint8)(s * color0.b + t * color1.b),
        (Uint8)(s * color0.a + t * color1.a)
    );
}

SDL_Color Color::ToSDL() const
{
    SDL_Color color = { 0 };
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;
    return color;
}
