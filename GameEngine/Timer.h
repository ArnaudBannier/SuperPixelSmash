/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "Settings.h"

/// @ingroup Timer
/// @brief Structure représentant un chronomètre.
class Timer
{
public:
    Timer();
    Timer(Timer const&) = delete;
    Timer& operator=(Timer const&) = delete;

    void Start();
    void Update();
    void Update(float deltaTime);
    void Update(Uint64 deltaTimeMS);

    void SetMaximumDeltaTime(float maxDelta);
    void SetTimeScale(float scale);
    float GetTimeScale() const;
    float GetDelta() const;
    float GetUnscaledDelta() const;
    float GetElapsed() const;
    float GetUnscaledElapsed() const;

    Uint64 GetDeltaMS() const;
    Uint64 GetUnscaledDeltaMS() const;
    Uint64 GetElapsedMS() const;
    Uint64 GetUnscaledElapsedMS() const;

protected:
    /// @protected
    /// @brief Temps de départ.
    Uint64 m_startTime;

    /// @protected
    /// @brief Temps du dernier appel à Timer_Update().
    Uint64 m_currentTime;

    /// @protected
    /// @brief Temps de l'avant dernier appel à Timer_Update().
    Uint64 m_previousTime;

    /// @protected
    /// @brief Ecart entre les deux derniers appels à Timer_Update().
    Uint64 m_delta;
    Uint64 m_unscaledDelta;

    float m_scale;

    Uint64 m_maxDelta;

    Uint64 m_elapsed;
    Uint64 m_unscaledElapsed;
};

inline void Timer::SetMaximumDeltaTime(float maxDelta)
{
    m_maxDelta = (Uint64)(maxDelta * 1000.f);
}

inline void Timer::SetTimeScale(float scale)
{
    m_scale = scale;
}

inline float Timer::GetDelta() const
{
    return (float)m_delta / 1000.f;
}

inline float Timer::GetTimeScale() const
{
    return m_scale;
};

inline float Timer::GetUnscaledDelta() const
{
    return (float)m_unscaledDelta / 1000.f;
}

inline float Timer::GetElapsed() const
{
    return (float)m_elapsed / 1000.f;
}

inline float Timer::GetUnscaledElapsed() const
{
    return (float)m_unscaledElapsed / 1000.f;
}

inline Uint64 Timer::GetDeltaMS() const
{
    return m_delta;
}

inline Uint64 Timer::GetUnscaledDeltaMS() const
{
    return m_unscaledElapsed;
}

inline Uint64 Timer::GetElapsedMS() const
{
    return m_elapsed;
}

inline Uint64 Timer::GetUnscaledElapsedMS() const
{
    return m_unscaledElapsed;
}

inline void Timer::Update(float deltaTime)
{
    Update((Uint64)(deltaTime * 1000.f));
}
