/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "GameSettings.h"
#include "GameCommon.h"
#include "BaseSceneManager.h"

#include "UIStagePage.h"
#include "UITitlePage.h"
#include "UITitleBackground.h"

class TitleManager : public BaseSceneManager
{
public:
    TitleManager(
        InputManager *inputManager,
        std::array<PlayerConfig, MAX_PLAYER_COUNT> &playerConfigs,
        StageConfig &stageConfig, int &playerCount);
    virtual ~TitleManager();

    enum class Page
    {
        TITLE = 0, STAGE
    };

    void OpenPage(Page page);
    void OpenPage(int pageID);
    void QuitPage(Page page);
    void QuitPage(int pageID);

    static TitleManager *GetFromScene(Scene *scene);

private:
    std::array<PlayerConfig, MAX_PLAYER_COUNT> &m_playerConfigs;
    StageConfig &m_stageConfig;
    int &m_playerCount;

    UITitlePage *m_titlePage;
    UIStagePage *m_stagePage;

    UITitleBackground *m_background;

    bool m_quitGame;
};

inline void TitleManager::OpenPage(int pageID)
{
    OpenPage(static_cast<Page>(pageID));
}

inline void TitleManager::QuitPage(int pageID)
{
    QuitPage(static_cast<Page>(pageID));
}
