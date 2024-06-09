/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "UISelectable.h"
#include "UICanvas.h"
#include "Utils.h"

UISelectable::UISelectable(Scene *scene, UISelectableGroup *group) :
    UIObject(scene), m_group(group), m_state(State::NORMAL),
    m_nextUp(nullptr), m_nextDown(nullptr), m_nextLeft(nullptr), m_nextRight(nullptr),
    m_updateID(scene->GetUpdateID()),
    m_clickedOnRelease(true), m_handlesInputs(false),
    m_soundClick(-1), m_soundPressed(-1), m_soundSelected(-1)
{
    SetName("UISelectable");
    if (m_group) m_group->AddSelectable(this);
}

UISelectable::~UISelectable()
{
}


const std::set<const UIInput *> *UISelectable::GetInputs() const
{
    if (m_group == nullptr) return nullptr;
    return m_group->GetInputs();
}

void UISelectable::SetOnUp(UISelectable *next)
{
    if (m_group == nullptr || m_group->ContainsSelectable(next) == false)
    {
        assert(false);
        return;
    }

    m_nextUp = next;
}

void UISelectable::SetOnDown(UISelectable *next)
{
    if (m_group == nullptr || m_group->ContainsSelectable(next) == false)
    {
        assert(false);
        return;
    }

    m_nextDown = next;
}

void UISelectable::SetOnLeft(UISelectable *next)
{
    if (m_group == nullptr || m_group->ContainsSelectable(next) == false)
    {
        assert(false);
        return;
    }

    m_nextLeft = next;
}

void UISelectable::SetOnRight(UISelectable *next)
{
    if (m_group == nullptr || m_group->ContainsSelectable(next) == false)
    {
        assert(false);
        return;
    }

    m_nextRight = next;
}

UISelectable *UISelectable::GetOnUp()
{
    UISelectable *next = m_nextUp;
    while (next != nullptr && next->IsUIEnabled() == false)
    {
        next = next->m_nextUp;
    }
    return next;
}

UISelectable *UISelectable::GetOnDown()
{
    UISelectable *next = m_nextDown;
    while (next != nullptr && next->IsUIEnabled() == false)
    {
        next = next->m_nextDown;
    }
    return next;
}

UISelectable *UISelectable::GetOnLeft()
{
    UISelectable *next = m_nextLeft;
    while (next != nullptr && next->IsUIEnabled() == false)
    {
        next = next->m_nextLeft;
    }
    return next;
}

UISelectable *UISelectable::GetOnRight()
{
    UISelectable *next = m_nextRight;
    while (next != nullptr && next->IsUIEnabled() == false)
    {
        next = next->m_nextRight;
    }
    return next;
}

void UISelectable::OnDelete()
{
    if (m_group) m_group->RemoveSelectable(this);
}

void UISelectable::DrawGizmos(Gizmos &gizmos)
{
    SDL_FRect navRect = GetCanvasNavigationRect();
    SDL_SetRenderDrawColor(g_renderer, 255, 0, 0, 50);
    SDL_RenderFillRectF(g_renderer, &navRect);

    SDL_FRect rect = GetCanvasRect();
    SDL_SetRenderDrawColor(g_renderer, 255, 255, 0, 50);
    SDL_RenderDrawRectF(g_renderer, &rect);

    if (GetGroup() && GetGroup()->GetSelected() == this)
    {
        SDL_SetRenderDrawColor(g_renderer, 255, 255, 255, 255);
        if (m_nextUp)
        {
            SDL_FRect nextRect = m_nextUp->GetCanvasRect();
            SDL_RenderDrawLineF(
                g_renderer, rect.x + 0.5f * rect.w, rect.y,
                nextRect.x + 0.5f * nextRect.w, nextRect.y + 0.5f * nextRect.h
            );
        }
        if (m_nextDown)
        {
            SDL_FRect nextRect = m_nextDown->GetCanvasRect();
            SDL_RenderDrawLineF(
                g_renderer, rect.x + 0.5f * rect.w, rect.y + rect.h,
                nextRect.x + 0.5f * nextRect.w, nextRect.y + 0.5f * nextRect.h
            );
        }
        if (m_nextRight)
        {
            SDL_FRect nextRect = m_nextRight->GetCanvasRect();
            SDL_RenderDrawLineF(
                g_renderer, rect.x + rect.w, rect.y + 0.5f * rect.h,
                nextRect.x + 0.5f * nextRect.w, nextRect.y + 0.5f * nextRect.h
            );
        }
        if (m_nextLeft)
        {
            SDL_FRect nextRect = m_nextLeft->GetCanvasRect();
            SDL_RenderDrawLineF(
                g_renderer, rect.x, rect.y + 0.5f * rect.h,
                nextRect.x + 0.5f * nextRect.w, nextRect.y + 0.5f * nextRect.h
            );
        }
    }

    if (m_group != nullptr && m_group->GetCursor() != nullptr)
    {
        SDL_FRect cursorRect = GetCanvasCursorRect();
        SDL_SetRenderDrawColor(g_renderer, 0, 255, 255, 50);
        SDL_RenderFillRectF(g_renderer, &cursorRect);
    }
}

