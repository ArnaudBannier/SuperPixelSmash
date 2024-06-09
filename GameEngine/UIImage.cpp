/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "UIImage.h"
#include "Camera.h"

UIImage::UIImage(Scene *scene, SpriteGroup *spriteGroup, int index) :
    UIObject(scene), m_spriteGroup(spriteGroup),
    m_borders(0, 0, 0, 0), m_renderMode(RenderMode::STRETCH), m_spriteID(index),
    m_scale(1.f), m_anchor(Anchor::CENTER)
{
    SetName("UIImage");
}

void UIImage::Render()
{
    if (IsUIEnabled() == false) return;
    if (m_spriteGroup == nullptr) return;

    SDL_Texture *texture = m_spriteGroup->GetTexture();
    const SDL_Rect *srcRect = m_spriteGroup->GetSourceRect(m_spriteID);

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(texture, (Uint8)(255.f * GetAlpha()));

    SDL_FRect dstRect = GetRenderRect();
    switch (m_renderMode)
    {
    default:
    case UIImage::RenderMode::STRETCH:
        SDL_RenderCopyF(g_renderer, texture, srcRect, &dstRect);
        break;

    case UIImage::RenderMode::ANCHOR:
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
        dstRect.h = srcRect->h * m_scale;
        dstRect.w = srcRect->w * m_scale;
        RenderCopyF(g_renderer, texture, srcRect, &dstRect, m_anchor);
        break;
    }

    case UIImage::RenderMode::BORDERS:
        RenderCopyBordersF(
            g_renderer, texture, srcRect, &dstRect, Anchor::NORTH_WEST,
            m_borders.left, m_borders.right,
            m_borders.top, m_borders.bottom,
            m_borders.scale
        );
        break;
    }
}

void UIImage::Update()
{
    UIObject::Update();
    SetVisible(true);
}

void UIImage::GetNativePixelSize(int &pixelWidth, int &pixelHeight) const
{
    const SDL_Rect *src = m_spriteGroup->GetSourceRect(m_spriteID);
    pixelWidth = src->w;
    pixelHeight = src->h;
}

float UIImage::GetNativeAspectRatio() const
{
    const SDL_Rect *src = m_spriteGroup->GetSourceRect(m_spriteID);
    return (float)src->w / (float)src->h;
}

b2Vec2 UIImage::GetNativeUISize() const
{
    float pixelsPerUnit = m_scene->GetUIPixelsPerUnit();
    const SDL_Rect *src = m_spriteGroup->GetSourceRect(m_spriteID);
    return 1.0f / pixelsPerUnit * b2Vec2((float)src->w, (float)src->h);
}

b2Vec2 UIImage::GetUISize() const
{
    if (m_renderMode == RenderMode::ANCHOR)
    {
        return m_scale * GetNativeUISize();
    }
    else
    {
        return GetRectSize();
    }
}
