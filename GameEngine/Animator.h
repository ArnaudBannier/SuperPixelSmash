/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "Settings.h"
#include "Timer.h"
#include "Renderer.h"

#include "Animation.h"
#include "SpriteAnim.h"

class Animator
{
public:
    Animator();
    Animator(Animator const&) = delete;
    Animator& operator=(Animator const&) = delete;
    virtual ~Animator();

    SpriteAnim *CreateAnimation(const std::string &name, SpriteGroup *spriteGroup);
    int DeleteAnimation(const std::string &name);
    SpriteAnim *GetAnimation(const std::string &name);

    int PlayAnimation(const std::string &name);
    int ResumeAnimation(const std::string &name);
    void StopAnimation();

    void Update(float dt);
    void AddListener(AnimationListener *listener);
    void RemoveListener(AnimationListener *listener);

    SDL_Texture *GetTexture() const;
    const SDL_Rect *GetSourceRect() const;
    int GetFrameID() const;
    SpriteAnim *GetActiveAnimation();
    bool IsAnimationDelayed() const;

protected:
    std::map<std::string, SpriteAnim *> m_spriteAnimMap;
    SpriteAnim *m_activeAnimation;
    std::set<AnimationListener *> m_listeners;
};


inline SpriteAnim *Animator::GetAnimation(const std::string &name)
{
    return m_spriteAnimMap.at(name);
}

inline int Animator::GetFrameID() const
{
    if (m_activeAnimation == nullptr) return -1;
    return m_activeAnimation->GetFrameID();
}

inline SpriteAnim *Animator::GetActiveAnimation()
{
    return m_activeAnimation;
}

inline bool Animator::IsAnimationDelayed() const
{
    if (m_activeAnimation == nullptr) return false;
    return m_activeAnimation->IsDelayed();
}
