/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "GameSettings.h"

class MouseInput : public InputGroup
{
public:
    MouseInput();

    virtual void OnPreEventProcess() override;
    virtual void OnEventProcess(SDL_Event evt) override;
    virtual void Reset() override;

    // Position de la souris
    b2Vec2 viewPos;
    b2Vec2 lastViewPos;

    // Clic gauche
    int  leftClickCount;
    bool leftDown;
    bool leftReleased;

    // Clic droit
    int  rightClickCount;
    bool rightDown;
    bool rightReleased;

    // Roulette
    bool middleClick;
    int  wheel;

    static MouseInput *GetFromManager(InputManager *inputManager);
};
