/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "InputManager.h"
#include "Utils.h"

InputManager::InputManager() :
    m_inputMap(), m_gameControllers(), m_keyboardIsPlayer(false),
    m_playerCount(0), m_maxPlayerCount(4)
{
    for (int i = 0; i < SDL_NumJoysticks(); i++)
    {
        if (SDL_IsGameController(i))
        {
            SDL_GameController *controller = SDL_GameControllerOpen(i);
            m_gameControllers.insert(controller);
        }
    }
    ResetPlayerMapping();
}

InputManager::~InputManager()
{
    for (auto pair : m_inputMap)
    {
        delete pair.second;
    }

    for (SDL_GameController *controller : m_gameControllers)
    {
        SDL_GameControllerClose(controller);
    }
}

void InputManager::ProcessEvents()
{
    for (auto &pair : m_inputMap)
    {
        InputGroup *group = pair.second;
        if (group->IsEnabled())
        {
            group->OnPreEventProcess();
        }
    }

    SDL_Event evt;
    SDL_GameController *controller = nullptr;
    int playerID = 0;
    while (SDL_PollEvent(&evt))
    {
        bool skipEvent = false;

        switch (evt.type)
        {
        case SDL_CONTROLLERDEVICEADDED:
            AddGameController(evt.cdevice.which);
            break;

        case SDL_CONTROLLERDEVICEREMOVED:
            RemoveGameController(evt.cdevice.which);
            break;

        case SDL_CONTROLLERAXISMOTION:
        case SDL_CONTROLLERBUTTONDOWN:
        case SDL_CONTROLLERBUTTONUP:
        case SDL_CONTROLLERDEVICEREMAPPED:
        case SDL_CONTROLLERTOUCHPADDOWN:
        case SDL_CONTROLLERTOUCHPADMOTION:
        case SDL_CONTROLLERTOUCHPADUP:
        case SDL_CONTROLLERSENSORUPDATE:
            controller = SDL_GameControllerFromInstanceID(evt.caxis.which);
            if (controller == nullptr)
            {
                skipEvent = true;
                break;
            }
            playerID = SDL_GameControllerGetPlayerIndex(controller);
            if (playerID < 0 || playerID >= m_maxPlayerCount)
            {
                skipEvent = true;
                break;
            }
            break;

        default:
            break;
        }

        if (skipEvent) continue;


        for (auto &pair : m_inputMap)
        {
            InputGroup *group = pair.second;
            if (group->IsEnabled())
            {
                group->OnEventProcess(evt);
            }
        }
    }
}

void InputManager::AddGameController(int joystickID)
{
    bool *playerIDs = new bool[m_maxPlayerCount];
    std::fill_n(playerIDs, m_maxPlayerCount, false);

    for (SDL_GameController *controller : m_gameControllers)
    {
        int id = SDL_GameControllerGetPlayerIndex(controller);
        if (id < 0 || id >= m_maxPlayerCount) continue;

        playerIDs[id] = true;

        if (getControllerInstanceID(controller) == joystickID)
        {
            // Le controller existe déjà
            return;
        }
    }

    int playerID = m_keyboardIsPlayer ? 1 : 0;
    for (int i = playerID; i < m_maxPlayerCount; i++)
    {
        if (playerIDs[i]) continue;

        playerID = i;
        break;
    }

    SDL_GameController *controller = SDL_GameControllerOpen(joystickID);
    SDL_GameControllerSetPlayerIndex(controller, playerID);
    m_gameControllers.insert(controller);

    UpdatePlayerCount();
}

void InputManager::RemoveGameController(int joystickID)
{
    SDL_GameController *controller = SDL_GameControllerFromInstanceID(joystickID);
    if (controller == nullptr) return;

    m_gameControllers.erase(controller);
    SDL_GameControllerClose(controller);

    UpdatePlayerCount();
}

void InputManager::UpdatePlayerCount()
{
    m_playerCount = 1;

    bool *playerIDs = new bool[m_maxPlayerCount];
    std::fill_n(playerIDs, m_maxPlayerCount, false);
    playerIDs[0] = true;

    for (SDL_GameController *controller : m_gameControllers)
    {
        int id = SDL_GameControllerGetPlayerIndex(controller);
        if (id < 0 || id >= m_maxPlayerCount) continue;

        m_playerCount += playerIDs[id] ? 0 : 1;
        playerIDs[id] = true;
    }
}

void InputManager::ResetPlayerMapping()
{
    int playerID = m_keyboardIsPlayer ? 1 : 0;
    for (SDL_GameController *controller : m_gameControllers)
    {
        playerID = Math::Clamp(playerID, 0, m_maxPlayerCount - 1);
        SDL_GameControllerSetPlayerIndex(controller, playerID++);
    }
    UpdatePlayerCount();
}

void InputManager::AddInputGroup(int groupID, InputGroup *inputGroup)
{
    auto it = m_inputMap.find(groupID);
    if (it != m_inputMap.end())
    {
        delete it->second;
        m_inputMap.erase(groupID);
    }

    m_inputMap.insert(std::make_pair(groupID, inputGroup));
}

InputGroup *InputManager::GetInputGroup(int groupID)
{
    auto it = m_inputMap.find(groupID);
    if (it != m_inputMap.end()) return it->second;
    return nullptr;
}

std::set<SDL_GameController *> InputManager::GetGameControllers()
{
    return m_gameControllers;
}

int InputManager::GetGameControllerCount() const
{
    return (int)m_gameControllers.size();
}
