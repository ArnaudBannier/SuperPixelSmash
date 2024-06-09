/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "UIObject.h"
#include "UICanvas.h"
#include "Scene.h"
#include "GameObject.h"
#include "Camera.h"
#include "Utils.h"

#define TARGET_RECT  0x1
#define TARGET_ALPHA 0x2
#define TARGET_COLOR 0x4

UIRect::UIRect() :
    anchorMin(b2Vec2(0.f, 0.f)), anchorMax(b2Vec2(1.f, 1.f)),
    offsetMin(b2Vec2(0.f, 0.f)), offsetMax(b2Vec2(0.f, 0.f))
{
}

UIRect::UIRect(b2Vec2 anchorMin, b2Vec2 anchorMax, b2Vec2 offsetMin, b2Vec2 offsetMax) :
    anchorMin(anchorMin), anchorMax(anchorMax), offsetMin(offsetMin), offsetMax(offsetMax)
{
};


UIObject::UIObject(Scene *scene) :
    GameObject(scene, DEFAULT_UI_LAYER), m_rect(),
    m_useParentAnim(true), m_useParentAlpha(true), m_alpha(1.f), m_fadeChildren(true),
    m_alphaAnimMap(), m_shiftAnimMap(), m_animListeners(),
    m_targets(), m_targetMask(0), m_transformAnim("Transform_Anim"), m_uiEnabled(true),
    m_fadeIAnim(nullptr), m_fadeOAnim(nullptr)
{
    SetName("UIObject");
    UIObject *canvas = scene->GetCanvas();
    if (canvas && this != canvas) SetParent(canvas);

    m_transformAnim.SetCycleCount(1);
    m_transformAnim.SetCycleTime(0.25f);
    m_transformAnim.SetEasing(EasingFct_InOut);

    AddAnimListener(this);
}

UIObject::~UIObject()
{
    for (auto it = m_alphaAnimMap.begin(); it != m_alphaAnimMap.end(); ++it)
    {
        delete it->second;
    }
    for (auto it = m_shiftAnimMap.begin(); it != m_shiftAnimMap.end(); ++it)
    {
        delete it->second;
    }
}

void UIObject::SetTargetRect(const UIRect &rect)
{
    m_targetMask |= TARGET_RECT;
    m_targets[1].rect = rect;
}

void UIObject::SetTargetOpacity(float alpha)
{
    m_targetMask |= TARGET_ALPHA;
    m_targets[1].alpha = alpha;
}

void UIObject::SetTargetColor(Color color)
{
    m_targetMask |= TARGET_COLOR;
    m_targets[1].color = color;
}

void UIObject::UpdateTransformToTarget(float dt)
{
    if (m_transformAnim.IsPlaying() == false) return;

    m_transformAnim.Update(dt);
    float t = m_transformAnim.GetProgression();

    if ((m_targetMask & TARGET_RECT) != 0)
    {
        m_rect = Math::Lerp(m_targets[0].rect, m_targets[1].rect, t);
    }
    if ((m_targetMask & TARGET_ALPHA) != 0)
    {
        m_alpha = Math::Lerp(m_targets[0].alpha, m_targets[1].alpha, t);
    }
    if ((m_targetMask & TARGET_COLOR) != 0)
    {
        m_color = Math::Lerp(m_targets[0].color, m_targets[1].color, t);
    }
}

SDL_FRect UIObject::GetCanvasRectRec(float pixelsPerUnit) const
{
    UIObject *uiParent = dynamic_cast<UIObject *>(GetParent());

    if (uiParent)
    {
        SDL_FRect pRect = uiParent->GetCanvasRectRec(pixelsPerUnit);
        float lowerX = pRect.x + pRect.w * m_rect.anchorMin.x + pixelsPerUnit * m_rect.offsetMin.x;
        float upperX = pRect.x + pRect.w * m_rect.anchorMax.x + pixelsPerUnit * m_rect.offsetMax.x;
        float lowerY = pRect.y + pRect.h * m_rect.anchorMin.y + pixelsPerUnit * m_rect.offsetMin.y;
        float upperY = pRect.y + pRect.h * m_rect.anchorMax.y + pixelsPerUnit * m_rect.offsetMax.y;

        return SDL_FRect{
            lowerX,
            lowerY,
            fabsf(upperX - lowerX),
            fabsf(upperY - lowerY)
        };
    }
    else
    {
        return SDL_FRect{
            pixelsPerUnit * m_rect.offsetMin.x,
            pixelsPerUnit * m_rect.offsetMin.y,
            pixelsPerUnit * fabsf(m_rect.offsetMax.x - m_rect.offsetMin.x),
            pixelsPerUnit * fabsf(m_rect.offsetMax.y - m_rect.offsetMin.y)
        };
    }
}

