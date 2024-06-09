/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "Settings.h"
#include "UIObject.h"
#include "Animator.h"

class UIAnimator : public UIObject
{
public:
    UIAnimator(Scene *scene);
    virtual ~UIAnimator();

    Animator *GetAnimator();

    void SetAnchor(Anchor anchor);
    void SetScale(float scale);
    void Stretch();

    virtual void Render() override;
    virtual void Update() override;

private:
    Animator m_animator;
    Anchor m_anchor;
    float m_scale;
    bool m_stretch;
};

inline Animator *UIAnimator::GetAnimator()
{
    return &m_animator;
}

inline void UIAnimator::SetAnchor(Anchor anchor)
{
    m_anchor = anchor;
    m_stretch = false;
}

inline void UIAnimator::SetScale(float scale)
{
    m_scale = scale;
}

inline void UIAnimator::Stretch()
{
    m_stretch = true;
}
