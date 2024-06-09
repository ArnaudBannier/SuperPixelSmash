/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "Settings.h"
#include "UIObject.h"

class UIImage : public UIObject
{
public:
    UIImage(Scene *scene, SpriteGroup *spriteGroup, int spriteID);

    enum class RenderMode
    {
        STRETCH, ANCHOR, BORDERS
    };
    void SetRenderMode(RenderMode renderMode);
    RenderMode GetRenderMode() const;

    void SetBorders(const UIBorders &borders);
    void SetBorders(int left, int right, int top, int bottom, float scale = 1.f);

    void SetAnchor(Anchor anchor);
    void SetScale(float scale);

    void SetSpriteGroup(SpriteGroup *spriteGroup);
    void SetSpriteID(int spriteID);


    void GetNativePixelSize(int &pixelWidth, int &pixelHeight) const;
    float GetNativeAspectRatio() const;
    b2Vec2 GetNativeUISize() const;
    b2Vec2 GetUISize() const;

    void GetNaturalSize(int &pixelWidth, int &pixelHeight) const;
    float GetNaturalRatio() const;

    virtual void Render() override;
    virtual void Update() override;

private:
    SpriteGroup *m_spriteGroup;
    int m_spriteID;

    RenderMode m_renderMode;
    Anchor m_anchor;
    UIBorders m_borders;
    float m_scale;
};

inline void UIImage::SetBorders(const UIBorders &borders)
{
    m_borders = borders;
    m_scale = 1.f;
    m_renderMode = RenderMode::BORDERS;
}

inline void UIImage::SetBorders(int left, int right, int top, int bottom, float scale)
{
    SetBorders(UIBorders(left, right, top, bottom, scale));
}

inline void UIImage::GetNaturalSize(int &pixelWidth, int &pixelHeight) const
{
    const SDL_Rect *src = m_spriteGroup->GetSourceRect(m_spriteID);
    pixelWidth = src->w;
    pixelHeight = src->h;
}

inline float UIImage::GetNaturalRatio() const
{
    const SDL_Rect *src = m_spriteGroup->GetSourceRect(m_spriteID);
    return (float)src->w / (float)src->h;
}

inline void UIImage::SetAnchor(Anchor anchor)
{
    m_renderMode = RenderMode::ANCHOR;
    m_anchor = anchor;
}

inline void UIImage::SetScale(float scale)
{
    m_renderMode = RenderMode::ANCHOR;
    m_scale = scale;
}

inline void UIImage::SetSpriteGroup(SpriteGroup *spriteGroup)
{
    m_spriteGroup = spriteGroup;
}

inline void UIImage::SetSpriteID(int spriteID)
{
    m_spriteID = spriteID;
}

inline void UIImage::SetRenderMode(RenderMode renderMode)
{
    m_renderMode = renderMode;
    if (renderMode != RenderMode::ANCHOR)
    {
        m_scale = 1.f;
    }
}

inline UIImage::RenderMode UIImage::GetRenderMode() const
{
    return m_renderMode;
}
