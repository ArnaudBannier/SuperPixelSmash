/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "GameSettings.h"
#include "GameCommon.h"

class UITitleBackground : public UIObject
{
public:
    UITitleBackground(Scene *scene);
    virtual ~UITitleBackground();

    virtual void Update() override;
    virtual void Render() override;
};
