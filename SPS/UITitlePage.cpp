/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "UITitlePage.h"
#include "UIDefaultButton.h"
#include "UIDefaultCursor.h"
#include "TitleManager.h"

UITitlePage::UITitlePage(Scene *scene) :
    UIObject(scene),
    m_group(nullptr)
{
    SetName("UITitlePage");
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
    m_titleText = new UIText(scene, "Super Pixel Smash", font, Colors::Gold);
    m_titleText->SetName("UI Main title");
    m_titleText->SetAnchor(Anchor::CENTER);

    //--------------------------------------------------------------------------
    // Buttons

    m_startButton = new UIDefaultButton(scene, "Start");
    m_startButton->AddSelectableListener(this);

    m_quitButton = new UIDefaultButton(scene, "Quit");
    m_quitButton->AddSelectableListener(this);

    //--------------------------------------------------------------------------
    // Cursor

    UIDefaultCursor *cursor = new UIDefaultCursor(scene);
    m_group->SetCursor(cursor);

    //--------------------------------------------------------------------------
    // Grid layouts

    UIGridLayout *vLayout = new UIGridLayout(m_scene, 3, 1);
    vLayout->SetParent(this);
    vLayout->SetAnchor(Anchor::CENTER);
    vLayout->SetSpacing(5.f);
    vLayout->SetRowSize(22.f);
    vLayout->SetColumnSize(100.f);

    vLayout->AddObject(m_titleText, 0, 0);
    vLayout->AddObject(m_startButton, 1, 0);
    vLayout->AddObject(m_quitButton, 2, 0);

    //--------------------------------------------------------------------------
    // Fade In/Out

    InitFadeAnim();

    //--------------------------------------------------------------------------
    // Navigation

    m_group->RemoveAllUnselected();
    m_group->AddSelectable(m_startButton);
    m_group->AddSelectable(m_quitButton);

    m_group->ComputeAutoNavigation();

    m_group->SetSelected(m_startButton);
    m_group->SetCursorOnSelected();
}

UITitlePage::~UITitlePage()
{
}

void UITitlePage::InitFadeAnim()
{
    UIFadeDef fadeDef;
    fadeDef.Reset(1.f);
    fadeDef.fadeOpacity = true;

    SetFadeInAnimation(fadeDef);
    SetFadeOutAnimation(fadeDef);
    SetFadeChildren(false);
}

void UITitlePage::OnClick(UISelectable *which)
{
    TitleManager *titleManager = TitleManager::GetFromScene(m_scene);
    if (titleManager == nullptr) return;

    if (which == m_startButton)
    {
        PlayFadeOut();
        m_group->SetSelected(nullptr);
        m_group->SetInteractable(false);

        titleManager->OpenPage(TitleManager::Page::STAGE);
    }
    else if (which == m_quitButton)
    {
        PlayFadeOut();
        m_group->SetSelected(nullptr);
        m_group->SetInteractable(false);

        titleManager->QuitGame();
    }
}

void UITitlePage::OnFadeOutEnd(UIObject *which)
{
    if (which != this) return;

    TitleManager *titleManager = TitleManager::GetFromScene(m_scene);
    if (titleManager == nullptr) return;

    titleManager->QuitPage(TitleManager::Page::TITLE);
}
