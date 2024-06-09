/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "Settings.h"
#include "EasingFct.h"

class Animation;

enum class AnimFlag : uint32_t
{
    /// @brief Met l'animation en pause.
    PAUSED = 1 << 0,
    /// @brief Joue l'animation dans le sens inverse.
    REVERSED = 1 << 1,
    /// @brief Joue l'animation en avant puis en arrière.
    ALTERNATE = 1 << 2,
    /// @brief Arrête automatiquement l'animation à la fin.
    STOP_AT_END = 1 << 3,
    STOPPED = 1 << 4
};

class AnimationListener
{
public:
    virtual ~AnimationListener() {}

    virtual void OnAnimationEnd(Animation *which, const std::string &name) {}
    virtual void OnAnimationStart(Animation *which, const std::string &name) {}
    virtual void OnCycleEnd(Animation *which, const std::string &name) {}
    virtual void OnFrameChanged(Animation *which, const std::string &name, int frameID) {}
};

class Animation
{
public:
    Animation(const std::string &name);
    Animation(Animation const&) = delete;
    Animation& operator=(Animation const&) = delete;
    virtual ~Animation();

    virtual void SetCycleTime(float cycleTime);
    void SetCycleCount(int cycleCount);
    void SetDelay(float delay);
    void SetSpeed(float speed);
    void SetPhase(float phase);
    void SetEasing(EasingFct easing);

    void AddListener(AnimationListener *listener);
    void RemoveListener(AnimationListener *listener);

    const std::string &GetName() const;
    float GetRawProgression() const;
    float GetProgression() const;
    float GetCycleTime() const;
    float GetTotalTime() const;

    void AddFlags(AnimFlag flags);
    void SubFlags(AnimFlag flags);
    bool IsStopped() const;
    bool IsDelayed() const;
    bool IsPlaying() const;

    virtual void Play();
    virtual void Resume();
    virtual void Stop();
    virtual void Update(float dt);

protected:

    std::string m_name;

    /// @brief Courbe d'accélération de l'animation.
    EasingFct m_easing;

    /// @brief Phase de l'animation.
    float m_phase;

    /// @brief Le coefficient multiplicateur de la vitesse de lecture.
    float m_speed;

    /// @brief La durée d'un cycle.
    float m_cycleTime;

    /// @brief Délai avant le début de l'animation.
    float m_delay;

    /// @brief Accumulateur.
    float m_accu;

    /// @brief Accumulateur pour le délai.
    float m_delayAccu;

    /// @brief Les flags de l'animation.
    /// Les valeurs possibles des flags sont données dans l'énumération AnimFlags.
    AnimFlag m_flags;

    /// @brief Nombre de cycles de l'animation.
    /// L'animation tourne en boucle si ce membre vaut -1.
    int m_cycleCount;

    /// @brief Indice du cycle courant. 
    int m_cycleIdx;

    std::set<AnimationListener *> m_listeners;
};

constexpr enum AnimFlag operator |(const enum AnimFlag selfValue, const enum AnimFlag inValue)
{
    return (enum AnimFlag)(uint32_t(selfValue) | uint32_t(inValue));
}

constexpr enum AnimFlag operator &(const enum AnimFlag selfValue, const enum AnimFlag inValue)
{
    return (enum AnimFlag)(uint32_t(selfValue) & uint32_t(inValue));
}

constexpr enum AnimFlag operator ~(const enum AnimFlag selfValue)
{
    return (enum AnimFlag)(~uint32_t(selfValue));
}

inline void Animation::SetCycleTime(float cycleTime)
{
    m_cycleTime = cycleTime;
}

inline void Animation::SetDelay(float delay)
{
    m_delay = delay;
}
inline void Animation::SetSpeed(float speed)
{
    m_speed = speed;
}

inline void Animation::SetPhase(float phase)
{
    m_phase = phase;
}

inline void Animation::SetEasing(EasingFct easing)
{
    m_easing = easing;
}

inline void Animation::AddListener(AnimationListener *listener)
{
    m_listeners.insert(listener);
}

inline void Animation::RemoveListener(AnimationListener *listener)
{
    m_listeners.erase(listener);
}

inline void Animation::AddFlags(AnimFlag flags)
{
    m_flags = m_flags | flags;
}

inline void Animation::SubFlags(AnimFlag flags)
{
    m_flags = m_flags & ~flags;
}
inline float Animation::GetRawProgression() const
{
    return m_accu / m_cycleTime;
}

inline bool Animation::IsStopped() const
{
    return uint32_t(m_flags & AnimFlag::STOPPED) != 0;
}

inline bool Animation::IsDelayed() const
{
    return m_delayAccu > 0.f;
}

inline bool Animation::IsPlaying() const
{
    return !(uint32_t(m_flags & (AnimFlag::STOPPED | AnimFlag::PAUSED)));
}

inline const std::string &Animation::GetName() const
{
    return m_name;
}

inline float Animation::GetCycleTime() const
{
    return m_cycleTime;
}

inline float Animation::GetTotalTime() const
{
    return m_cycleCount >= 0 ? m_cycleCount * m_cycleTime : m_cycleTime;
}
