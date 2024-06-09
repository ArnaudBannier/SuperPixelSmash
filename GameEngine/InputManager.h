/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "Settings.h"
#include "InputGroup.h"

class InputManager
{
public:
    InputManager();
    InputManager(InputManager const&) = delete;
    InputManager& operator=(InputManager const&) = delete;
    ~InputManager();

    std::set<SDL_GameController *> GetGameControllers();
    int GetGameControllerCount() const;

    void ProcessEvents();

    void SetKeyboardIsPlayer(bool keyboardIsPlayer);
    bool KeyboardIsPlayer() const;
    void ResetPlayerMapping();

    void AddInputGroup(int groupID, InputGroup *inputGroup);
    InputGroup *GetInputGroup(int groupID);

    void SetMaxPlayerCount(int maxPlayerCount);
    int GetMaxPlayerCount() const;

protected:
    void AddGameController(int joystickID);
    void RemoveGameController(int joystickID);
    void UpdatePlayerCount();

    std::map<int, InputGroup *> m_inputMap;

    int m_maxPlayerCount;
    int m_playerCount;
    bool m_keyboardIsPlayer;
    std::set<SDL_GameController *> m_gameControllers;
};

inline SDL_JoystickID getControllerInstanceID(SDL_GameController *controller)
{
    return SDL_JoystickInstanceID(
        SDL_GameControllerGetJoystick(controller));
}

inline void InputManager::SetKeyboardIsPlayer(bool keyboardIsPlayer)
{
    m_keyboardIsPlayer = keyboardIsPlayer;
    ResetPlayerMapping();
}

inline bool InputManager::KeyboardIsPlayer() const
{
    return m_keyboardIsPlayer;
}

inline void InputManager::SetMaxPlayerCount(int maxPlayerCount)
{
    m_maxPlayerCount = maxPlayerCount;
}

inline int InputManager::GetMaxPlayerCount() const
{
    return m_maxPlayerCount;
}
