/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "Timer.h"

Timer::Timer()
{
    m_startTime = 0;
    m_currentTime = 0;
    m_previousTime = 0;

    m_delta = 0;
    m_unscaledDelta = 0;
    m_elapsed = 0;
    m_unscaledElapsed = 0;

    m_maxDelta = 100;
    m_scale = 1.0;
}

void Timer::Start()
{
    m_currentTime = SDL_GetTicks64();
    m_previousTime = SDL_GetTicks64();
    m_delta = 0;
}

void Timer::Update()
{
    m_previousTime = m_currentTime;
    m_currentTime = SDL_GetTicks64();

    m_unscaledDelta = m_currentTime - m_previousTime;
    if (m_unscaledDelta > m_maxDelta)
    {
        m_unscaledDelta = m_maxDelta;
    }
    m_delta = (Uint64)(m_scale * (double)m_unscaledDelta);

    m_unscaledElapsed += m_unscaledDelta;
    m_elapsed += m_delta;
}

void Timer::Update(Uint64 deltaTimeMS)
{
    m_unscaledDelta = deltaTimeMS;
    if (m_unscaledDelta > m_maxDelta)
    {
        m_unscaledDelta = m_maxDelta;
    }
    m_delta = (Uint64)(m_scale * (double)m_unscaledDelta);

    m_previousTime = m_currentTime;
    m_currentTime += m_delta;

    m_unscaledElapsed += m_unscaledDelta;
    m_elapsed += m_delta;
}
