/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "UIBaseList.h"

UIBaseList::UIBaseList(Scene *scene, int itemCount, UISelectableGroup *group) :
    UISelectable(scene, group), m_itemCount(itemCount), m_itemIdx(0),
    m_isCycle(true), m_listNavigation(Navigation::ALL), m_autoCancel(true), m_soundItem(-1)
{
    SetName("UIBaseList");
    SetClickedOnRelease(false);
}

UIBaseList::~UIBaseList()
{
}

void UIBaseList::Update()
{
    if (IsUIEnabled() == false) return;

    UISelectable::Update();
    SetVisible(true);

    if (GetState() != State::PRESSED) return;

    auto *inputs = GetInputs();
    if (inputs == nullptr) return;

    for (auto input : *inputs)
    {
        if (input->cancel || input->validatePressed)
        {
            SetHandlesInputs(false);
            return;
        }

        bool goNext = false;
        bool goPrev = false;

        switch (m_listNavigation)
        {
        case UIBaseList::Navigation::HORIZONTAL:
            if (GetAutoCancel() && input->up)
            {
                SetHandlesInputs(false);
                SelectNextUp();
            }
            else if (GetAutoCancel() && input->down)
            {
                SetHandlesInputs(false);
                SelectNextDown();
            }
            goNext = input->right;
            goPrev = input->left;
            break;
        case UIBaseList::Navigation::VERTICAL:
            if (GetAutoCancel() && input->left)
            {
                SetHandlesInputs(false);
                SelectNextLeft();
            }
            else if (GetAutoCancel() && input->right)
            {
                SetHandlesInputs(false);
                SelectNextRight();
            }
            goNext = input->up;
            goPrev = input->down;
            break;
        default:
        case UIBaseList::Navigation::ALL:
            goNext = input->right || input->up;
            goPrev = input->left || input->down;
            break;
        }

        if (goNext)
        {
            if (m_isCycle)
            {
                int prevItemIdx = m_itemIdx;
                m_itemIdx = (m_itemIdx + 1) % m_itemCount;
                OnItemChanged(m_itemIdx, prevItemIdx, true);
            }
            else if (m_itemIdx < m_itemCount - 1)
            {
                int prevItemIdx = m_itemIdx;
                m_itemIdx++;
                OnItemChanged(m_itemIdx, prevItemIdx, true);
            }
        }
        if (goPrev)
        {
            if (m_isCycle)
            {
                int prevItemIdx = m_itemIdx;
                m_itemIdx = (m_itemIdx + m_itemCount - 1) % m_itemCount;
                OnItemChanged(m_itemIdx, prevItemIdx, false);
            }
            else if (m_itemIdx > 0)
            {
                int prevItemIdx = m_itemIdx;
                m_itemIdx--;
                OnItemChanged(m_itemIdx, prevItemIdx, false);
            }
        }
    }
}

void UIBaseList::OnStateChanged(State state, State prevState, bool makeSound)
{
    UISelectable::OnStateChanged(state, prevState, makeSound);

    if (state == State::PRESSED)
    {
        SetHandlesInputs(true);
    }
    else if (HandlesInputs())
    {
        SetHandlesInputs(false);
    }
}

void UIBaseList::OnItemChanged(int itemIdx, int prevItemIdx, bool increase)
{
    for (auto listener : m_selectableListeners)
    {
        listener->OnItemChanged(this, itemIdx, prevItemIdx, increase);
    }

    if (m_soundItem >= 0)
    {
        AssetManager *assets = m_scene->GetAssetManager();
        assets->PlaySoundFX(m_soundItem);
    }
}

void UIBaseList::OnSetFirstItem(int itemIdx, int prevItemIdx)
{
}

void UIBaseList::SetSelectedItem(int itemIndex)
{
    if (itemIndex < 0 || itemIndex >= m_itemCount) return;
    if (itemIndex == m_itemIdx) return;

    int prevItemIdx = m_itemIdx;
    m_itemIdx = itemIndex;
    OnItemChanged(m_itemIdx, prevItemIdx, false);
}

void UIBaseList::SetFirstSelectedItem(int itemIndex)
{
    if (itemIndex < 0 || itemIndex >= m_itemCount) return;
    if (itemIndex == m_itemIdx) return;

    int prevItemIdx = m_itemIdx;
    m_itemIdx = itemIndex;
    OnSetFirstItem(m_itemIdx, prevItemIdx);
}
