/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "UIStageHUD.h"
#include "StageManager.h"

UIStageHUD::UIStageHUD(Scene *scene) :
    UIObject(scene), m_damageTexts(), m_timeText(nullptr)
{
    SetName("UIStageHUD");
    SetFadeChildren(false);

    AssetManager *assets = scene->GetAssetManager();
    SpriteSheet *spriteSheet = nullptr;
    SpriteGroup *spriteGroup = nullptr;
    TTF_Font *font = nullptr;

    StageManager *stageManager = StageManager::GetFromScene(m_scene);
    int playerCount = stageManager->GetPlayerCount();

    //--------------------------------------------------------------------------
    // Grid layout

    UIGridLayout *hLayout = new UIGridLayout(m_scene, 1, playerCount);
    hLayout->SetParent(this);
    hLayout->SetAnchor(Anchor::SOUTH);
    hLayout->SetPadding(10.f);
    hLayout->SetColumnSpacing(50.f);
    hLayout->SetRowSize(30.f);
    hLayout->SetColumnSize(80.f);

    //--------------------------------------------------------------------------
    // Informations sur les joueurs

    for (int i = 0; i < playerCount; i++)
    {
        SpriteSheet *spriteSheet = assets->GetSpriteSheet(SHEET_UI);
        AssertNew(spriteSheet);
        SpriteGroup *spriteGroup = spriteSheet->GetGroup("PlayerBorder");
        AssertNew(spriteGroup);

        UIImage *fillImage = new UIImage(m_scene, spriteGroup, 1);
        fillImage->SetBorders(6, 6, 6, 6, 4.f);
        fillImage->SetOpacity(0.5f);
        fillImage->SetLayer(LAYER_UI_BACKGROUND);
        fillImage->SetParent(this);

        hLayout->AddObject(fillImage, 0, i);

        // Compteur des dégats
        font = assets->GetFont(FONT_DAMAGE);
        UIText *text = new UIText(scene, "0%", font, Colors::White);
        text->SetAnchor(Anchor::CENTER);

        hLayout->AddObject(text, 0, i);

        m_damageTexts.push_back(text);
    }

    // Compteur du temps restant
    font = assets->GetFont(FONT_TIME);
    m_timeText = new UIText(scene, "0:00", font, Colors::White);
    m_timeText->SetParent(this);
    m_timeText->SetAnchor(Anchor::NORTH_EAST);
    m_timeText->GetLocalRect().offsetMax.Set(-10.f, -10.f);
}

void UIStageHUD::Update()
{
    UIObject::Update();

    StageManager *stageManager = StageManager::GetFromScene(m_scene);
    int playerCount = stageManager->GetPlayerCount();

    int minutes = (int)stageManager->GetRemainingTime() / 60;
    int seconds = (int)stageManager->GetRemainingTime() % 60;
    char buffer[128] = { 0 };

    sprintf_s(buffer, "%d:%02d", minutes, seconds);
    m_timeText->SetString(buffer);

    for (int i = 0; i < playerCount; i++)
    {
        Player *player = stageManager->GetPlayer(i);
        int score = (int)player->GetEjectionScore();

        m_damageTexts[i]->SetString(std::to_string(score) + "%");
    }
}
