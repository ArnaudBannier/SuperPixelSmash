/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "UIItemList.h"
#include "UICanvas.h"

UIItemList::UIItemList(Scene *scene, int itemCount, UISelectableGroup *group) :
    UIBaseList(scene, itemCount, group),
    m_itemMaps(), m_labelMap(), m_prevArrow(nullptr), m_nextArrow(nullptr),
    m_itemShiftValue(8.f), m_arrowShiftValue(3.f)
{
    SetName("UIItemList");
    SetFadeChildren(false);

    for (int i = 0; i < m_itemCount; i++)
    {
        m_itemMaps.push_back(std::map<UIObject*, Item>());
    }
}

UIItemList::~UIItemList()
{
}

void UIItemList::AddLabelObject(UIObject *object)
{
    Label label;
    label.object = object;

    AddLabelObject(object, label);
}

void UIItemList::AddLabelObject(
    UIObject *object, const UIStateModifiers &stateModifiers)
{
    Label label;
    label.object = object;
    label.stateModifiers = stateModifiers;

    AddLabelObject(object, label);
}

void UIItemList::RemoveLabelObject(UIObject *object)
{
    auto it = m_labelMap.find(object);
    if (it == m_labelMap.end())
    {
        return;
    }

    object->SetParent(m_scene->GetCanvas());
    object->SetUseParentAnimation(false);

    m_labelMap.erase(it);
}

void UIItemList::AddItemObject(
    int itemIdx, UIObject *object, ItemAnimationMode mode)
{
    Item item;
    item.object = object;
    item.mode = mode;

    AddItemObject(itemIdx, object, item);
}

void UIItemList::OnPlayFadeIn()
{
    PlayFadeInAlone();

    for (auto &it : m_labelMap)
    {
        UIObject *object = it.first;
        object->PlayFadeIn();
    }
    for (auto &it : m_itemMaps[m_itemIdx])
    {
        UIObject *object = it.first;
        object->PlayFadeIn();
    }
}

void UIItemList::AddItemObject(
    int itemIdx, UIObject *object, const UIStateModifiers &stateModifiers,
    ItemAnimationMode mode)
{
    Item item;
    item.object = object;
    item.mode = mode;
    item.stateModifiers = stateModifiers;

    AddItemObject(itemIdx, object, item);
}

void UIItemList::RemoveItemObject(int itemIdx, UIObject *object)
{
    if (itemIdx < 0 || itemIdx >= m_itemCount)
    {
        assert(false);
        return;
    }

    auto it = m_itemMaps[itemIdx].find(object);
    if (it == m_itemMaps[itemIdx].end())
    {
        return;
    }

    object->SetParent(m_scene->GetCanvas());
    object->SetUseParentAnimation(false);

    m_itemMaps[itemIdx].erase(it);
}

void UIItemList::OnItemChanged(int itemIdx, int prevItemIdx, bool increase)
{
    UIBaseList::OnItemChanged(itemIdx, prevItemIdx, increase);

    if (increase)
    {
        if (m_nextArrow) m_nextArrow->PlayShiftAnim("Move");
    }
    else
    {
        if (m_prevArrow) m_prevArrow->PlayShiftAnim("Move");
    }

    if (m_isCycle == false)
    {
        if (m_prevArrow && itemIdx == 0)
        {
            m_prevArrow->SetTargetOpacity(0.5f);
            m_prevArrow->PlayTransformToTarget();
        }
        if (m_prevArrow && prevItemIdx == 0)
        {
            m_prevArrow->SetTargetOpacity(1.f);
            m_prevArrow->PlayTransformToTarget();
        }
        if (m_nextArrow && itemIdx == m_itemCount - 1)
        {
            m_nextArrow->SetTargetOpacity(0.5f);
            m_nextArrow->PlayTransformToTarget();
        }
        if (m_nextArrow && prevItemIdx == m_itemCount - 1)
        {
            m_nextArrow->SetTargetOpacity(1.f);
            m_nextArrow->PlayTransformToTarget();
        }
    }


    for (auto &it : m_itemMaps[prevItemIdx])
    {
        UIObject *object = it.first;
        Item &item = it.second;
        const float sign = increase ? 1.f : -1.f;
        UIFadeAnim anim = object->GetFadeOutAnim();

        switch (item.mode)
        {
        case UIItemList::ItemAnimationMode::SHIFT:
            if (anim.shiftAnim != nullptr)
            {
                b2Vec2 shift = b2Vec2_zero;
                if (GetListNavigation() == Navigation::VERTICAL)
                    shift.y = sign * m_itemShiftValue;
                else
                    shift.x = sign * m_itemShiftValue;

                anim.shiftAnim->SetValues(b2Vec2_zero, shift);
            }
            object->PlayFadeOut();
            break;

        case UIItemList::ItemAnimationMode::ALPHA:
            object->PlayFadeOut();
            break;

        default:
        case UIItemList::ItemAnimationMode::NONE:
            object->SetUIEnabled(false);
            break;
        }
    }

    for (auto &it : m_itemMaps[itemIdx])
    {
        UIObject *object = it.first;
        Item &item = it.second;
        const float sign = increase ? -1.f : 1.f;
        UIFadeAnim anim = object->GetFadeInAnim();

        switch (item.mode)
        {

        case UIItemList::ItemAnimationMode::SHIFT:
            if (anim.shiftAnim != nullptr)
            {
                b2Vec2 shift = b2Vec2_zero;
                if (GetListNavigation() == Navigation::VERTICAL)
                    shift.y = sign * m_itemShiftValue;
                else
                    shift.x = sign * m_itemShiftValue;

                anim.shiftAnim->SetValues(shift, b2Vec2_zero);
            }
            object->PlayFadeIn();
            break;

        case UIItemList::ItemAnimationMode::ALPHA:
            object->PlayFadeIn();
            break;

        default:
        case UIItemList::ItemAnimationMode::NONE:
            object->SetUIEnabled(true);
            break;
        }
    }
}

