/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "UIPauseMenu.h"
#include "StageManager.h"
#include "UIDefaultCursor.h"

UIPauseMenu::UIPauseMenu(Scene *scene) :
    UIObject(scene)
{
    SetName("UIPauseMenu");
    SetParent(m_scene->GetCanvas());

    InputManager *inputManager = m_scene->GetInputManager();
    ApplicationInput *applicationInput = ApplicationInput::GetFromManager(inputManager);

    AssetManager *assets = scene->GetAssetManager();
    SpriteSheet *spriteSheet = nullptr;
    SpriteGroup *spriteGroup = nullptr;
    TTF_Font *font = nullptr;

    m_group = new UISelectableGroup(m_scene);
    m_group->SetParent(this);
    m_group->AddInput(&(applicationInput->uiInputs[0]));
    m_group->SetInteractable(true);

    //--------------------------------------------------------------------------
    // Title text

    font = assets->GetFont(FONT_LARGE);
    m_titleText = new UIText(scene, "Pause", font, Colors::Gold);
    m_titleText->SetName("UI Pause title");
    m_titleText->SetAnchor(Anchor::CENTER);

    //--------------------------------------------------------------------------
    // Fill

    m_pauseFill = new UIFillRect(scene, Colors::DarkBlue);
    m_pauseFill->SetParent(this);
    m_pauseFill->SetLayer(LAYER_UI_BACKGROUND);
    m_pauseFill->SetOpacity(0.5f);
    m_pauseFill->GetLocalRect().anchorMin.y = 0.33f;
    m_pauseFill->GetLocalRect().anchorMax.y = 0.67f;

    //--------------------------------------------------------------------------
    // Buttons

    m_continueButton = new UIDefaultButton(scene, "Resume");
    m_continueButton->AddSelectableListener(this);
    m_continueButton->SetSoundOnClick(SFX_UI_UNPAUSE);

    m_mainMenuButton = new UIDefaultButton(scene, "Quit to menu");
    m_mainMenuButton->AddSelectableListener(this);

    //--------------------------------------------------------------------------
    // Cursor

    UIDefaultCursor *cursor = new UIDefaultCursor(scene);
    m_group->SetCursor(cursor);

    //--------------------------------------------------------------------------
    // Grid layout

    UIGridLayout *vLayout = new UIGridLayout(m_scene, 3, 1);
    vLayout->SetParent(this);
    vLayout->SetAnchor(Anchor::CENTER);
    vLayout->SetSpacing(5.f);
    vLayout->SetRowSize(22.f);
    vLayout->SetColumnSize(100.f);

    vLayout->AddObject(m_titleText, 0, 0);
    vLayout->AddObject(m_continueButton, 1, 0);
    vLayout->AddObject(m_mainMenuButton, 2, 0);

    //--------------------------------------------------------------------------
    // Fade In/Out

    InitFadeAnim();

    //--------------------------------------------------------------------------
    // Navigation

    m_group->AddSelectable(m_continueButton);
    m_group->AddSelectable(m_mainMenuButton);
    m_group->ComputeAutoNavigation();

    m_group->SetSelected(m_continueButton);
    m_group->SetCursorOnSelected();
}

void UIPauseMenu::OnClick(UISelectable *which)
{
    StageManager *stageManager = StageManager::GetFromScene(m_scene);
    if (stageManager == nullptr) return;

    if (which == m_continueButton)
    {
        m_group->SetInteractable(false);
        PlayFadeOut();
    }
    else if (which == m_mainMenuButton)
    {
        stageManager->QuitScene();
    }
}

void UIPauseMenu::OnPlayFadeIn()
{
    PlayFadeInAlone();

    UIObject *objects[] = {
        m_titleText, m_continueButton, m_mainMenuButton,
        m_group->GetCursor(), m_pauseFill
    };
    for (UIObject *object : objects)
    {
        object->PlayFadeIn();
    }
}

void UIPauseMenu::OnPlayFadeOut()
{
    PlayFadeOutAlone();

    UIObject *objects[] = {
        m_titleText, m_continueButton, m_mainMenuButton,
        m_group->GetCursor(), m_pauseFill
    };
    for (UIObject *object : objects)
    {
        object->PlayFadeOut();
    }
}

void UIPauseMenu::OnFadeOutEnd(UIObject *which)
{
    if (which == this)
    {
        StageManager *stageManager = StageManager::GetFromScene(m_scene);
        if (stageManager) stageManager->QuitPause();
    }
}

void UIPauseMenu::InitFadeAnim()
{
    UIFadeDef fadeDef;
    fadeDef.Reset(0.5f);

    SetFadeInAnimation(fadeDef);
    SetFadeOutAnimation(fadeDef);
    SetFadeChildren(false);

    fadeDef.Reset(0.5f);
    fadeDef.fadeOpacity = true;
    fadeDef.shift = b2Vec2(0.f, 10.f);

    m_titleText->SetFadeInAnimation(fadeDef);
    m_continueButton->SetFadeInAnimation(fadeDef);
    m_mainMenuButton->SetFadeInAnimation(fadeDef);
    m_group->GetCursor()->SetFadeInAnimation(fadeDef);

    fadeDef.Reset(0.5f);
    fadeDef.fadeOpacity = true;
    fadeDef.shift = b2Vec2(0.f, -10.f);

    m_titleText->SetFadeOutAnimation(fadeDef);
    m_continueButton->SetFadeOutAnimation(fadeDef);
    m_mainMenuButton->SetFadeOutAnimation(fadeDef);
    m_group->GetCursor()->SetFadeOutAnimation(fadeDef);

    fadeDef.Reset(0.5f);
    fadeDef.fadeOpacity = true;
    m_pauseFill->SetFadeInAnimation(fadeDef);
    m_pauseFill->SetFadeOutAnimation(fadeDef);
}
