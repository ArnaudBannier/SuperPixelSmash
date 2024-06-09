/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "Settings.h"

class Text
{
public:

    Text(SDL_Renderer *renderer, TTF_Font *font, const std::string &str, SDL_Color color);
    Text(Text const&) = delete;
    Text& operator=(Text const&) = delete;
    ~Text();

    void SetString(const std::string &str);
    void SetColor(SDL_Color color);
    const std::string &GetString() const;
    SDL_Texture *GetTexture();

protected:

    void RefreshTexture();

    std::string m_str;
    TTF_Font *m_font;
    SDL_Color m_color;
    SDL_Texture *m_texture;
    SDL_Renderer *m_renderer;
};

inline const std::string &Text::GetString() const
{
    return m_str;
}

inline SDL_Texture *Text::GetTexture()
{
    return m_texture;
}
