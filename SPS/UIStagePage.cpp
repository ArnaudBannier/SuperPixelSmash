/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "UIStagePage.h"
#include "TitleManager.h"
#include "UIDefaultCursor.h"

UIStagePage::UIStagePage(
    Scene *scene, std::array<PlayerConfig, MAX_PLAYER_COUNT> &playerConfigs,
    StageConfig &stageConfig) :
    UIObject(scene), m_stageConfig(stageConfig), m_playerConfigs(playerConfigs),
    m_ratioHeader(0.2f),
    m_header(nullptr), m_content(nullptr), m_group(nullptr)
{
    SetName("UIStagePage");
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
    // Header

    m_header = new UIObject(scene);
    m_header->SetName("UI Stage Header panel");
    m_header->SetParent(this);
    m_header->GetLocalRect().anchorMin.Set(0.f, 1.f - m_ratioHeader);
    m_header->GetLocalRect().anchorMax.Set(1.f, 1.f);

    UIFillRect *headerFill = new UIFillRect(scene, Colors::DarkBlue);
    headerFill->SetParent(m_header);
    headerFill->SetOpacity(0.5f);
    headerFill->SetLayer(LAYER_UI_BACKGROUND);

    //--------------------------------------------------------------------------
    // Content

    m_content = new UIObject(scene);
    m_content->SetName("UI Stage Content panel");
    m_content->SetParent(this);
    m_content->GetLocalRect().anchorMin.Set(0.f, 0.f);
    m_content->GetLocalRect().anchorMax.Set(1.f, 1.f - m_ratioHeader);

    //--------------------------------------------------------------------------
    // Title text

    font = assets->GetFont(FONT_LARGE);
    m_titleText = new UIText(scene, "Stage selection", font, Colors::Gold);
    m_titleText->SetParent(m_header);
    m_titleText->SetAnchor(Anchor::CENTER);
    
    //--------------------------------------------------------------------------
    // Player text

    font = assets->GetFont(FONT_LARGE);
    m_playerText = new UIText(scene, "Fighter selection", font, Colors::Black);
    m_playerText->SetAnchor(Anchor::WEST);

    //--------------------------------------------------------------------------
    // Options text

    font = assets->GetFont(FONT_LARGE);
    m_optionText = new UIText(scene, "Options", font, Colors::Black);
    m_optionText->SetAnchor(Anchor::WEST);

    //--------------------------------------------------------------------------
    // Cursor

    UIDefaultCursor *cursor = new UIDefaultCursor(scene);
    m_group->SetCursor(cursor);

    //--------------------------------------------------------------------------
    // Player list

    std::string labelString;
    std::vector<std::string> textStrings;
    const float textWidth = 95.f;

    // Player 1
    labelString.assign("Player 1");
    textStrings.push_back("Fire warrior");
    textStrings.push_back("Lighting warrior");
    m_player1List = new UITextList(
        m_scene, labelString, textStrings, Colors::White, textWidth
    );
    m_player1List->SetIsCycle(true);
    m_player1List->AddSelectableListener(this);

    // Player 2
    labelString.assign("Player 2");
    m_player2List = new UITextList(
        m_scene, labelString, textStrings, Colors::White, textWidth
    );
    m_player2List->SetIsCycle(true);
    m_player2List->AddSelectableListener(this);

    //--------------------------------------------------------------------------
    // Stage duration list

    labelString.assign("Stage duration");
    textStrings.clear();
    textStrings.push_back("1 min");
    textStrings.push_back("2 min");
    textStrings.push_back("3 min");
    m_timeList = new UITextList(
        m_scene, labelString, textStrings, Colors::White, textWidth
    );
    m_timeList->SetIsCycle(false);

    ////--------------------------------------------------------------------------
    //// TODO : Ajout d'un texte et d'une liste pour la fréquence d'apparition des potions


    //--------------------------------------------------------------------------
    // Buttons

    m_startButton = new UIDefaultButton(scene, "Start");
    m_startButton->AddSelectableListener(this);

    m_backButton = new UIDefaultButton(scene, "Back");
    m_backButton->AddSelectableListener(this);

    //--------------------------------------------------------------------------
    // Grid layouts

    // TODO : Modifier le vLayout pour ajouter un champ potion
    UIGridLayout *vLayout = new UIGridLayout(m_scene, 6, 1); 
    vLayout->SetParent(m_content);
    vLayout->SetAnchor(Anchor::CENTER);
    vLayout->SetSpacing(2.f);
    vLayout->SetRowSpacing(2, 10.f);
    vLayout->SetRowSpacing(4, 20.f);
    vLayout->SetRowSize(22.f);
    vLayout->SetColumnSize(300.f);

    vLayout->AddObject(m_playerText, 0, 0);
    vLayout->AddObject(m_player1List, 1, 0);
    vLayout->AddObject(m_player2List, 2, 0);
    vLayout->AddObject(m_optionText, 3, 0);
    vLayout->AddObject(m_timeList, 4, 0);
    // TODO : Ajout de l'objet potion

    UIGridLayout *hLayout = new UIGridLayout(m_scene, 1, 2);
    hLayout->SetSpacing(20.f);
    hLayout->SetAnchor(Anchor::CENTER);
    hLayout->SetColumnSize(100.f);

    hLayout->AddObject(m_backButton, 0, 0);
    hLayout->AddObject(m_startButton, 0, 1);

    // TODO : Modifier pour laisser la place au champ potion
    vLayout->AddObject(hLayout, 5, 0);

    vLayout->Update();
    hLayout->Update();

    //--------------------------------------------------------------------------
    // Fade In/Out et Initialisation

    InitFadeAnim();
    InitPageWithConfig();

    //--------------------------------------------------------------------------
    // Navigation

    m_group->AddSelectable(m_player1List);
    m_group->AddSelectable(m_player2List);
    m_group->AddSelectable(m_timeList);
    m_group->AddSelectable(m_backButton);
    m_group->AddSelectable(m_startButton);
    // TODO : Ajouter la potion

    m_group->ComputeAutoNavigation();

    m_group->SetSelected(m_player1List);
    m_group->SetCursorOnSelected();
}

