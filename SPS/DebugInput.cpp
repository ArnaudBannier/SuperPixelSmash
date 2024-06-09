/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "DebugInput.h"
#include "GameCommon.h"

DebugInput::DebugInput() :
    InputGroup(), nextStepDown(false), nextStepPressed(false), quitStepPressed(false),
    infoPressed(false), gizmosPressed(false), bodyPressed(false),
    camPressed(false), gridPressed(false)
{
}

void DebugInput::OnPreEventProcess()
{
    bodyPressed = false;
    camPressed = false;
    gizmosPressed = false;
    infoPressed = false;
    quitStepPressed = false;
    nextStepPressed = false;
    gridPressed = false;
}

void DebugInput::OnEventProcess(SDL_Event evt)
{
    SDL_Scancode scanCode;
    switch (evt.type)
    {
    case SDL_KEYDOWN:
        scanCode = evt.key.keysym.scancode;

        if (evt.key.repeat)
            break;

        switch (scanCode)
        {
        case SDL_SCANCODE_F1:
            // Mode pas à pas - Nouveau pas
            nextStepPressed = true;
            nextStepDown = true;
            break;

        case SDL_SCANCODE_F2:
            // Fin du mode pas à pas
            quitStepPressed = true;
            break;

        case SDL_SCANCODE_F3:
            // Body
            bodyPressed = true;
            break;

        case SDL_SCANCODE_F4:
            // Gizmos
            gizmosPressed = true;
            break;

        case SDL_SCANCODE_F5:
            // Affichage de la grille
            gridPressed = true;
            break;

        case SDL_SCANCODE_F6:
            // Caméra
            camPressed = true;
            break;

        case SDL_SCANCODE_F7:
            // Liste des objets
            infoPressed = true;
            break;

        default:
            break;
        }
        break;

    case SDL_KEYUP:
        scanCode = evt.key.keysym.scancode;

        switch (scanCode)
        {
        case SDL_SCANCODE_F1:
            // Mode pas à pas
            nextStepDown = false;
            break;

        default:
            break;
        }
        break;

    default:
        break;
    }
}

void DebugInput::Reset()
{
    bodyPressed = false;
    camPressed = false;
    gizmosPressed = false;
    infoPressed = false;
    quitStepPressed = false;
    nextStepDown = false;
    nextStepPressed = false;
    quitStepPressed = false;
    gridPressed = false;
}

DebugInput *DebugInput::GetFromManager(InputManager *inputManager)
{
    DebugInput *inputGroup = dynamic_cast<DebugInput *>(
        inputManager->GetInputGroup((int)InputID::DEBUG));
    AssertNew(inputGroup);

    return inputGroup;
}
