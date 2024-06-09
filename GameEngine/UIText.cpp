/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "UIText.h"
#include "Scene.h"

UIText::UIText(Scene *scene, const std::string &str, TTF_Font *font, Color color) :
    UIObject(scene), m_text(g_renderer, font, str, color.ToSDL()),
    m_anchor(Anchor::CENTER), m_renderMode(RenderMode::ANCHOR),
    m_pixelW(0), m_pixelH(0)
{
    SetName("UIText");
    SetColor(color);
    SDL_Texture *texture = m_text.GetTexture();
    SDL_QueryTexture(texture, NULL, NULL, &m_pixelW, &m_pixelH);
}

UIText::~UIText()
{
}

void UIText::SetString(const std::string &str)
{
    m_text.SetString(str);
    SDL_Texture *texture = m_text.GetTexture();
    SDL_QueryTexture(texture, NULL, NULL, &m_pixelW, &m_pixelH);
}

void UIText::Render()
{
    if (IsUIEnabled() == false) return;
    SDL_Texture *texture = m_text.GetTexture();
    if (texture == nullptr) return;

    float fadeAlpha = GetAlpha();
    SDL_SetTextureAlphaMod(texture, (Uint8)(fadeAlpha * 255));

    SDL_FRect dstRect = GetRenderRect();
    if (m_renderMode == RenderMode::STRETCH)
    {
        SDL_RenderCopyF(g_renderer, texture, NULL, &dstRect);
    }
    else
    {
        switch (m_anchor)
        {
        case Anchor::NORTH_WEST:
            break;
        case Anchor::NORTH:
            dstRect.x += 0.5f * dstRect.w;
            break;
        case Anchor::NORTH_EAST:
            dstRect.x += dstRect.w;
            break;
        case Anchor::WEST:
            dstRect.y += 0.5f * dstRect.h;
            break;
        case Anchor::CENTER:
            dstRect.x += 0.5f * dstRect.w;
            dstRect.y += 0.5f * dstRect.h;
            break;
        case Anchor::EAST:
            dstRect.x += dstRect.w;
            dstRect.y += 0.5f * dstRect.h;
            break;
        case Anchor::SOUTH_WEST:
            dstRect.y += dstRect.h;
            break;
        case Anchor::SOUTH:
            dstRect.x += 0.5f * dstRect.w;
            dstRect.y += dstRect.h;
            break;
        case Anchor::SOUTH_EAST:
            dstRect.x += dstRect.w;
            dstRect.y += dstRect.h;
            break;
        default:
            break;
        }
        dstRect.w = (float)m_pixelW;
        dstRect.h = (float)m_pixelH;

        RenderCopyF(g_renderer, texture, NULL, &dstRect, m_anchor);
    }
}

void UIText::Update()
{
    UIObject::Update();
    SetVisible(true);

    m_text.SetColor(GetColor().ToSDL());
}

void UIText::GetNativePixelSize(int &pixelWidth, int &pixelHeight) const
{
    pixelWidth = m_pixelW;
    pixelHeight = m_pixelH;
}

b2Vec2 UIText::GetNativeUISize() const
{
    float pixelsPerUnit = m_scene->GetUIPixelsPerUnit();
    return 1.0f / pixelsPerUnit * b2Vec2((float)m_pixelW, (float)m_pixelH);
}

b2Vec2 UIText::GetUISize() const
{
    if (m_renderMode == RenderMode::STRETCH)
    {
        return GetRectSize();
    }
    else
    {
        return GetNativeUISize();
    }
}
