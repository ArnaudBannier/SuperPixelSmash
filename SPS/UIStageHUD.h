/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "GameSettings.h"
#include "GameCommon.h"

class UIStageHUD : public UIObject
{
public:
    UIStageHUD(Scene *scene);

    virtual void Update() override;

private:
    std::vector<UIText *>m_damageTexts;
    UIText *m_timeText;
};