b2Vec2 UIObject::GetRectSize() const
{
    SDL_FRect rect = GetCanvasRectRec(1.0f);
    return b2Vec2(rect.w, rect.h);
}

void UIObject::OnAnimationEnd(Animation *which, const std::string &name)
{
    if (which == m_fadeIAnim)
    {
        for (UIAnimListener *listener : m_animListeners)
        {
            listener->OnFadeInEnd(this);
        }
    }
    else if (which == m_fadeOAnim)
    {
        SetUIEnabled(false);
        for (UIAnimListener *listener : m_animListeners)
        {
            listener->OnFadeOutEnd(this);
        }
    }
    else if (which == &m_transformAnim)
    {
        TransformToTarget();
        for (UIAnimListener *listener : m_animListeners)
        {
            listener->OnTransformEnd(this);
        }
    }
}

float UIObject::GetInheritedBaseAlpha() const
{
    float alpha = 1.f;

    const UIObject *uiObject = this;
    while (uiObject != nullptr)
    {
        alpha *= uiObject->m_alpha;
        if (uiObject->m_useParentAlpha == false) break;

        uiObject = dynamic_cast<UIObject *>(uiObject->GetParent());
    }

    return alpha;
}

float UIObject::GetInheritedAnimAlpha() const
{
    float alpha = 1.f;

    const UIObject *uiObject = this;
    while (uiObject != nullptr)
    {
        auto &alphaMap = uiObject->m_alphaAnimMap;
        for (auto it = alphaMap.begin(); it != alphaMap.end(); ++it)
        {
            LerpAnim<float> *alphaAnim = it->second;
            if (alphaAnim->IsStopped()) continue;

            alpha *= alphaAnim->GetValue();
        }

        if (uiObject->m_useParentAnim == false) break;

        uiObject = dynamic_cast<UIObject *>(uiObject->GetParent());
    }

    return alpha;
}

SDL_FRect UIObject::GetCanvasRect() const
{
    int rasterW, rasterH;
    SDL_RenderGetLogicalSize(g_renderer, &rasterW, &rasterH);

    float pixelsPerUnit = m_scene->GetCanvas()->GetPixelsPerUnit();
    SDL_FRect rect = GetCanvasRectRec(pixelsPerUnit);
    rect.y = rasterH - rect.y - rect.h;
    return rect;
}

bool UIObject::Contains(const SDL_FPoint &point) const
{
    SDL_FRect rect = GetCanvasRect();
    return (point.x >= rect.x) && (point.x <= rect.x + rect.w)
        && (point.y >= rect.y) && (point.y <= rect.y + rect.h);
}

void UIObject::Update()
{
    GameObject::Update();

    if (IsUIEnabled() == false) return;

    float dt = m_scene->GetTime().GetUnscaledDelta();

    for (auto it = m_alphaAnimMap.begin(); it != m_alphaAnimMap.end(); ++it)
    {
        it->second->Update(dt);
    }
    for (auto it = m_shiftAnimMap.begin(); it != m_shiftAnimMap.end(); ++it)
    {
        it->second->Update(dt);
    }

    UpdateTransformToTarget(dt);
}

