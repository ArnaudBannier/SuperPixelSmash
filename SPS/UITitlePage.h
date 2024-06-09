/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "GameSettings.h"
#include "GameCommon.h"
#include "UIDefaultButton.h"
#include "UITextList.h"

class UITitlePage : public UIObject, public UISelectableListener
{
public:
    UITitlePage(Scene *scene);
    virtual ~UITitlePage();
    virtual void OnClick(UISelectable *which) override;

protected:
    virtual void OnFadeOutEnd(UIObject *which) override;

private:
    UIText *m_titleText;

    UIDefaultButton *m_startButton;
    UIDefaultButton *m_quitButton;

    UISelectableGroup *m_group;

    void InitFadeAnim();
};
