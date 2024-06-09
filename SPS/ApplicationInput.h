/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "GameSettings.h"

class ApplicationInput : public InputGroup
{
public:
    ApplicationInput();

    virtual void OnPreEventProcess() override;
    virtual void OnEventProcess(SDL_Event evt) override;
    virtual void Reset() override;

    bool quitPressed;
    bool pausePressed;


    std::array<UIInput, MAX_PLAYER_COUNT> uiInputs;

    static ApplicationInput *GetFromManager(InputManager *inputManager);
};