LerpAnim<float> *UIObject::CreateAlphaAnimation(
    const std::string &name, float alpha0, float alpha1)
{
    LerpAnim<float> *anim = nullptr;
    auto itAlpha = m_alphaAnimMap.find(name);
    if (itAlpha != m_alphaAnimMap.end())
    {
        // L'animation existe déjà
        anim = itAlpha->second;
        anim->SetValues(alpha0, alpha1);
        return anim;
    }

    // Création de l'animation
    anim = new LerpAnim<float>(name, alpha0, alpha1);
    m_alphaAnimMap.insert(std::make_pair(name, anim));
    for (UIAnimListener *listener : m_animListeners)
    {
        anim->AddListener(listener);
    }
    return anim;
}

LerpAnim<b2Vec2> *UIObject::CreateShiftAnimation(
    const std::string &name, b2Vec2 shift0, b2Vec2 shift1)
{
    LerpAnim<b2Vec2> *anim = nullptr;
    auto itShift = m_shiftAnimMap.find(name);
    if (itShift != m_shiftAnimMap.end())
    {
        // L'animation existe déjà
        anim = itShift->second;
        anim->SetValues(shift0, shift1);
        return anim;
    }

    // Création de l'animation
    anim = new LerpAnim<b2Vec2>(name, shift0, shift1);
    m_shiftAnimMap.insert(std::make_pair(name, anim));
    for (UIAnimListener *listener : m_animListeners)
    {
        anim->AddListener(listener);
    }
    return anim;
}

void UIObject::SetFadeInAnimation(UIFadeDef fadeDef)
{
    LerpAnim<float> *alphaAnim = CreateAlphaAnimation(
        "FadeIn_Alpha", fadeDef.fadeOpacity ? 0.f : 1.f, 1.f
    );
    alphaAnim->SetEasing(fadeDef.easing);
    alphaAnim->SetDelay(fadeDef.delay);
    alphaAnim->SetCycleTime(fadeDef.duration);
    alphaAnim->SetCycleCount(1);
    alphaAnim->SubFlags(AnimFlag::STOP_AT_END);

    LerpAnim<b2Vec2> *shiftAnim = CreateShiftAnimation(
        "FadeIn_Shift", fadeDef.shift, b2Vec2_zero
    );
    shiftAnim->SetEasing(fadeDef.easing);
    shiftAnim->SetDelay(fadeDef.delay);
    shiftAnim->SetCycleTime(fadeDef.duration);
    shiftAnim->SetCycleCount(1);
    shiftAnim->SubFlags(AnimFlag::STOP_AT_END);

    m_fadeIAnim = shiftAnim;
}

void UIObject::SetFadeOutAnimation(UIFadeDef fadeDef)
{
    LerpAnim<float> *alphaAnim = CreateAlphaAnimation(
        "FadeOut_Alpha", 1.f, fadeDef.fadeOpacity ? 0.f : 1.f
    );
    alphaAnim->SetEasing(fadeDef.easing);
    alphaAnim->SetDelay(fadeDef.delay);
    alphaAnim->SetCycleTime(fadeDef.duration);
    alphaAnim->SetCycleCount(1);
    alphaAnim->SubFlags(AnimFlag::STOP_AT_END);

    LerpAnim<b2Vec2> *shiftAnim = CreateShiftAnimation(
        "FadeOut_Shift", b2Vec2_zero, fadeDef.shift
    );
    shiftAnim->SetEasing(fadeDef.easing);
    shiftAnim->SetDelay(fadeDef.delay);
    shiftAnim->SetCycleTime(fadeDef.duration);
    shiftAnim->SetCycleCount(1);
    shiftAnim->SubFlags(AnimFlag::STOP_AT_END);

    m_fadeOAnim = shiftAnim;
}

void UIObject::PlayFadeIn()
{
    if (IsFadingIn()) return;

    OnPlayFadeIn();

    if (m_fadeChildren)
    {
        for (GameObject *gameObject : GetChildren())
        {
            UIObject *child = dynamic_cast<UIObject *>(gameObject);
            if (child == nullptr) continue;

            child->PlayFadeIn();
        }
    }
}

void UIObject::PlayFadeOut()
{
    if (IsFadingOut()) return;

    OnPlayFadeOut();

    if (m_fadeChildren)
    {
        for (GameObject *gameObject : GetChildren())
        {
            UIObject *child = dynamic_cast<UIObject *>(gameObject);
            if (child == nullptr) continue;

            child->PlayFadeOut();
        }
    }
}

