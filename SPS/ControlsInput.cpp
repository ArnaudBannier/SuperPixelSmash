/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "ControlsInput.h"
#include "GameCommon.h"

ControlsInput::ControlsInput() :
    InputGroup(), players()
{
}

void ControlsInput::OnPreEventProcess()
{
    for (PlayerInput &player : players)
    {
        player.jumpPressed = false;
        player.attackPressed = false;
        player.specialPressed = false;
        player.smashPressed = false;
    }
}

void ControlsInput::OnEventProcess(SDL_Event evt)
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
        case SDL_CONTROLLER_BUTTON_Y:
        case SDL_CONTROLLER_BUTTON_DPAD_UP:
            players[playerID].jumpPressed = true;
            players[playerID].jumpDown = true;
            break;

        case SDL_CONTROLLER_BUTTON_X:
            players[playerID].attackPressed = true;
            players[playerID].attackDown = true;
            break;

        case SDL_CONTROLLER_BUTTON_B:
            players[playerID].specialPressed = true;
            players[playerID].specialDown = true;
            break;

        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            players[playerID].goDownDown = true;
            break;

        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
            players[playerID].axisX = -1.f;
            break;

        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
            players[playerID].axisX = 1.f;
            break;

        case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
            players[playerID].defendDown = true;
            break;

        case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
            players[playerID].smashPressed = true;
            players[playerID].smashDown = true;
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
        case SDL_CONTROLLER_BUTTON_Y:
        case SDL_CONTROLLER_BUTTON_DPAD_UP:
            players[playerID].jumpDown = false;
            break;

        case SDL_CONTROLLER_BUTTON_X:
            players[playerID].attackDown = false;
            break;

        case SDL_CONTROLLER_BUTTON_B:
            players[playerID].specialDown = false;
            break;

        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            players[playerID].goDownDown = false;
            break;

        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
            players[playerID].axisX = 0.f;
            break;

        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
            players[playerID].axisX = 0.f;
            break;

        case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
            players[playerID].defendDown = false;
            break;

        case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
            players[playerID].smashDown = false;
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
            players[playerID].SetAxisLeftX(evt.caxis.value);
            break;

        case SDL_CONTROLLER_AXIS_LEFTY:
            players[playerID].SetAxisLeftY(evt.caxis.value);
            break;

        case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
            players[playerID].SetTriggerLeft(evt.caxis.value);
            break;

        case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
            players[playerID].SetTriggerRight(evt.caxis.value);
            break;

        default:
            break;
        }

    case SDL_KEYDOWN:
        scanCode = evt.key.keysym.scancode;

        if (evt.key.repeat)
            break;

        switch (scanCode)
        {
        case SDL_SCANCODE_RIGHT:
            // Deplacement à droite
            players[0].axisX = 1.f;
            break;

        case SDL_SCANCODE_LEFT:
            // Deplacement à gauche
            players[0].axisX = -1.f;
            break;

        case SDL_SCANCODE_DOWN:
            // Descente d'une plateforme
            players[0].goDownDown = true;
            break;

        case SDL_SCANCODE_X:
            // Attaque
            players[0].attackPressed = true;
            players[0].attackDown = true;
            break;

        case SDL_SCANCODE_Z:
            // Défence
            players[0].defendDown = true;
            break;

        case SDL_SCANCODE_C:
            // Défence
            players[0].specialPressed = true;
            players[0].specialDown = true;
            break;

        case SDL_SCANCODE_S:
            // Smash
            players[0].smashPressed = true;
            players[0].smashDown = true;
            break;

        case SDL_SCANCODE_SPACE:
        case SDL_SCANCODE_UP:
            // Saut
            players[0].jumpDown = true;
            players[0].jumpPressed = true;
            break;

        default:
            break;
        }
        break;

    case SDL_KEYUP:
        scanCode = evt.key.keysym.scancode;

        switch (scanCode)
        {
        case SDL_SCANCODE_RIGHT:
            // Deplacement à droite
            if (players[0].axisX > 0.f)
                players[0].axisX = 0.f;
            break;

        case SDL_SCANCODE_LEFT:
            // Deplacement à gauche
            if (players[0].axisX < 0.f)
                players[0].axisX = 0.f;
            break;

        case SDL_SCANCODE_DOWN:
            // Descente d'une plateforme
            players[0].goDownDown = false;
            break;

        case SDL_SCANCODE_SPACE:
        case SDL_SCANCODE_UP:
            // Saut
            players[0].jumpDown = false;
            break;

        case SDL_SCANCODE_Z:
            // Défence
            players[0].defendDown = false;
            break;

        case SDL_SCANCODE_S:
            // Smash
            players[0].smashDown = false;
            break;

        case SDL_SCANCODE_C:
            // Special
            players[0].specialDown = false;
            break;

        case SDL_SCANCODE_X:
            // Attaque
            players[0].attackDown = false;
            break;

        default:
            break;
        }
        break;

    default:
        break;
    }
}

void ControlsInput::Reset()
{
    for (PlayerInput &player : players)
    {
        player.axisX = 0.f;
        player.jumpDown = false;
        player.jumpPressed = false;
        player.goDownDown = false;
        player.defendDown = false;
    }
}

ControlsInput *ControlsInput::GetFromManager(InputManager *inputManager)
{
    ControlsInput *inputGroup = dynamic_cast<ControlsInput *>(
        inputManager->GetInputGroup((int)InputID::CONTROLS));
    AssertNew(inputGroup);

    return inputGroup;
}

PlayerInput::PlayerInput() :
    axisX(0.f), axisY(0.f),
    jumpPressed(false), jumpDown(false),
    attackPressed(false), attackDown(false),
    specialPressed(false), specialDown(false),
    smashPressed(false), smashDown(false),
    goDownDown(false), defendDown(false),
    m_deadZone(8000)
{
}

void PlayerInput::SetAxisLeftX(Sint16 value)
{
    if (-m_deadZone <= value && value < m_deadZone)
    {
        value = 0;
    }
    axisX = Math::Clamp((float)value / 32767.f, -1.f, 1.f);
}

void PlayerInput::SetAxisLeftY(Sint16 value)
{
    if (-m_deadZone <= value && value < m_deadZone)
    {
        value = 0;
    }
    axisY = -Math::Clamp((float)value / 32767.f, -1.f, 1.f);

    goDownDown = (axisY < -0.8f);
}

void PlayerInput::SetTriggerLeft(Sint16 value)
{
    if (value > m_deadZone)
    {
        defendDown = true;
    }
    else
    {
        defendDown = false;
    }
}

void PlayerInput::SetTriggerRight(Sint16 value)
{
    if (value > m_deadZone)
    {
        if (smashDown == false) smashPressed = true;
        smashDown = true;
    }
    else
    {
        smashDown = false;
    }
}
