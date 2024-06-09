/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "Settings.h"
#include "UIObject.h"
#include "UIInput.h"

class UISelectable;
class UISelectableListener;

class UISelectableGroup : public GameObject
{
public:
    UISelectableGroup(Scene *scene);
    virtual ~UISelectableGroup();

    bool IsInteractable() const;
    void SetInteractable(bool interactable);

    void AddInput(const UIInput *input);
    void RemoveInput(const UIInput *input);
    void RemoveAllInputs();
    const std::set<const UIInput *> *GetInputs() const;

    UISelectable *GetSelected();
    void SetSelected(UISelectable *selectable, bool makeSound = false);

    void SetNavigationEnabled(bool navigationEnabled);
    bool IsNavigationEnabled() const;

    void ClearSelectableNavigation();

    bool IsCanceled() const;

    bool ContainsSelectable(UISelectable *selectable);
    void AddSelectable(UISelectable *selectable);
    void RemoveSelectable(UISelectable *selectable);
    void RemoveAllSelectables();
    void RemoveAllUnselected();
    void ComputeAutoNavigation();

    void SetCursor(UIObject *object);
    void SetCursorOnSelected();
    UIObject *GetCursor();

    virtual void Update() override;
    virtual void OnDelete() override;

protected:
    friend class UISelectable;

    std::set<UISelectable *> m_selectableSet;
    std::set<const UIInput *> m_inputs;

private:
    void SetCursorTarget();
    void SelectEnabledSelectable();

    UIObject *m_cursor;
    bool m_interactable;
    bool m_navigationEnabled;
    Uint64 m_updateID;

    bool m_canceled;
    UISelectable *m_selected;
};

inline void UISelectableGroup::SetCursor(UIObject *object)
{
    m_cursor = object;
    m_cursor->SetParent(this);
}

inline UIObject *UISelectableGroup::GetCursor()
{
    return m_cursor;
}
