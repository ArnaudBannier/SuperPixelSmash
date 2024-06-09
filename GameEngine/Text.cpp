/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "Text.h"
#include <iostream>

using namespace std;

Text::Text(SDL_Renderer *renderer, TTF_Font *font, const std::string &str, SDL_Color color) :
    m_renderer(renderer), m_font(font), m_color(color), m_str(str), m_texture(nullptr)
{
    RefreshTexture();
}

Text::~Text()
{
    if (m_texture)
    {
        SDL_DestroyTexture(m_texture);
    }
}

void Text::SetString(const std::string &str)
{
    if (m_str == str) return;

    m_str.assign(str);
    RefreshTexture();
}

void Text::SetColor(SDL_Color color)
{
    if ((m_color.r == color.r) &&
        (m_color.g == color.g) &&
        (m_color.b == color.b) &&
        (m_color.a == color.a))
        return;

    m_color = color;
    RefreshTexture();
}

void Text::RefreshTexture()
{
    if (m_texture)
    {
        SDL_DestroyTexture(m_texture);
        m_texture = nullptr;
    }
    SDL_Surface *surface = TTF_RenderUTF8_Blended(m_font, m_str.c_str(), m_color);
    AssertNew(surface);
    m_texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    AssertNew(m_texture);

    SDL_FreeSurface(surface);
}
