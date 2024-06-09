/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "UISelectableGroup.h"
#include "UISelectable.h"
#include "UICanvas.h"
#include "Utils.h"

UISelectableGroup::UISelectableGroup(Scene *scene) :
    GameObject(scene, DEFAULT_UI_LAYER), m_interactable(false), m_navigationEnabled(false),
    m_selected(nullptr), m_updateID(0), m_canceled(false), m_cursor(nullptr)
{
    SetName("UISelectableGroup");
}

UISelectableGroup::~UISelectableGroup()
{
}

bool UISelectableGroup::IsInteractable() const
{
    return m_interactable;
}

void UISelectableGroup::SetInteractable(bool interactable)
{
    if (interactable == m_interactable) return;

    SetNavigationEnabled(interactable);
    m_interactable = interactable;
}

void UISelectableGroup::AddInput(const UIInput *input)
{
    m_inputs.insert(input);
}

void UISelectableGroup::RemoveInput(const UIInput *input)
{
    m_inputs.erase(input);
}

void UISelectableGroup::RemoveAllInputs()
{
    m_inputs.clear();
}

const std::set<const UIInput *> *UISelectableGroup::GetInputs() const
{
    return &m_inputs;
}

UISelectable *UISelectableGroup::GetSelected()
{
    return m_selected;
}

void UISelectableGroup::SetSelected(UISelectable *selectable, bool makeSound)
{
    if (selectable == m_selected) return;

    if (m_selected)
    {
        m_selected->SetState(UISelectable::State::NORMAL, makeSound);
    }
    m_selected = nullptr;

    if (selectable && ContainsSelectable(selectable))
    {
        m_selected = selectable;
        m_selected->SetState(UISelectable::State::SELECTED, makeSound);

        if (m_cursor)
        {
            SetCursorTarget();
            m_cursor->PlayTransformToTarget();
        }
    }
}

void UISelectableGroup::SetCursorTarget()
{
    const float pixelsPerUnit = m_scene->GetCanvas()->GetPixelsPerUnit();
    SDL_FRect canvasRect = m_selected->GetCanvasCursorRect();
    UIRect targetRect;
    targetRect.anchorMin = b2Vec2_zero;
    targetRect.anchorMax = b2Vec2_zero;
    targetRect.offsetMin.x = canvasRect.x / pixelsPerUnit;
    targetRect.offsetMin.y = 360.f - ((canvasRect.y + canvasRect.h) / pixelsPerUnit);
    targetRect.offsetMax.x = (canvasRect.x + canvasRect.w) / pixelsPerUnit;
    targetRect.offsetMax.y = 360.f - (canvasRect.y / pixelsPerUnit);
    m_cursor->SetTargetRect(targetRect);
}

void UISelectableGroup::SetCursorOnSelected()
{
    if (m_cursor == nullptr) return;

    SetCursorTarget();
    m_cursor->TransformToTarget();
}

void UISelectableGroup::SelectEnabledSelectable()
{
    if (m_selected != nullptr && m_selected->IsUIEnabled()) return;

    for (UISelectable *selectable : m_selectableSet)
    {
        if (selectable->IsUIEnabled())
        {
            SetSelected(selectable, false);
            return;
        }
    }
}

void UISelectableGroup::SetNavigationEnabled(bool navigationEnabled)
{
    m_updateID = m_scene->GetUpdateID();
    m_navigationEnabled = navigationEnabled;
}

bool UISelectableGroup::IsNavigationEnabled() const
{
    return m_navigationEnabled;
}

void UISelectableGroup::ClearSelectableNavigation()
{
    for (UISelectable *selectable : m_selectableSet)
    {
        selectable->m_nextUp = nullptr;
        selectable->m_nextDown = nullptr;
        selectable->m_nextLeft = nullptr;
        selectable->m_nextRight = nullptr;
    }
}

bool UISelectableGroup::IsCanceled() const
{
    return m_canceled;
}

void UISelectableGroup::Update()
{
    if (m_updateID == m_scene->GetUpdateID()) return;
    if (m_navigationEnabled == false) return;

    SelectEnabledSelectable();

    for (const UIInput *input : m_inputs)
    {
        if (m_selected && input->up && m_selected->GetOnUp())
        {
            SetSelected(m_selected->GetOnUp(), true);
        }
        if (m_selected && input->down && m_selected->GetOnDown())
        {
            SetSelected(m_selected->GetOnDown(), true);
        }
        if (m_selected && input->left && m_selected->GetOnLeft())
        {
            SetSelected(m_selected->GetOnLeft(), true);
        }
        if (m_selected && input->right && m_selected->GetOnRight())
        {
            SetSelected(m_selected->GetOnRight(), true);
        }

        if (m_selected && input->validatePressed)
        {
            m_selected->SetState(UISelectable::State::PRESSED, true);
            if (m_selected->IsClickedOnRelease() == false)
            {
                m_selected->OnClick();
            }
        }
        if (m_selected && input->validateReleased)
        {
            if (m_selected->IsClickedOnRelease() &&
                m_selected->GetState() == UISelectable::State::PRESSED)
            {
                m_selected->SetState(UISelectable::State::SELECTED, true);
                m_selected->OnClick();
            }
        }
        if (input->cancel)
        {
            m_canceled = true;
        }
    }
}

