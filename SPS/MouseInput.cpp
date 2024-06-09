/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "MouseInput.h"
#include "GameCommon.h"

MouseInput::MouseInput() :
    InputGroup(),
    leftClickCount(0), leftDown(false), leftReleased(false),
    rightClickCount(0), rightDown(false), rightReleased(false),
    middleClick(false), wheel(0)
{
    viewPos.SetZero();
    lastViewPos.SetZero();
}

void MouseInput::OnPreEventProcess()
{
    leftClickCount = 0;
    leftReleased = false;
    rightClickCount = 0;
    rightReleased = false;
    middleClick = false;
    wheel = 0;
    lastViewPos = viewPos;
}

void MouseInput::OnEventProcess(SDL_Event evt)
{
    Uint8 mouseButton;
    switch (evt.type)
    {
    case SDL_MOUSEMOTION:
        viewPos.Set((float)evt.motion.x, (float)evt.motion.y);
        break;

    case SDL_MOUSEBUTTONDOWN:
        mouseButton = evt.button.button;

        switch (mouseButton)
        {
        case SDL_BUTTON_MIDDLE:
            middleClick = true;
            break;
        case SDL_BUTTON_LEFT:
            leftClickCount = evt.button.clicks;
            leftDown = true;
            break;
        case SDL_BUTTON_RIGHT:
            rightClickCount = evt.button.clicks;
            rightDown = true;
            break;
        default:
            break;
        }
        break;

    case SDL_MOUSEBUTTONUP:
        mouseButton = evt.button.button;

        switch (mouseButton)
        {
        case SDL_BUTTON_LEFT:
            leftDown = false;
            leftReleased = true;
            break;
        case SDL_BUTTON_RIGHT:
            rightDown = false;
            rightReleased = true;
            break;
        default:
            break;
        }
        break;

    case SDL_MOUSEWHEEL:
        wheel += evt.wheel.y;
        break;

    default:
        break;
    }
}

void MouseInput::Reset()
{
    lastViewPos.SetZero();
    viewPos.SetZero();

    leftClickCount = 0;
    leftDown = false;
    leftReleased = false;

    rightClickCount = 0;
    rightDown = false;
    rightReleased = false;

    middleClick = false;
    wheel = 0;
}

MouseInput *MouseInput::GetFromManager(InputManager *inputManager)
{
    MouseInput *inputGroup = dynamic_cast<MouseInput *>(
        inputManager->GetInputGroup((int)InputID::MOUSE));
    AssertNew(inputGroup);

    return inputGroup;
}