bool UIObject::IsFadingIn() const
{
    if (m_fadeIAnim) return m_fadeIAnim->IsPlaying();
    return false;
}

bool UIObject::IsFadingOut() const
{
    if (m_fadeOAnim) return m_fadeOAnim->IsPlaying();
    return false;
}

float UIObject::GetFadeInProgression() const
{
    if (m_fadeIAnim) return m_fadeIAnim->GetRawProgression();
    return -1.f;
}

float UIObject::GetFadeOutProgression() const
{
    if (m_fadeOAnim) return m_fadeOAnim->GetRawProgression();
    return -1.f;
}

void UIObject::AddAnimListener(UIAnimListener *listener)
{
    m_animListeners.insert(listener);
    for (auto it = m_alphaAnimMap.begin(); it != m_alphaAnimMap.end(); ++it)
    {
        it->second->AddListener(listener);
    }
    for (auto it = m_shiftAnimMap.begin(); it != m_shiftAnimMap.end(); ++it)
    {
        it->second->AddListener(listener);
    }
}

void UIObject::RemoveAnimListener(UIAnimListener *listener)
{
    m_animListeners.erase(listener);
    for (auto it = m_alphaAnimMap.begin(); it != m_alphaAnimMap.end(); ++it)
    {
        it->second->RemoveListener(listener);
    }
    for (auto it = m_shiftAnimMap.begin(); it != m_shiftAnimMap.end(); ++it)
    {
        it->second->RemoveListener(listener);
    }
}

SDL_FRect UIObject::GetRenderRect() const
{
    float pixelsPerUnit = m_scene->GetCanvas()->GetPixelsPerUnit();
    SDL_FRect rect = GetCanvasRect();
    b2Vec2 shift = GetShift();
    rect.x += pixelsPerUnit * shift.x;
    rect.y -= pixelsPerUnit * shift.y;

    // Pixel perfect LAYER_UI
    rect.x = roundf(rect.x);
    rect.y = roundf(rect.y);
    rect.w = roundf(rect.w);
    rect.h = roundf(rect.h);
    return rect;
}

float UIObject::GetAlpha() const
{
    return GetInheritedBaseAlpha() * GetInheritedAnimAlpha();
}

b2Vec2 UIObject::GetShift() const
{
    b2Vec2 shift = b2Vec2_zero;
    for (auto it = m_shiftAnimMap.begin(); it != m_shiftAnimMap.end(); ++it)
    {
        LerpAnim<b2Vec2> *shiftAnim = it->second;
        if (shiftAnim->IsStopped()) continue;

        shift += shiftAnim->GetValue();
    }

    if (m_useParentAnim)
    {
        UIObject *parent = dynamic_cast<UIObject *>(GetParent());
        if (parent != nullptr)
        {
            return shift + parent->GetShift();
        }
    }
    return shift;
}

void UIObject::PlayFadeInAlone()
{
    SetUIEnabled(true);

    auto itAlpha = m_alphaAnimMap.find("FadeOut_Alpha");
    if (itAlpha != m_alphaAnimMap.end()) itAlpha->second->Stop();

    auto itShift = m_shiftAnimMap.find("FadeOut_Shift");
    if (itShift != m_shiftAnimMap.end()) itShift->second->Stop();

    bool hasAnimation = false;
    itAlpha = m_alphaAnimMap.find("FadeIn_Alpha");
    if (itAlpha != m_alphaAnimMap.end())
    {
        itAlpha->second->Play();
        hasAnimation = true;
    }

    itShift = m_shiftAnimMap.find("FadeIn_Shift");
    if (itShift != m_shiftAnimMap.end())
    {
        itShift->second->Play();
        hasAnimation = true;
    }

    if (hasAnimation == false)
    {
        OnFadeInEnd(this);
    }
}

