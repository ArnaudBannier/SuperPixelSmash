/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "Settings.h"
#include "UIObject.h"
#include "Text.h"

class UIText : public UIObject
{
public:
    UIText(Scene *scene, const std::string &str, TTF_Font *font, Color color);
    virtual ~UIText();

    enum class RenderMode
    {
        STRETCH, ANCHOR
    };
    void SetRenderMode(RenderMode renderMode);
    RenderMode GetRenderMode() const;

    void SetString(const std::string &str);
    void SetAnchor(Anchor anchor);

    const std::string &GetString() const;

    void GetNativePixelSize(int &pixelWidth, int &pixelHeight) const;
    b2Vec2 GetNativeUISize() const;
    b2Vec2 GetUISize() const;

    virtual void Render() override;
    virtual void Update() override;

protected:
    RenderMode m_renderMode;
    Text m_text;
    Anchor m_anchor;
    int m_pixelW;
    int m_pixelH;
};

inline void UIText::SetAnchor(Anchor anchor)
{
    m_renderMode = RenderMode::ANCHOR;
    m_anchor = anchor;
}

inline const std::string &UIText::GetString() const
{
    return m_text.GetString();
}

inline void UIText::SetRenderMode(RenderMode renderMode)
{
    m_renderMode = renderMode;
}

inline UIText::RenderMode UIText::GetRenderMode() const
{
    return m_renderMode;
}
