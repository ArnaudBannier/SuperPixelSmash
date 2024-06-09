/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "Animator.h"
#include "Animation.h"

Animator::Animator() :
    m_spriteAnimMap(), m_listeners(), m_activeAnimation(nullptr)
{
}

Animator::~Animator()
{
    for (auto it = m_spriteAnimMap.begin(); it != m_spriteAnimMap.end(); ++it)
    {
        delete it->second;
    }
}

SpriteAnim *Animator::CreateAnimation(const std::string &name, SpriteGroup *spriteGroup)
{
    assert(spriteGroup);
    SpriteAnim *anim = new SpriteAnim(name, spriteGroup);
    for (AnimationListener *listener : m_listeners)
    {
        anim->AddListener(listener);
    }

    m_spriteAnimMap.insert(std::make_pair(name, anim));
    return anim;
}

int Animator::DeleteAnimation(const std::string &name)
{
    auto it = m_spriteAnimMap.find(name);
    if (it != m_spriteAnimMap.end())
    {
        delete it->second;
        m_spriteAnimMap.erase(it);
        return EXIT_SUCCESS;
    }
    return EXIT_SUCCESS;
}

int Animator::PlayAnimation(const std::string &name)
{
    try
    {
        if (m_activeAnimation)
        {
            m_activeAnimation->Stop();
        }
        SpriteAnim *anim = m_spriteAnimMap.at(name);
        anim->Play();
        m_activeAnimation = anim;
    }
    catch (const std::out_of_range &e)
    {
        std::cerr << "ERROR - No animation \"" << name << "\"" << std::endl;
        std::cerr << "      - Out of Range error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int Animator::ResumeAnimation(const std::string &name)
{
    try
    {
        if (m_activeAnimation)
        {
            m_activeAnimation->Stop();
        }
        SpriteAnim *anim = m_spriteAnimMap.at(name);
        anim->Resume();
        m_activeAnimation = anim;
    }
    catch (const std::out_of_range &e)
    {
        std::cerr << "ERROR - No animation \"" << name << "\"" << std::endl;
        std::cerr << "      - Out of Range error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void Animator::StopAnimation()
{
    if (m_activeAnimation)
    {
        m_activeAnimation->Stop();
    }
    m_activeAnimation = nullptr;
}

void Animator::Update(float dt)
{
    if (m_activeAnimation)
    {
        m_activeAnimation->Update(dt);
    }
}

void Animator::AddListener(AnimationListener *listener)
{
    for (auto it = m_spriteAnimMap.begin(); it != m_spriteAnimMap.end(); ++it)
    {
        it->second->AddListener(listener);
    }
    m_listeners.insert(listener);
}

void Animator::RemoveListener(AnimationListener *listener)
{
    for (auto it = m_spriteAnimMap.begin(); it != m_spriteAnimMap.end(); ++it)
    {
        it->second->RemoveListener(listener);
    }
    m_listeners.erase(listener);
}

SDL_Texture *Animator::GetTexture() const
{
    if (m_activeAnimation == nullptr) return nullptr;
    return m_activeAnimation->GetTexture();
}

const SDL_Rect *Animator::GetSourceRect() const
{
    if (m_activeAnimation == nullptr) return nullptr;
    return m_activeAnimation->GetSourceRect();
}
