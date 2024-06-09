/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "ApplicationInput.h"
#include "GameCommon.h"

ApplicationInput::ApplicationInput() :
    InputGroup(), pausePressed(false), quitPressed(false), uiInputs()
{
}

void ApplicationInput::OnPreEventProcess()
{
    quitPressed = false;
    pausePressed = false;

    for (UIInput &input : uiInputs)
    {
        input.up = false;
        input.down = false;
        input.left = false;
        input.right = false;
        input.validatePressed = false;
        input.validateReleased = false;
        input.cancel = false;
        input.start = false;
    }
}

void ApplicationInput::OnEventProcess(SDL_Event evt)
{
    SDL_Scancode scanCode;
    SDL_GameController *controller = nullptr;
    int playerID = 0;

    switch (evt.type)
    {
    case SDL_CONTROLLERBUTTONDOWN:
        controller = SDL_GameControllerFromInstanceID(evt.cbutton.which);
        playerID = SDL_GameControllerGetPlayerIndex(controller);
        assert(0 <= playerID && playerID < MAX_PLAYER_COUNT);

        switch (evt.cbutton.button)
        {
        case SDL_CONTROLLER_BUTTON_A:
        case SDL_CONTROLLER_BUTTON_X:
            uiInputs[playerID].validatePressed = true;
            break;

        case SDL_CONTROLLER_BUTTON_B:
        case SDL_CONTROLLER_BUTTON_Y:
            uiInputs[playerID].cancel = true;
            break;

        case SDL_CONTROLLER_BUTTON_DPAD_UP:
            uiInputs[playerID].up = true;
            break;

        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            uiInputs[playerID].down = true;
            break;

        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
            uiInputs[playerID].left = true;
            break;

        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
            uiInputs[playerID].right = true;
            break;

        case SDL_CONTROLLER_BUTTON_START:
            uiInputs[playerID].start = true;
            pausePressed = true;
            break;
        }
        break;

    case SDL_CONTROLLERBUTTONUP:
        controller = SDL_GameControllerFromInstanceID(evt.cbutton.which);
        playerID = SDL_GameControllerGetPlayerIndex(controller);
        assert(0 <= playerID && playerID < MAX_PLAYER_COUNT);

        switch (evt.cbutton.button)
        {
        case SDL_CONTROLLER_BUTTON_A:
        case SDL_CONTROLLER_BUTTON_X:
            uiInputs[playerID].validateReleased = true;
            break;
        }
        break;

    case SDL_CONTROLLERAXISMOTION:
        controller = SDL_GameControllerFromInstanceID(evt.caxis.which);
        playerID = SDL_GameControllerGetPlayerIndex(controller);
        assert(0 <= playerID && playerID < MAX_PLAYER_COUNT);
        
        switch (evt.caxis.axis)
        {
        case SDL_CONTROLLER_AXIS_LEFTX:
            uiInputs[playerID].SetAxisLeftX(evt.caxis.value);
            break;

        case SDL_CONTROLLER_AXIS_LEFTY:
            uiInputs[playerID].SetAxisLeftY(evt.caxis.value);
            break;

        default:
            break;
        }
        break;

    case SDL_QUIT:
        quitPressed = true;
        break;

    case SDL_KEYDOWN:
        scanCode = evt.key.keysym.scancode;

        if (evt.key.repeat)
            break;

        switch (scanCode)
        {
        case SDL_SCANCODE_ESCAPE:
            quitPressed = true;
            break;

        case SDL_SCANCODE_RETURN:
            pausePressed = true;
            uiInputs[0].start = true;
            break;

        case SDL_SCANCODE_BACKSPACE:
            uiInputs[0].cancel = true;
            break;

        case SDL_SCANCODE_UP:
            uiInputs[0].up = true;
            break;

        case SDL_SCANCODE_DOWN:
            uiInputs[0].down = true;
            break;

        case SDL_SCANCODE_LEFT:
            uiInputs[0].left = true;
            break;

        case SDL_SCANCODE_RIGHT:
            uiInputs[0].right = true;
            break;

        case SDL_SCANCODE_SPACE:
            uiInputs[0].validatePressed = true;
            break;

        default:
            break;
        }
        break;

    case SDL_KEYUP:
        scanCode = evt.key.keysym.scancode;

        if (evt.key.repeat)
            break;

        switch (scanCode)
        {
        case SDL_SCANCODE_SPACE:
            uiInputs[0].validateReleased = true;
            break;

        default:
            break;
        }
        break;

    default:
        break;
    }
}

void ApplicationInput::Reset()
{
    quitPressed = false;
    pausePressed = false;

    for (UIInput &input : uiInputs)
    {
        input.SetAxisLeftX(0);
        input.SetAxisLeftY(0);

        input.up = false;
        input.down = false;
        input.left = false;
        input.right = false;
        input.validatePressed = false;
        input.validateReleased = false;
        input.cancel = false;
        input.start = false;
    }
}

ApplicationInput *ApplicationInput::GetFromManager(InputManager *inputManager)
{
    ApplicationInput *inputGroup = dynamic_cast<ApplicationInput *>(
        inputManager->GetInputGroup((int)InputID::APPLICATION));
    AssertNew(inputGroup);

    return inputGroup;
}
