/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "UIInput.h"

UIInput::UIInput() :
    up(false), down(false), left(false), right(false), start(false),
    validatePressed(false), validateReleased(false), cancel(false),
    m_leftAxisX(0), m_leftAxisY(0),
    m_deadZone(8000), m_activeZone(1 << 14)
{
}

void UIInput::SetAxisLeftX(Sint16 value)
{
    if (value >= m_activeZone && m_leftAxisX < m_activeZone)
    {
        right = true;
    }
    else if (value <= -m_activeZone && m_leftAxisX > -m_activeZone)
    {
        left = true;
    }
    m_leftAxisX = value;
}

void UIInput::SetAxisLeftY(Sint16 value)
{
    if (value >= m_activeZone && m_leftAxisY < m_activeZone)
    {
        down = true;
    }
    else if (value <= -m_activeZone && m_leftAxisY > -m_activeZone)
    {
        up = true;
    }
    m_leftAxisY = value;
}
