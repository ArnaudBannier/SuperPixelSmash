/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "Animation.h"
#include "Animator.h"

Animation::Animation(const std::string &name) :
    m_accu(0.f), m_cycleCount(-1), m_cycleIdx(0), m_cycleTime(1.f),
    m_delay(0.f), m_delayAccu(0.f), m_phase(0.f), m_speed(1.f),
    m_name(name), m_flags(AnimFlag::STOP_AT_END | AnimFlag::STOPPED),
    m_easing(EasingFct_Linear), m_listeners()
{
}

Animation::~Animation()
{
}

void Animation::SetCycleCount(int cycleCount)
{
    m_cycleCount = cycleCount;
    m_cycleIdx = 0;
    if (cycleCount == 0)
    {
        AddFlags(AnimFlag::PAUSED);
    }
}
float Animation::GetProgression() const
{
    float t = GetRawProgression();
    
    // Phase
    t += m_phase;
    if (t > 1.f)
        t -= floorf(t);
    
    // Animation alternée
    if (uint32_t(m_flags & AnimFlag::ALTERNATE))
    {
        t = (t < 0.5f) ? 2.f * t : 2.f - 2.f * t;
    }
    
    // Animation inversée
    if (uint32_t(m_flags & AnimFlag::REVERSED))
    {
        t = 1.f - t;
    }
    
    return (m_easing)(t);
}

void Animation::Play()
{
    m_accu = 0.f;
    m_cycleIdx = 0;
    m_delayAccu = m_delay;
    SubFlags(AnimFlag::PAUSED | AnimFlag::STOPPED);
}

void Animation::Resume()
{
    SubFlags(AnimFlag::PAUSED | AnimFlag::STOPPED);
}

void Animation::Stop()
{
    AddFlags(AnimFlag::STOPPED);
}

void Animation::Update(float dt)
{
    if (uint32_t(m_flags & (AnimFlag::PAUSED | AnimFlag::STOPPED)))
        return;

    const float epsilon = 0.001f;
    if (m_delayAccu > epsilon)
    {
        m_delayAccu -= dt;

        if (m_delayAccu <= epsilon)
        {
            for (AnimationListener *listener : m_listeners)
            {
                listener->OnAnimationStart(this, m_name);
                if (uint32_t(m_flags & AnimFlag::STOPPED))
                    return;
            }
        }
        return;
    }

    m_accu += m_speed * dt;

    while (m_accu >= m_cycleTime - epsilon)
    {
        m_accu -= m_cycleTime;
        m_cycleIdx++;

        for (AnimationListener *listener : m_listeners)
        {
            listener->OnCycleEnd(this, m_name);
            if (uint32_t(m_flags & AnimFlag::STOPPED))
                return;
        }

        if ((m_cycleCount > 0) && (m_cycleIdx >= m_cycleCount))
        {
            // Fin de l'animation
            m_accu = m_cycleTime;
            AddFlags(AnimFlag::PAUSED);

            if (uint32_t(m_flags & AnimFlag::STOP_AT_END))
            {
                AddFlags(AnimFlag::STOPPED);
            }

            for (AnimationListener *listener : m_listeners)
            {
                listener->OnAnimationEnd(this, m_name);
            }
            return;
        }
    }
}
