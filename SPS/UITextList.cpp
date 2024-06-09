/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "UITextList.h"

UITextList::UITextList(
    Scene *scene, const std::string &labelString,
    const std::vector<std::string> &textStrings, Color color,
    float textWidth, UISelectableGroup *group) :
    UIItemList(scene, (int)textStrings.size(), group)
{
    SetName("UITextList");

    const float paddingL = 10.f;
    const float paddingR = 10.f;

    AssetManager *assets = m_scene->GetAssetManager();
    TTF_Font *font = assets->GetFont(FONT_NORMAL);
    UIText *text = nullptr;

    SpriteSheet *spriteSheet = assets->GetSpriteSheet(SHEET_UI);
    AssertNew(spriteSheet);
    SpriteGroup *spriteGroup = spriteSheet->GetGroup("Arrows");
    AssertNew(spriteGroup);

    UIImage *prevArrow = new UIImage(m_scene, spriteGroup, 0);
    prevArrow->SetAnchor(Anchor::EAST);
    prevArrow->SetScale(4.f);

    UIImage *nextArrow = new UIImage(m_scene, spriteGroup, 1);
    nextArrow->SetAnchor(Anchor::EAST);
    nextArrow->SetScale(4.f);

    float arrowWidth = nextArrow->GetUISize().x;

    SetListNavigation(UIItemList::Navigation::HORIZONTAL);

    UIStateModifiers stateModifier;
    stateModifier.alphaMap[UISelectable::State::NORMAL] = 0.7f;
    stateModifier.alphaMap[UISelectable::State::SELECTED] = 1.0f;
    stateModifier.alphaMap[UISelectable::State::PRESSED] = 1.0f;

    text = new UIText(m_scene, labelString, font, color);
    text->SetAnchor(Anchor::WEST);
    text->GetLocalRect().offsetMin.x = +paddingL;
    AddLabelObject(text, stateModifier);

    float textPaddingR = paddingR + arrowWidth;
    for (int i = 0; i < textStrings.size(); i++)
    {
        font = assets->GetFont(FONT_NORMAL);
        text = new UIText(m_scene, textStrings[i], font, color);
        text->SetAnchor(Anchor::CENTER);
        text->GetLocalRect().anchorMin.x = 1.f;
        text->GetLocalRect().anchorMax.x = 1.f;
        text->GetLocalRect().offsetMin.x = -textPaddingR - textWidth;
        text->GetLocalRect().offsetMax.x = -textPaddingR;

        AddItemObject(i, text, stateModifier, UIItemList::ItemAnimationMode::SHIFT);
    }

    stateModifier.alphaMap[UISelectable::State::NORMAL] = 0.f;
    stateModifier.alphaMap[UISelectable::State::SELECTED] = 0.25f;
    stateModifier.alphaMap[UISelectable::State::PRESSED] = 0.25f;

    UIFillRect *fillRect = new UIFillRect(m_scene, Colors::Black);
    fillRect->SetLayer(LAYER_UI_BACKGROUND);
    AddLabelObject(fillRect, stateModifier);


    prevArrow->GetLocalRect().offsetMax.x = -textPaddingR - textWidth;
    nextArrow->GetLocalRect().offsetMax.x = -paddingR;

    AddNextArrow(nextArrow);
    AddPrevArrow(prevArrow);

    GetLocalCursorRect().anchorMin.Set(0.f, 0.5f);
    GetLocalCursorRect().anchorMax.Set(0.f, 0.5f);
    GetLocalCursorRect().offsetMin.Set(-5.f, -5.f);
    GetLocalCursorRect().offsetMax.Set(+5.f, +5.f);

    SetSoundOnClick(SFX_UI_CLICK);
    SetSoundOnSelected(SFX_UI_SELECTED);
    SetSoundOnItemChanged(SFX_UI_ITEM);
}
