/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "Settings.h"

class InputGroup
{
public:
    InputGroup();
    InputGroup(InputGroup const&) = delete;
    InputGroup& operator=(InputGroup const&) = delete;
    virtual ~InputGroup();

    virtual void OnPreEventProcess();
    virtual void OnEventProcess(SDL_Event evt);
    virtual void Reset();

    void SetEnabled(bool enabled);
    bool IsEnabled();

protected:
    bool m_enabled;
};

inline void InputGroup::SetEnabled(bool enabled)
{
    if (m_enabled != enabled)
    {
        m_enabled = enabled;
        Reset();
    }
}

inline bool InputGroup::IsEnabled()
{
    return m_enabled;
}