void UISelectableGroup::OnDelete()
{
    RemoveAllInputs();
    RemoveAllSelectables();
}

bool UISelectableGroup::ContainsSelectable(UISelectable *selectable)
{
    return (m_selectableSet.find(selectable) != m_selectableSet.end());
}

void UISelectableGroup::AddSelectable(UISelectable *selectable)
{
    if (selectable == nullptr) return;
    if (ContainsSelectable(selectable)) return;

    UISelectableGroup *prevGroup = selectable->m_group;
    if (prevGroup != nullptr && prevGroup != this)
    {
        prevGroup->RemoveSelectable(selectable);
    }
    m_selectableSet.insert(selectable);
    selectable->m_group = this;
    selectable->SetState(UISelectable::State::NORMAL);
}

void UISelectableGroup::RemoveSelectable(UISelectable *selectable)
{
    if (selectable == nullptr) return;
    if (ContainsSelectable(selectable) == false) return;

    m_selectableSet.erase(selectable);
    for (UISelectable *other : m_selectableSet)
    {
        if (other->m_nextDown == selectable)
        {
            other->m_nextDown = nullptr;
        }
        if (other->m_nextUp == selectable)
        {
            other->m_nextUp = nullptr;
        }
        if (other->m_nextLeft == selectable)
        {
            other->m_nextLeft = nullptr;
        }
        if (other->m_nextRight == selectable)
        {
            other->m_nextRight = nullptr;
        }
    }
    if (m_selected == selectable)
    {
        SetSelected(nullptr, false);
        SelectEnabledSelectable();
    }
    selectable->m_group = nullptr;
}

void UISelectableGroup::RemoveAllSelectables()
{
    SetSelected(nullptr);
    for (UISelectable *selectable : m_selectableSet)
    {
        selectable->m_nextUp = nullptr;
        selectable->m_nextDown = nullptr;
        selectable->m_nextLeft = nullptr;
        selectable->m_nextRight = nullptr;

        selectable->m_group = nullptr;
    }
    m_selectableSet.clear();
}

void UISelectableGroup::RemoveAllUnselected()
{
    for (UISelectable *selectable : m_selectableSet)
    {
        selectable->m_nextUp = nullptr;
        selectable->m_nextDown = nullptr;
        selectable->m_nextLeft = nullptr;
        selectable->m_nextRight = nullptr;

        if (selectable != m_selected)
        {
            selectable->m_group = nullptr;
        }
    }
    m_selectableSet.clear();
    if (m_selected) m_selectableSet.insert(m_selected);
}

void UISelectableGroup::ComputeAutoNavigation()
{
    std::map<UISelectable*, SDL_FRect> rectMap;

    for (UISelectable *selectable : m_selectableSet)
    {
        rectMap[selectable] = selectable->GetCanvasNavigationRect();
    }

    for (UISelectable *currSelectable : m_selectableSet)
    {
        SDL_FRect currRect = rectMap[currSelectable];

        UISelectable *nextR = nullptr;
        UISelectable *nextL = nullptr;
        UISelectable *nextU = nullptr;
        UISelectable *nextD = nullptr;

        float distR = INFINITY;
        float distL = INFINITY;
        float distU = INFINITY;
        float distD = INFINITY;

        for (UISelectable *nextSelectable : m_selectableSet)
        {
            if (nextSelectable == currSelectable) continue;

            SDL_FRect nextRect = rectMap[nextSelectable];
            b2Vec2 dist = SDL_FRectDistance(currRect, nextRect);
            dist.x = fmaxf(dist.x, 0.f);
            dist.y = fmaxf(dist.y, 0.f);
            float xDist = dist.x + 10.f * dist.y;
            float yDist = dist.y + 10.f * dist.x;

            if ((currRect.x + currRect.w < nextRect.x) && (xDist < distR))
            {   
                distR = xDist; nextR = nextSelectable;
            }
            if ((currRect.x > nextRect.x + nextRect.w) && (xDist < distL))
            {
                distL = xDist; nextL = nextSelectable;
            }
            if ((currRect.y > nextRect.y + nextRect.h) && (yDist < distU))
            {
                distU = yDist; nextU = nextSelectable;
            }
            if ((currRect.y + currRect.h < nextRect.y) && (yDist < distD))
            {
                distD = yDist; nextD = nextSelectable;
            }
        }

        currSelectable->m_nextUp = nextU;
        currSelectable->m_nextDown = nextD;
        currSelectable->m_nextLeft = nextL;
        currSelectable->m_nextRight = nextR;
    }
}
