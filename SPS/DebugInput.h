/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "GameSettings.h"

class DebugInput : public InputGroup
{
public:
    DebugInput();

    virtual void OnPreEventProcess() override;
    virtual void OnEventProcess(SDL_Event evt) override;
    virtual void Reset() override;

    // Mode pas-à-pas
    bool nextStepDown;
    bool nextStepPressed;
    bool quitStepPressed;

    // Informations
    bool infoPressed;
    bool gizmosPressed;
    bool bodyPressed;
    bool gridPressed;

    // Changement de caméra
    bool camPressed;

    static DebugInput *GetFromManager(InputManager *inputManager);
};
