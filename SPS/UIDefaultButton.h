/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "GameSettings.h"
#include "GameCommon.h"

class UIDefaultButton : public UIButton
{
public:
    UIDefaultButton(
        Scene *scene,
        const std::string &labelString,
        UISelectableGroup *group = nullptr
    );
};
