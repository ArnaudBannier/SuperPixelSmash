/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "UIButton.h"
#include "Scene.h"
#include "Camera.h"
#include "UICanvas.h"

UIButton::UIButton(Scene *scene, UISelectableGroup *group):
    UISelectable(scene, group), m_stateObjectMaps()
{
    SetName("UIButton");
    SetFadeChildren(false);
}

UIButton::~UIButton()
{
}

void UIButton::AddLabelObject(UIObject *object)
{
    Label label;
    label.object = object;

    AddLabelObject(object, label);
}

void UIButton::AddLabelObject(UIObject *object, const UIStateModifiers &stateModifiers)
{
    Label label;
    label.object = object;
    label.stateModifiers = stateModifiers;

    AddLabelObject(object, label);
}

void UIButton::RemoveLabelObject(UIObject *object)
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

void UIButton::AddStateObject(State state, UIObject *object, bool fade)
{
    object->SetParent(this);
    object->SetUseParentAnimation(true);
    object->SetUseParentAlpha(true);
    object->SetUIEnabled(state == GetState());

    int stateIdx = (int)state;

    auto it = m_stateObjectMaps[stateIdx].find(object);
    if (it != m_stateObjectMaps[stateIdx].end())
    {
        it->second = fade;
    }
    else
    {
        m_stateObjectMaps[stateIdx].insert(std::make_pair(object, fade));
    }

    if (fade)
    {
        UIFadeDef fadeDef;
        fadeDef.fadeOpacity = true;
        fadeDef.duration = 0.1f;
        object->SetFadeInAnimation(fadeDef);
        object->SetFadeOutAnimation(fadeDef);
    }
}

void UIButton::RemoveStateObject(State state, UIObject *object)
{
    int stateIdx = (int)state;
    auto it = m_stateObjectMaps[stateIdx].find(object);
    if (it == m_stateObjectMaps[stateIdx].end())
    {
        return;
    }

    object->SetParent(m_scene->GetCanvas());
    object->SetUseParentAnimation(false);

    m_stateObjectMaps[stateIdx].erase(it);
}

void UIButton::Update()
{
    if (IsUIEnabled() == false) return;

    UISelectable::Update();
    SetVisible(true);
}

void UIButton::OnStateChanged(State state, State prevState, bool makeSound)
{
    UISelectable::OnStateChanged(state, prevState, makeSound);

    for (auto &it : m_labelMap)
    {
        UIObject *object = it.first;
        Label &label = it.second;

        label.stateModifiers.SetObjectTarget(object, state);
        object->PlayTransformToTarget();
    }

    std::map<UIObject *, bool> *m_stateMap = &m_stateObjectMaps[(int)state];
    for (auto it = m_stateMap->begin(); it != m_stateMap->end(); ++it)
    {
        UIObject *object = it->first;
        bool fade = it->second;

        if (fade)
        {
            object->PlayFadeIn();
        }
        else
        {
            object->SetUIEnabled(true);
        }
    }

    m_stateMap = &m_stateObjectMaps[(int)prevState];
    for (auto it = m_stateMap->begin(); it != m_stateMap->end(); ++it)
    {
        UIObject *object = it->first;
        bool fade = it->second;

        if (fade)
        {
            object->PlayFadeOut();
        }
        else
        {
            object->SetUIEnabled(false);
        }
    }
}

void UIButton::AddLabelObject(UIObject *object, Label &label)
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
