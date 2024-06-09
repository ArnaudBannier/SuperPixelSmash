/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "SpriteAnim.h"
#include "Animator.h"

SpriteAnim::SpriteAnim(const std::string &name, SpriteGroup *spriteGroup) :
    Animation(name), m_spriteGroup(spriteGroup),
    m_frameTimes(spriteGroup->GetSpriteCount()), m_frameID(0)
{
    assert(m_spriteGroup);
    if (m_spriteGroup->GetSpriteCount() < 2)
        AddFlags(AnimFlag::PAUSED);
}

void SpriteAnim::Update(float dt)
{
    int cycleIdx = m_cycleIdx;

    Animation::Update(dt);
    if (uint32_t(m_flags & AnimFlag::STOPPED))
    {
        return;
    }

    const int frameCount = m_spriteGroup->GetSpriteCount();
    if (cycleIdx < m_cycleIdx)
    {
        for (AnimationListener *listener : m_listeners)
        {
            for (m_frameID = m_frameID + 1; m_frameID < frameCount; m_frameID++)
            {
                listener->OnFrameChanged(this, m_name, m_frameID);
                if (uint32_t(m_flags & AnimFlag::STOPPED))
                {
                    return;
                }
            }

            m_frameID = 0;
            listener->OnFrameChanged(this, m_name, 0);
            if (uint32_t(m_flags & AnimFlag::STOPPED))
            {
                return;
            }
        }

        m_frameID = 0;
    }

    const float epsilon = 0.001f;
    const float p = GetProgression();
    const float time = p * m_cycleTime;
    while (m_frameID < frameCount - 1)
    {
        int nextFrameID = m_frameID + 1;
        if (time < m_frameTimes[nextFrameID] - epsilon) break;

        m_frameID = nextFrameID;

        for (AnimationListener *listener : m_listeners)
        {
            listener->OnFrameChanged(this, m_name, m_frameID);
            if (uint32_t(m_flags & AnimFlag::STOPPED))
            {
                return;
            }
        }
    }
}

void SpriteAnim::ComputeFrameTimes()
{
    m_frameTimes.clear();
    int frameCount = m_spriteGroup->GetSpriteCount();
    for (int i = 0; i < frameCount; i++)
    {
        m_frameTimes.push_back((float)i / (float)frameCount * m_cycleTime);
    }
}

void SpriteAnim::Play()
{
    Animation::Play();

    m_frameID = 0;

    for (AnimationListener *listener : m_listeners)
    {
        listener->OnFrameChanged(this, m_name, m_frameID);
    }
}
