/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "TitleManager.h"

TitleManager::TitleManager(
    InputManager *inputManager,
    std::array<PlayerConfig, MAX_PLAYER_COUNT> &playerConfigs,
    StageConfig &stageConfig, int &playerCount) :
    BaseSceneManager(inputManager),
    m_playerConfigs(playerConfigs), m_stageConfig(stageConfig), m_playerCount(playerCount),
    m_titlePage(nullptr), m_stagePage(nullptr), m_quitGame(false)
{
    Scene *scene = GetScene();
    AssetManager *assets = scene->GetAssetManager();
    InitMusic(assets);
    InitAssets(assets);
    InitBackground(assets, (int)StageConfig::Type::ROCKY_PASS);
    InitFonts(assets);
    InitSFX(assets);

    scene->GetAssetManager()->FadeInMusic(MUSIC_MENU);

    // Crée la caméra
    SetMainCamera(new Camera(scene));

    // Fond
    m_background = new UITitleBackground(scene);

    // Première page
    OpenPage(Page::TITLE);
}

TitleManager::~TitleManager()
{
}

void TitleManager::OpenPage(Page page)
{
    Scene *scene = GetScene();
    UIObject *uiPage = nullptr;
    switch (page)
    {
    case Page::TITLE:
        if (m_titlePage) m_titlePage->Delete();
        m_titlePage = new UITitlePage(scene);
        uiPage = m_titlePage;
        break;

    case Page::STAGE:
        if (m_stagePage) m_stagePage->Delete();
        m_stagePage = new UIStagePage(scene, m_playerConfigs, m_stageConfig);
        uiPage = m_stagePage;
        break;

    default:
        assert(false);
        return;
    }

    uiPage->SetParent(scene->GetCanvas());
    uiPage->PlayFadeIn();
}

void TitleManager::QuitPage(Page page)
{
    UIObject *uiPage = nullptr;
    switch (page)
    {
    case Page::TITLE:
        uiPage = m_titlePage;
        m_titlePage = nullptr;
        break;

    case Page::STAGE:
        uiPage = m_stagePage;
        m_stagePage = nullptr;
        break;

    default:
        assert(false);
        return;
    }
    if (uiPage) uiPage->Delete();
}

TitleManager *TitleManager::GetFromScene(Scene *scene)
{
    if (scene == nullptr)
    {
        assert(false);
        return nullptr;
    }
    return dynamic_cast<TitleManager *>(scene->GetSceneManager());
}