void UIItemList::OnStateChanged(State state, State prevState, bool makeSound)
{
    UIBaseList::OnStateChanged(state, prevState, makeSound);

    for (auto &it : m_labelMap)
    {
        UIObject *object = it.first;
        Label &label = it.second;

        label.stateModifiers.SetObjectTarget(object, state);
        object->PlayTransformToTarget();
    }

    for (int i = 0; i < m_itemCount; i++)
    {
        for (auto &it : m_itemMaps[i])
        {
            UIObject *object = it.first;
            Item &item = it.second;

            item.stateModifiers.SetObjectTarget(object, state);

            if (i == m_itemIdx)
                object->PlayTransformToTarget();
            else
                object->TransformToTarget();
        }
    }

    if (state == State::PRESSED)
    {
        if (m_prevArrow)
        {
            if (m_isCycle == false && m_itemIdx == 0)
                m_prevArrow->SetOpacity(0.5f);

            m_prevArrow->PlayFadeIn();
        }
        if (m_nextArrow)
        {
            if (m_isCycle == false && m_itemIdx == m_itemCount - 1)
                m_nextArrow->SetOpacity(0.5f);

            m_nextArrow->PlayFadeIn();
        }
    }
    if (prevState == State::PRESSED)
    {
        if (m_prevArrow) m_prevArrow->PlayFadeOut();
        if (m_nextArrow) m_nextArrow->PlayFadeOut();
    }
}

void UIItemList::OnSetFirstItem(int itemIdx, int prevItemIdx)
{
    UIBaseList::OnSetFirstItem(itemIdx, prevItemIdx);

    for (auto &it : m_itemMaps[prevItemIdx])
    {
        UIObject *object = it.first;
        object->SetUIEnabled(false);
    }
    for (auto &it : m_itemMaps[itemIdx])
    {
        UIObject *object = it.first;
        object->SetUIEnabled(true);
    }
}

void UIItemList::AddPrevArrow(UIObject *arrow)
{
    m_prevArrow = arrow;
    b2Vec2 shift = b2Vec2_zero;
    if (GetListNavigation() == Navigation::VERTICAL)
        shift.y = -m_arrowShiftValue;
    else
        shift.x = -m_arrowShiftValue;

    AddArrow(arrow, shift);
}

void UIItemList::AddNextArrow(UIObject *arrow)
{
    m_nextArrow = arrow;
    b2Vec2 shift = b2Vec2_zero;
    if (GetListNavigation() == Navigation::VERTICAL)
        shift.y = +m_arrowShiftValue;
    else
        shift.x = +m_arrowShiftValue;

    AddArrow(arrow, shift);
}

void UIItemList::AddItemObject(int itemIdx, UIObject *object, Item &item)
{
    if (itemIdx < 0 || itemIdx >= m_itemCount)
    {
        assert(false);
        return;
    }

    object->SetParent(this);
    object->SetUseParentAnimation(true);
    object->SetUseParentAlpha(true);
    object->SetTransformDuration(0.1f);
    object->SetUIEnabled(itemIdx == m_itemIdx);

    item.stateModifiers.SetObjectTarget(object, GetState());
    object->TransformToTarget();

    auto it = m_itemMaps[itemIdx].find(object);
    if (it != m_itemMaps[itemIdx].end())
    {
        it->second = item;
    }
    else
    {
        m_itemMaps[itemIdx].insert(std::make_pair(object, item));
    }

    if (item.mode != ItemAnimationMode::NONE)
    {
        UIFadeDef fadeDef;
        fadeDef.duration = 0.25f;
        fadeDef.fadeOpacity = true;
        fadeDef.easing = EasingFct_Cos;
        object->SetFadeInAnimation(fadeDef);
        object->SetFadeOutAnimation(fadeDef);
    }
}

void UIItemList::AddLabelObject(UIObject *object, Label &label)
{
    object->SetParent(this);
    object->SetUseParentAnimation(true);
    object->SetUseParentAlpha(true);
    object->SetTransformDuration(0.1f);

    auto it = m_labelMap.find(object);
    if (it != m_labelMap.end())
    {
        it->second = label;
    }
    else
    {
        m_labelMap.insert(std::make_pair(object, label));
    }

    label.stateModifiers.SetObjectTarget(object, GetState());
    object->TransformToTarget();
}

void UIItemList::AddArrow(UIObject *object, b2Vec2 shift)
{
    object->SetParent(this);
    object->SetUseParentAnimation(true);
    object->SetUseParentAlpha(true);
    object->SetUIEnabled(false);
    object->SetTransformDuration(0.1f);

    LerpAnim<b2Vec2> *shiftAnim = nullptr;

    shiftAnim = object->CreateShiftAnimation("Move", b2Vec2_zero, shift);
    shiftAnim->SetCycleCount(1);
    shiftAnim->SetCycleTime(0.1f);
    shiftAnim->AddFlags(AnimFlag::ALTERNATE);

    UIFadeDef fadeDef;
    fadeDef.duration = 0.25f;
    fadeDef.fadeOpacity = true;
    fadeDef.easing = EasingFct_InOut;
    object->SetFadeInAnimation(fadeDef);
    object->SetFadeOutAnimation(fadeDef);
}
