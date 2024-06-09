/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "Settings.h"
#include "UIBaseList.h"

class UIItemList : public UIBaseList
{
public:
    UIItemList(Scene *scene, int itemCount, UISelectableGroup *group = nullptr);
    virtual ~UIItemList();

    enum class ItemAnimationMode : int
    {
        SHIFT, ALPHA, NONE
    };

    void AddLabelObject(UIObject *object);
    void AddLabelObject(UIObject *object, const UIStateModifiers &stateModifiers);
    void RemoveLabelObject(UIObject *object);

    void AddItemObject(
        int itemIdx, UIObject *object,
        ItemAnimationMode mode = ItemAnimationMode::SHIFT);
    void AddItemObject(
        int itemIdx, UIObject *object, const UIStateModifiers &stateModifiers,
        ItemAnimationMode mode = ItemAnimationMode::SHIFT);
    void RemoveItemObject(int itemIdx, UIObject *object);

    void AddPrevArrow(UIObject *arrow);
    void AddNextArrow(UIObject *arrow);

    void SetItemShift(float shiftValue);
    void SetArrowShift(float shiftValue);

protected:
    virtual void OnItemChanged(int itemIdx, int prevItemIdx, bool increase) override;
    virtual void OnStateChanged(State state, State prevState, bool makeSound) override;
    virtual void OnSetFirstItem(int itemIdx, int prevItemIdx) override;

    virtual void OnPlayFadeIn() override;
    struct Label
    {
        Label() :
            object(nullptr), stateModifiers()
        {}

        UIObject *object;
        UIStateModifiers stateModifiers;
    };
    struct Item
    {
        Item() :
            object(nullptr), stateModifiers(),
            mode(ItemAnimationMode::SHIFT)
        {}

        UIObject *object;
        ItemAnimationMode mode;
        UIStateModifiers stateModifiers;
    };

    std::map<UIObject *, Label> m_labelMap;
    std::vector<std::map<UIObject *, Item>> m_itemMaps;
    float m_itemShiftValue;
    float m_arrowShiftValue;

    UIObject *m_nextArrow;
    UIObject *m_prevArrow;

private:
    void AddItemObject(int itemIdx, UIObject *object, Item &item);
    void AddLabelObject(UIObject *object, Label &label);
    void AddArrow(UIObject *object, b2Vec2 shift);
};

inline void UIItemList::SetItemShift(float shiftValue)
{
    m_itemShiftValue = shiftValue;
}

inline void UIItemList::SetArrowShift(float shiftValue)
{
    m_arrowShiftValue = shiftValue;
}
