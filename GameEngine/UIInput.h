/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "Settings.h"

class UIInput
{
public:
    UIInput();

    bool up;
    bool down;
    bool left;
    bool right;
    bool start;
    bool validatePressed;
    bool validateReleased;
    bool cancel;

    void SetAxisLeftX(Sint16 value);
    void SetAxisLeftY(Sint16 value);

private:
    Sint16 m_deadZone;
    Sint16 m_activeZone;
    Sint16 m_leftAxisX;
    Sint16 m_leftAxisY;
};
