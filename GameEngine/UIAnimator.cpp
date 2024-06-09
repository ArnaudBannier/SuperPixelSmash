/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "UIAnimator.h"

UIAnimator::UIAnimator(Scene *scene) :
    UIObject(scene), m_animator(),
    m_stretch(false), m_scale(1.f), m_anchor(Anchor::CENTER)
{
    SetName("UIAnimator");
}

UIAnimator::~UIAnimator()
{
}

void UIAnimator::Render()
{
    if (IsUIEnabled() == false) return;

    SDL_Texture *texture = m_animator.GetTexture();
    if (texture == nullptr) return;

    SDL_SetTextureAlphaMod(texture, (Uint8)(255.f * GetAlpha()));
    const SDL_Rect *srcRect = m_animator.GetSourceRect();
    SDL_FRect dstRect = GetRenderRect();

    if (m_stretch)
    {
        SDL_RenderCopyF(g_renderer, texture, srcRect, &dstRect);
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

        dstRect.h = (float)srcRect->h * m_scale;
        dstRect.w = (float)srcRect->w * m_scale;

        RenderCopyF(g_renderer, texture, srcRect, &dstRect, m_anchor);
    }
}

void UIAnimator::Update()
{
    UIObject::Update();
    SetVisible(true);

    float dt = m_scene->GetTime().GetUnscaledDelta();
    m_animator.Update(dt);
}
