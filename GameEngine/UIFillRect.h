/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "Settings.h"
#include "UIObject.h"

class UIFillRect : public UIObject
{
public:
    UIFillRect(Scene *scene, Color color);

    virtual void Update() override;
    virtual void Render() override;
};
