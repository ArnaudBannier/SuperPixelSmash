/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "GameSettings.h"
#include "GameCommon.h"
#include "BaseSceneManager.h"

#include "Player.h"
#include "MainCamera.h"

#include "UIPauseMenu.h"
#include "UIStageHUD.h"

class StageManager : public BaseSceneManager
{
public:
    StageManager(
        InputManager *inputManager,
        std::array<PlayerConfig, MAX_PLAYER_COUNT> &playerConfigs,
        StageConfig &stageConfig);
    virtual ~StageManager();

    virtual void OnSceneUpdate() override;
    virtual void OnSceneFixedUpdate() override;

    Player *GetPlayer(int playerID);
    int GetPlayerCount() const;

    PlayerConfig *GetPlayerConfig(int playerID);
    PlayerStats *GetPlayerStats(int playerID);

    StageConfig *GetStageConfig();
    float GetRemainingTime() const;

    void QuitPause();

    void SetKO(int playerID);
    bool IsPaused() const;

    static StageManager *GetFromScene(Scene *scene);

private:
    void InitRockyPass();
    void AddPotion();

    int m_playerCount;
    std::vector<Player *> m_players;
    std::vector<PlayerConfig> m_configs;
    std::vector<PlayerStats> m_playerStats;

    StageConfig &m_stageConfig;

    UIPauseMenu *m_pauseMenu;
    UIStageHUD *m_stageHUD;
    bool m_paused;

    float m_delayStage;
//    float m_delayPotion; // ADD
};

inline Player *StageManager::GetPlayer(int playerID)
{
    assert(0 <= playerID && playerID < m_playerCount);
    return m_players[playerID];
}

inline int StageManager::GetPlayerCount() const
{
    assert(m_playerCount == m_players.size());
    return m_playerCount;
}

inline PlayerConfig *StageManager::GetPlayerConfig(int playerID)
{
    assert(0 <= playerID && playerID < m_playerCount);
    return &m_configs[playerID];
}

inline PlayerStats *StageManager::GetPlayerStats(int playerID)
{
    assert(0 <= playerID && playerID < m_playerCount);
    return &m_playerStats[playerID];
}

inline float StageManager::GetRemainingTime() const
{
    return m_delayStage;
}

inline StageConfig *StageManager::GetStageConfig()
{
    return &m_stageConfig;
}

inline bool StageManager::IsPaused() const
{
    return m_paused;
}
