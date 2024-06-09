/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "Settings.h"
#include "UISelectable.h"

class UIBaseList : public UISelectable
{
public:
    UIBaseList(Scene *scene, int itemCount, UISelectableGroup *group = nullptr);
    virtual ~UIBaseList();

    void SetIsCycle(bool isCycle);

    virtual void Update() override;

    void SetSelectedItem(int itemIndex);
    void SetFirstSelectedItem(int itemIndex);
    int GetSelectedItem() const;

    void SetAutoCancel(bool autoCancel);
    bool GetAutoCancel() const;

    void SetSoundOnItemChanged(int soundID);

    enum class Navigation
    {
        HORIZONTAL, VERTICAL, ALL
    };
    void SetListNavigation(Navigation navigation);
    Navigation GetListNavigation() const;

protected:
    virtual void OnStateChanged(State state, State prevState, bool makeSound) override;
    virtual void OnItemChanged(int itemIdx, int prevItemIdx, bool increase);
    virtual void OnSetFirstItem(int itemIdx, int prevItemIdx);

    int m_itemIdx;
    int m_itemCount;
    bool m_isCycle;

private:
    bool m_autoCancel;
    Navigation m_listNavigation;

    int m_soundItem;
};

inline void UIBaseList::SetIsCycle(bool isCycle)
{
    m_isCycle = isCycle;
}

inline void UIBaseList::SetListNavigation(Navigation navigation)
{
    m_listNavigation = navigation;
}

inline UIBaseList::Navigation UIBaseList::GetListNavigation() const
{
    return m_listNavigation;
}

inline int UIBaseList::GetSelectedItem() const
{
    return m_itemIdx;
}

inline bool UIBaseList::GetAutoCancel() const
{
    return m_autoCancel;
}

inline void UIBaseList::SetAutoCancel(bool autoCancel)
{
    m_autoCancel = autoCancel;
}

inline void UIBaseList::SetSoundOnItemChanged(int soundID)
{
    m_soundItem = soundID;
}