void UISelectable::OnStateChanged(State state, State prevState, bool makeSound)
{
    if (IsUIEnabled() == false || makeSound == false) return;

    if (state == State::SELECTED && prevState == State::NORMAL && m_soundSelected >= 0)
    {
        AssetManager *assets = m_scene->GetAssetManager();
        assets->PlaySoundFX(m_soundSelected);
    }
    else if (state == State::PRESSED && m_soundPressed >= 0)
    {
        AssetManager *assets = m_scene->GetAssetManager();
        assets->PlaySoundFX(m_soundPressed);
    }
}

void UISelectable::OnClick()
{
    for (auto listener : m_selectableListeners)
    {
        listener->OnClick(this);
    }
    
    if (m_soundClick >= 0)
    {
        AssetManager *assets = m_scene->GetAssetManager();
        assets->PlaySoundFX(m_soundClick);
    }
}

void UISelectable::SetHandlesInputs(bool handlesInputs)
{
    if (m_updateID == m_scene->GetUpdateID()) return;

    UISelectableGroup *group = GetGroup();
    if (group == nullptr) return;

    group->SetNavigationEnabled(!handlesInputs);
    m_updateID = m_scene->GetUpdateID();
    m_handlesInputs = handlesInputs;

    if (handlesInputs == false) SetState(State::SELECTED);
}

void UISelectable::SetState(State state, bool makeSound)
{
    State prevState = m_state;
    if (prevState == state) return;

    m_state = state;
    for (auto listener : m_selectableListeners)
    {
        listener->OnStateChanged(this, state, prevState);
    }
    OnStateChanged(state, prevState, makeSound);
}

void UISelectable::SelectNextUp()
{
    if (m_group == nullptr) return;
    UISelectable *next = GetOnUp();
    if (next) m_group->SetSelected(next);
}

void UISelectable::SelectNextDown()
{
    if (m_group == nullptr) return;
    UISelectable *next = GetOnDown();
    if (next) m_group->SetSelected(next);
}

void UISelectable::SelectNextLeft()
{
    if (m_group == nullptr) return;
    UISelectable *next = GetOnLeft();
    if (next) m_group->SetSelected(next);
}

void UISelectable::SelectNextRight()
{
    if (m_group == nullptr) return;
    UISelectable *next = GetOnRight();
    if (next) m_group->SetSelected(next);
}

SDL_FRect UISelectable::GetCanvasNavigationRect() const
{
    SDL_FRect canvasRect = GetCanvasRect();
    float pixelsPerUnit = m_scene->GetCanvas()->GetPixelsPerUnit();

    b2Vec2 ancMin = m_autoNavRect.anchorMin;
    b2Vec2 ancMax = m_autoNavRect.anchorMax;
    b2Vec2 offMin = m_autoNavRect.offsetMin;
    b2Vec2 offMax = m_autoNavRect.offsetMax;
    float lowerX = canvasRect.x + ancMin.x * canvasRect.w + pixelsPerUnit * offMin.x;
    float upperX = canvasRect.x + ancMax.x * canvasRect.w + pixelsPerUnit * offMax.x;
    float lowerY = canvasRect.y + (1.f - ancMin.y) * canvasRect.h - pixelsPerUnit * offMin.y;
    float upperY = canvasRect.y + (1.f - ancMax.y) * canvasRect.h - pixelsPerUnit * offMax.y;

    return SDL_FRect{
        lowerX,
        upperY,
        fabsf(upperX - lowerX),
        fabsf(upperY - lowerY)
    };
}

SDL_FRect UISelectable::GetCanvasCursorRect() const
{
    SDL_FRect canvasRect = GetCanvasRect();
    float pixelsPerUnit = m_scene->GetCanvas()->GetPixelsPerUnit();

    b2Vec2 ancMin = m_cursorRect.anchorMin;
    b2Vec2 ancMax = m_cursorRect.anchorMax;
    b2Vec2 offMin = m_cursorRect.offsetMin;
    b2Vec2 offMax = m_cursorRect.offsetMax;
    float lowerX = canvasRect.x + ancMin.x * canvasRect.w + pixelsPerUnit * offMin.x;
    float upperX = canvasRect.x + ancMax.x * canvasRect.w + pixelsPerUnit * offMax.x;
    float lowerY = canvasRect.y + (1.f - ancMin.y) * canvasRect.h - pixelsPerUnit * offMin.y;
    float upperY = canvasRect.y + (1.f - ancMax.y) * canvasRect.h - pixelsPerUnit * offMax.y;

    return SDL_FRect{
        lowerX,
        upperY,
        fabsf(upperX - lowerX),
        fabsf(upperY - lowerY)
    };
}

void UIStateModifiers::SetObjectTarget(UIObject *object, UISelectable::State state) const
{
    auto itAlpha = alphaMap.find(state);
    if (itAlpha != alphaMap.end())
    {
        object->SetTargetOpacity(itAlpha->second);
    }
    auto itColor = colorMap.find(state);
    if (itColor != colorMap.end())
    {
        object->SetTargetColor(itColor->second);
    }
}