UIStagePage::~UIStagePage()
{
}

void UIStagePage::Update()
{
    UIObject::Update();

    if (IsFadingIn() || IsFadingOut()) return;

    TitleManager *titleManager = TitleManager::GetFromScene(m_scene);
    if (titleManager == nullptr) return;

    AssetManager *assets = m_scene->GetAssetManager();
    InputManager *inputManager = m_scene->GetInputManager();
    ApplicationInput *applicationInput = ApplicationInput::GetFromManager(inputManager);

    for (const UIInput &input : applicationInput->uiInputs)
    {
        if (input.start)
        {
            m_scene->GetSceneManager()->QuitScene();

            assets->PlaySoundFX(SFX_UI_VALIDATE);

            UpdateConfigs();
            m_group->SetSelected(nullptr);
            m_group->SetInteractable(false);

            titleManager->QuitScene();
        }
    }
}

void UIStagePage::OnClick(UISelectable *which)
{
    TitleManager *titleManager = TitleManager::GetFromScene(m_scene);
    if (titleManager == nullptr) return;

    if (which == m_startButton)
    {
        UpdateConfigs();
        m_group->SetSelected(nullptr);
        m_group->SetInteractable(false);

        titleManager->QuitScene();
    }
    else if (which == m_backButton)
    {
        UpdateConfigs();
        PlayFadeOut();
        m_group->SetSelected(nullptr);
        m_group->SetInteractable(false);

        titleManager->OpenPage(TitleManager::Page::TITLE);
    }
}

void UIStagePage::OnItemChanged(UISelectable *which, int itemIdx, int prevItemIdx, bool increase)
{
    UpdateConfigs();
}

void UIStagePage::OnFadeOutEnd(UIObject *which)
{
    if (which != this) return;

    TitleManager *titleManager = TitleManager::GetFromScene(m_scene);
    if (titleManager == nullptr) return;

    titleManager->QuitPage(TitleManager::Page::STAGE);
}

void UIStagePage::InitFadeAnim()
{
    UIFadeDef fadeDef;
    fadeDef.Reset(1.f);
    fadeDef.fadeOpacity = true;

    SetFadeInAnimation(fadeDef);
    SetFadeOutAnimation(fadeDef);
    SetFadeChildren(false);
}

void UIStagePage::InitPageWithConfig()
{
    switch (m_playerConfigs[0].type)
    {
    default:
    case PlayerConfig::Type::FIRE_WARRIOR: 
        m_player1List->SetFirstSelectedItem(0); break;
    case PlayerConfig::Type::LIGHTNING_WARRIOR: 
        m_player1List->SetFirstSelectedItem(1); break;
    }

    switch (m_playerConfigs[1].type)
    {
    default:
    case PlayerConfig::Type::FIRE_WARRIOR: 
        m_player2List->SetFirstSelectedItem(0); break;
    case PlayerConfig::Type::LIGHTNING_WARRIOR: 
        m_player2List->SetFirstSelectedItem(1); break;
    }

    switch (m_stageConfig.duration)
    {
    default:
    case  1: m_timeList->SetFirstSelectedItem(0); break;
    case  2: m_timeList->SetFirstSelectedItem(1); break;
    case  3: m_timeList->SetFirstSelectedItem(2); break;
    }

    // TODO : Gérer la fréquence de la potion
}

void UIStagePage::UpdateConfigs()
{
    switch (m_player1List->GetSelectedItem())
    {
    default:
    case 0: m_playerConfigs[0].type = PlayerConfig::Type::FIRE_WARRIOR; break;
    case 1: m_playerConfigs[0].type = PlayerConfig::Type::LIGHTNING_WARRIOR; break;
    }

    switch (m_player2List->GetSelectedItem())
    {
    default:
    case 0: m_playerConfigs[1].type = PlayerConfig::Type::FIRE_WARRIOR; break;
    case 1: m_playerConfigs[1].type = PlayerConfig::Type::LIGHTNING_WARRIOR; break;
    }

    switch (m_timeList->GetSelectedItem())
    {
    default:
    case 0: m_stageConfig.duration = 1; break;
    case 1: m_stageConfig.duration = 2; break;
    case 2: m_stageConfig.duration = 3; break;
    }

    // TODO : Gérer la fréquence de la potion
}
