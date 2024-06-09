/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "GameSettings.h"
#include "GameCommon.h"
#include "UITextList.h"
#include "UIDefaultButton.h"

class UIStagePage : public UIObject, public UISelectableListener
{
public:
    UIStagePage(Scene *scene, std::array<PlayerConfig, MAX_PLAYER_COUNT> &playerConfigs, StageConfig &stageConfig);
    virtual ~UIStagePage();

    virtual void Update() override;

    virtual void OnClick(UISelectable *which) override;
    virtual void OnItemChanged(
        UISelectable *which, int itemIdx, int prevItemIdx, bool increase);

protected:
    virtual void OnFadeOutEnd(UIObject *which) override;

private:
    float m_ratioHeader;
    UIObject *m_header;
    UIObject *m_content;

    UIText *m_titleText;
    UIText *m_playerText;
    UIText *m_optionText;
    // TODO : texte pour la potion

    UITextList *m_player1List;
    UITextList *m_player2List;
    UITextList *m_timeList;
    // TODO : Liste de fréquence pour la potion

    UIDefaultButton *m_startButton;
    UIDefaultButton *m_backButton;

    UISelectableGroup *m_group;

    std::array<PlayerConfig, MAX_PLAYER_COUNT> &m_playerConfigs;
    StageConfig &m_stageConfig;

    void InitFadeAnim();
    void InitPageWithConfig();
    void UpdateConfigs();
};

