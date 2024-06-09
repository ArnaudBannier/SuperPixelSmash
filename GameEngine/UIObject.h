/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "Settings.h"
#include "GameObject.h"
#include "Animation.h"
#include "LerpAnim.h"

class UIObject;

// Unités UIRect offset : 640 x 360

class UIRect
{
public:
    UIRect();
    UIRect(b2Vec2 anchorMin, b2Vec2 anchorMax, b2Vec2 offsetMin, b2Vec2 offsetMax);

    b2Vec2 anchorMin; // Position relative
    b2Vec2 anchorMax; // Position relative
    b2Vec2 offsetMin; // Position absolue
    b2Vec2 offsetMax; // Position absolue
};

class UIBorders
{
public:
    int left, right, top, bottom;
    float scale;

    UIBorders(int left, int right, int top, int bottom) :
        left(left), right(right), top(top), bottom(bottom), scale(1.f)
    {}

    UIBorders(int left, int right, int top, int bottom, float scale) :
        left(left), right(right), top(top), bottom(bottom), scale(scale)
    {}
};

class UIFadeDef
{
public:
    UIFadeDef() :
        shift(b2Vec2_zero), delay(0.f), duration(0.25f),
        easing(EasingFct_InOut), fadeOpacity(true)
    {}

    b2Vec2 shift;
    float delay;
    float duration;
    EasingFct easing;
    bool fadeOpacity;

    void Reset(float duration);
};

class UIAnimListener : public AnimationListener
{
public:
    UIAnimListener() {};
    virtual ~UIAnimListener() {};

    virtual void OnFadeInEnd(UIObject *which) {};
    virtual void OnFadeOutEnd(UIObject *which) {};
    virtual void OnTransformEnd(UIObject *which) {};
};

struct UIFadeAnim
{
    LerpAnim<float> *alphaAnim;
    LerpAnim<b2Vec2> *shiftAnim;
};

class UIObject : public GameObject, public UIAnimListener
{
public:
    UIObject(Scene *scene);
    virtual ~UIObject();

    void SetLocalRect(const UIRect &rect);
    UIRect &GetLocalRect();
    SDL_FRect GetCanvasRect() const;
    bool Contains(const SDL_FPoint &point) const;

    virtual void Update() override;

    void SetFadeInAnimation(UIFadeDef fadeDef);
    void SetFadeOutAnimation(UIFadeDef fadeDef);
    UIFadeAnim GetFadeInAnim();
    UIFadeAnim GetFadeOutAnim();
    void PlayFadeIn();
    void PlayFadeOut();

    bool IsFadingIn() const;
    bool IsFadingOut() const;
    float GetFadeInProgression() const;
    float GetFadeOutProgression() const;

    void SetFadeChildren(bool fadeChildren);
    bool GetFadeChildren() const;

    void SetColor(Color color);
    Color GetColor() const;

    void ResetTarget();
    void SetTargetOpacity(float alpha);
    void SetTargetRect(const UIRect &rect);
    void SetTargetColor(Color color);
    void SetTransformEasing(EasingFct easing);
    void SetTransformDuration(float duration);
    void PlayTransformToTarget();
    void TransformToTarget();

    void SetUIEnabled(bool isUIEnabled);
    bool IsUIEnabled() const;
    
    void AddAnimListener(UIAnimListener *listener);
    void RemoveAnimListener(UIAnimListener *listener);
    
    void SetUseParentAnimation(bool useParentAnim);
    void SetUseParentAlpha(bool useParentAlpha);
    bool UsingParentAnimation() const;
    bool UsingParentAlpha() const;

    LerpAnim<float> *CreateAlphaAnimation(const std::string &name, float alpha0, float alpha1);
    LerpAnim<b2Vec2> *CreateShiftAnimation(const std::string &name, b2Vec2 shift0, b2Vec2 shift1);

    int PlayAlphaAnim(const std::string &name);
    int PlayShiftAnim(const std::string &name);