void UIObject::PlayFadeOutAlone()
{
    auto itAlpha = m_alphaAnimMap.find("FadeIn_Alpha");
    if (itAlpha != m_alphaAnimMap.end()) itAlpha->second->Stop();

    auto itShift = m_shiftAnimMap.find("FadeIn_Shift");
    if (itShift != m_shiftAnimMap.end()) itShift->second->Stop();

    bool hasAnimation = false;
    itAlpha = m_alphaAnimMap.find("FadeOut_Alpha");
    if (itAlpha != m_alphaAnimMap.end())
    {
        itAlpha->second->Play();
        hasAnimation = true;
    }

    itShift = m_shiftAnimMap.find("FadeOut_Shift");
    if (itShift != m_shiftAnimMap.end())
    {
        itShift->second->Play();
        hasAnimation = true;
    }

    if (hasAnimation == false)
    {
        OnFadeOutEnd(this);
        SetUIEnabled(false);
    }
}

void UIObject::OnPlayFadeIn()
{
    PlayFadeInAlone();
}

void UIObject::OnPlayFadeOut()
{
    PlayFadeOutAlone();
}

void UIObject::OnPlayTransformToTarget()
{
}

int UIObject::PlayAlphaAnim(const std::string &name)
{
    try
    {
        LerpAnim<float> *anim = m_alphaAnimMap.at(name);
        anim->Play();
    }
    catch (const std::out_of_range &e)
    {
        std::cerr << "ERROR - No animation \"" << name << "\"" << std::endl;
        std::cerr << "      - Out of Range error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int UIObject::PlayShiftAnim(const std::string &name)
{
    try
    {
        LerpAnim<b2Vec2> *anim = m_shiftAnimMap.at(name);
        anim->Play();
    }
    catch (const std::out_of_range &e)
    {
        std::cerr << "ERROR - No animation \"" << name << "\"" << std::endl;
        std::cerr << "      - Out of Range error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void UIObject::ResetTarget()
{
    m_targetMask = 0;
    m_transformAnim.Stop();
}

void UIObject::PlayTransformToTarget()
{
    m_targets[0].rect = m_rect;
    m_targets[0].alpha = m_alpha;
    m_targets[0].color = m_color;
    m_transformAnim.Play();
}

void UIObject::TransformToTarget()
{
    if ((m_targetMask & TARGET_RECT) != 0)
    {
        m_rect = m_targets[1].rect;
    }
    if ((m_targetMask & TARGET_ALPHA) != 0)
    {
        m_alpha = m_targets[1].alpha;
    }
    if ((m_targetMask & TARGET_COLOR) != 0)
    {
        m_color = m_targets[1].color;
    }
    m_targetMask = 0;
}

void UIObject::SetUIEnabled(bool isUIEnabled)
{
    m_uiEnabled = isUIEnabled;
}

bool UIObject::IsUIEnabled() const
{
    if (m_uiEnabled == false) return false;

    UIObject *parent = dynamic_cast<UIObject *>(GetParent());
    if (parent == nullptr) return m_uiEnabled;

    return parent->IsUIEnabled();
}

LerpAnim<float> *UIObject::GetAlphaAnim(const std::string &name)
{
    auto it = m_alphaAnimMap.find(name);
    if (it != m_alphaAnimMap.end()) return it->second;
    return nullptr;
}

LerpAnim<b2Vec2> *UIObject::GetShiftAnim(const std::string &name)
{
    auto it = m_shiftAnimMap.find(name);
    if (it != m_shiftAnimMap.end()) return it->second;
    return nullptr;
}

UIFadeAnim UIObject::GetFadeInAnim()
{
    UIFadeAnim fadeAnim = { 0 };
    fadeAnim.alphaAnim = GetAlphaAnim("FadeIn_Alpha");
    fadeAnim.shiftAnim = GetShiftAnim("FadeIn_Shift");
    return fadeAnim;
}

UIFadeAnim UIObject::GetFadeOutAnim()
{
    UIFadeAnim fadeAnim = { 0 };
    fadeAnim.alphaAnim = GetAlphaAnim("FadeOut_Alpha");
    fadeAnim.shiftAnim = GetShiftAnim("FadeOut_Shift");
    return fadeAnim;
}

void UIFadeDef::Reset(float durationIn)
{
    shift = b2Vec2_zero;
    delay = 0.f;
    duration = durationIn;
    easing = EasingFct_InOut;
    fadeOpacity = true;
}
