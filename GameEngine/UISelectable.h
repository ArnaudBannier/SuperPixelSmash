/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "Settings.h"
#include "UIObject.h"
#include "UIInput.h"
#include "UISelectableGroup.h"

class UISelectable : public UIObject
{
public:
    UISelectable(Scene *scene, UISelectableGroup *group = nullptr);
    virtual ~UISelectable();

    UISelectableGroup *GetGroup();
    const std::set<const UIInput *> *GetInputs() const;

    void SetLocalNavigationRect(const UIRect &rect);
    UIRect &GetLocalNavigationRect();
    SDL_FRect GetCanvasNavigationRect() const;

    void SetLocalCursorRect(const UIRect &rect);
    UIRect &GetLocalCursorRect();
    SDL_FRect GetCanvasCursorRect() const;

    void SetOnUp(UISelectable *next);
    void SetOnDown(UISelectable *next);
    void SetOnLeft(UISelectable *next);
    void SetOnRight(UISelectable *next);

    UISelectable *GetOnUp();
    UISelectable *GetOnDown();
    UISelectable *GetOnLeft();
    UISelectable *GetOnRight();

    void SetSoundOnSelected(int soundID);
    void SetSoundOnPressed(int soundID);
    void SetSoundOnClick(int soundID);

    enum class State : int
    {
        NORMAL = 0, SELECTED, PRESSED, DISABLED
    };

    UISelectable::State GetState() const;

    void AddSelectableListener(UISelectableListener *listener);
    void RemoveSelectableListener(UISelectableListener *listener);

    virtual void OnDelete() override;
    virtual void DrawGizmos(Gizmos &gizmos);

    void SetClickedOnRelease(bool clickedOnRelease);
    bool IsClickedOnRelease() const;
    bool HandlesInputs() const;

protected:
    virtual void OnStateChanged(State state, State prevState, bool makeSound);
    virtual void OnClick();
    void SetHandlesInputs(bool handlesInputs);
    void SetState(State state, bool makeSound = false);
    void SelectNextUp();
    void SelectNextDown();
    void SelectNextLeft();
    void SelectNextRight();
    std::set<UISelectableListener *> m_selectableListeners;

private:
    friend class UISelectableGroup;

    bool m_clickedOnRelease;
    bool m_handlesInputs;
    UIRect m_autoNavRect;
    UIRect m_cursorRect;
    State m_state;
    Uint64 m_updateID;

    UISelectable *m_nextUp;
    UISelectable *m_nextDown;
    UISelectable *m_nextLeft;
    UISelectable *m_nextRight;

    UISelectableGroup *m_group;

    int m_soundSelected;
    int m_soundPressed;
    int m_soundClick;
};

class UIStateModifiers
{
public:
    UIStateModifiers() : alphaMap(), colorMap() {}

    std::map<UISelectable::State, float> alphaMap;
    std::map<UISelectable::State, Color> colorMap;

    void SetObjectTarget(UIObject *object, UISelectable::State state) const;
};

class UISelectableListener
{
public:
    virtual ~UISelectableListener() {}
    virtual void OnStateChanged(UISelectable *which, UISelectable::State state, UISelectable::State prevState) {}
    virtual void OnClick(UISelectable *which) {}
    virtual void OnItemChanged(UISelectable *which, int itemIdx, int prevItemIdx, bool increase) {}
};

inline UISelectableGroup *UISelectable::GetGroup()
{
    return m_group;
}

inline void UISelectable::SetLocalNavigationRect(const UIRect &rect)
{
    m_autoNavRect = rect;
}

inline UIRect &UISelectable::GetLocalNavigationRect()
{
    return m_autoNavRect;
}

inline void UISelectable::SetLocalCursorRect(const UIRect &rect)
{
    m_cursorRect = rect;
}

inline UIRect &UISelectable::GetLocalCursorRect()
{
    return m_cursorRect;
}

inline UISelectable::State UISelectable::GetState() const
{
    return m_state;
}

inline void UISelectable::AddSelectableListener(UISelectableListener *listener)
{
    m_selectableListeners.insert(listener);
}

inline void UISelectable::RemoveSelectableListener(UISelectableListener *listener)
{
    m_selectableListeners.erase(listener);
}

inline void UISelectable::SetClickedOnRelease(bool clickedOnRelease)
{
    m_clickedOnRelease = clickedOnRelease;
}

inline bool UISelectable::IsClickedOnRelease() const
{
    return m_clickedOnRelease;
}

inline bool UISelectable::HandlesInputs() const
{
    return m_handlesInputs;
}

inline void UISelectable::SetSoundOnSelected(int soundID)
{
    m_soundSelected = soundID;
}

inline void UISelectable::SetSoundOnPressed(int soundID)
{
    m_soundPressed = soundID;
}

inline void UISelectable::SetSoundOnClick(int soundID)
{
    m_soundClick = soundID;
}