    LerpAnim<float> *GetAlphaAnim(const std::string &name);
    LerpAnim<b2Vec2> *GetShiftAnim(const std::string &name);

    void SetOpacity(float alpha);

    SDL_FRect GetRenderRect() const;
    float GetAlpha() const;
    b2Vec2 GetShift() const;

    b2Vec2 GetRectSize() const;

    virtual void OnAnimationEnd(Animation *which, const std::string &name) override;

protected:
    virtual void PlayFadeInAlone();
    virtual void PlayFadeOutAlone();
    virtual void OnPlayFadeIn();
    virtual void OnPlayFadeOut();
    virtual void OnPlayTransformToTarget();

    UIRect m_rect;

private:
    void UpdateTransformToTarget(float dt);
    SDL_FRect GetCanvasRectRec(float pixelsPerUnit) const;
    float GetInheritedBaseAlpha() const;
    float GetInheritedAnimAlpha() const;

    std::map<std::string, LerpAnim<float>*> m_alphaAnimMap;
    std::map<std::string, LerpAnim<b2Vec2>*> m_shiftAnimMap;
    std::set<UIAnimListener *> m_animListeners;

    bool m_useParentAnim;
    bool m_useParentAlpha;
    bool m_fadeChildren;
    bool m_uiEnabled;
    float m_alpha;
    Color m_color;
    
    struct UITarget
    {
        UITarget() :
            alpha(1.f), color(), rect()
        {}

        float alpha;
        Color color;
        UIRect rect;
    };
    int m_targetMask;
    std::array<UITarget, 2> m_targets;
    LerpAnim<b2Vec2> *m_fadeIAnim;
    LerpAnim<b2Vec2> *m_fadeOAnim;
    Animation m_transformAnim;
};

inline void UIObject::SetLocalRect(const UIRect &rect)
{
    m_rect = rect;
}

inline UIRect &UIObject::GetLocalRect()
{
    return m_rect;
}

inline void UIObject::SetTransformEasing(EasingFct easing)
{
    m_transformAnim.SetEasing(easing);
}

inline void UIObject::SetTransformDuration(float duration)
{
    m_transformAnim.SetCycleTime(duration);
}

inline void UIObject::SetUseParentAnimation(bool useParentAnim)
{
    m_useParentAnim = useParentAnim;
}

inline void UIObject::SetUseParentAlpha(bool useParentAlpha)
{
    m_useParentAlpha = useParentAlpha;
}

inline bool UIObject::UsingParentAnimation() const
{
    return m_useParentAnim;
}

inline bool UIObject::UsingParentAlpha() const
{
    return m_useParentAlpha;
}

inline void UIObject::SetOpacity(float alpha)
{
    m_alpha = alpha;
}

inline void UIObject::SetColor(Color color)
{
    m_color = color;
}

inline Color UIObject::GetColor() const
{
    return m_color;
}

inline void UIObject::SetFadeChildren(bool fadeChildren)
{
    m_fadeChildren = fadeChildren;
}

inline bool UIObject::GetFadeChildren() const
{
    return m_fadeChildren;
}

inline UIRect operator + (const UIRect& a, const UIRect& b)
{
    return UIRect(
        a.anchorMin + b.anchorMin,
        a.anchorMax + b.anchorMax,
        a.offsetMin + b.offsetMin,
        a.offsetMax + b.offsetMax
    );
}

inline UIRect operator - (const UIRect& a, const UIRect& b)
{
    return UIRect(
        a.anchorMin - b.anchorMin,
        a.anchorMax - b.anchorMax,
        a.offsetMin - b.offsetMin,
        a.offsetMax - b.offsetMax
    );
}

inline UIRect operator * (float s, const UIRect& a)
{
    return UIRect(
        s * a.anchorMin,
        s * a.anchorMax,
        s * a.offsetMin,
        s * a.offsetMax
    );
}
