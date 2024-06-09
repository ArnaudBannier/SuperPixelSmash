/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "UIDefaultCursor.h"

UIDefaultCursor::UIDefaultCursor(Scene *scene) :
    UIImage(scene, nullptr, 0)
{
    SetName("UIDefaultCursor");
    AssetManager *assets = scene->GetAssetManager();
    SpriteSheet *spriteSheet = assets->GetSpriteSheet(SHEET_UI);
    AssertNew(spriteSheet);
    SpriteGroup *spriteGroup = spriteSheet->GetGroup("Cursor");
    AssertNew(spriteGroup);

    SetSpriteGroup(spriteGroup);
    SetLayer(LAYER_UI_FOREGROUND);
    SetAnchor(Anchor::EAST);
    SetScale(4.f);
    SetTransformDuration(0.25f);
    SetTransformEasing(EasingFct_Cos);

    Animation *anim = CreateShiftAnimation("Emph", b2Vec2_zero, b2Vec2(-2.f, 0.f));
    anim->SetCycleCount(-1);
    anim->SetCycleTime(1.5f);
    anim->SetEasing(EasingFct_Cos);
    anim->AddFlags(AnimFlag::ALTERNATE);

    anim = CreateAlphaAnimation("Emph", 1.f, 0.5f);
    anim->SetCycleCount(-1);
    anim->SetCycleTime(1.0f);
    anim->SetEasing(EasingFct_Cos);
    anim->AddFlags(AnimFlag::ALTERNATE);

    PlayShiftAnim("Emph");
    PlayAlphaAnim("Emph");
}
