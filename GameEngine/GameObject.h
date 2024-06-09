/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "Settings.h"
#include "Common.h"
#include "Gizmos.h"

class Scene;

class GameObject
{
public:
    GameObject(Scene *scene, int layer);
    GameObject(GameObject const&) = delete;
    GameObject& operator=(GameObject const&) = delete;
    virtual ~GameObject();

    virtual void DrawGizmos(Gizmos &gizmos);
    virtual void FixedUpdate();
    virtual void OnDisable();
    virtual void OnEnable();
    virtual void Render();
    virtual void Start();
    virtual void Update();
    virtual void OnDelete();

    bool IsEnabled();
    int GetLayer() const;
    void SetLayer(int layer);
    void SetVisible(bool isVisible, bool applyToChildren = false);
    void SetEnabled(bool isEnabled, bool applyToChildren = false);
    void Delete();

    void SetParent(GameObject *parent);
    GameObject *GetParent() const;

    void SetName(const std::string &name);
    std::string &GetName();

    int GetDepth() const;
    int GetID() const;

    const std::set<GameObject *> &GetChildren();

    void AddUpdateDelay(float *delay, bool useTimeScale = true);
    void RemoveUpdateDelay(float *delay);

    void AddFixedUpdateDelay(float *delay);
    void RemoveFixedUpdateDelay(float *delay);

protected:
    Scene *m_scene;
    std::string m_name;

private:
    friend class ObjectManager;

    void AddChild(GameObject *child);
    void RemoveChild(GameObject *child);
    void UpdateDepth();

    bool m_enabled;
    int m_layer;
    int m_depth;
    int m_objectID;

    enum class Flag : uint32_t
    {
        NONE       = 0x00,
        TO_START   = 0x01,
        TO_ENABLE  = 0x02,
        TO_DISABLE = 0x04,
        TO_DELETE  = 0x08,
        TO_PROCESS = 0x10,
        DEBUG_DELETED = 0x80000000,
    };
    Flag m_flags;

    void AddFlags(GameObject::Flag flags);
    void SubFlags(GameObject::Flag flags);
    bool TestFlag(GameObject::Flag flag) const;

    GameObject *m_parent;
    std::set<GameObject *> m_children;
    std::set<float *> m_fixedDelays;
    std::map<float *, bool> m_delays;
};

#include "Scene.h"

constexpr enum GameObject::Flag operator |(
    const enum GameObject::Flag selfValue, const enum GameObject::Flag inValue)
{
    return (enum GameObject::Flag)(uint32_t(selfValue) | uint32_t(inValue));
}

constexpr enum GameObject::Flag operator &(
    const enum GameObject::Flag selfValue, const enum GameObject::Flag inValue)
{
    return (enum GameObject::Flag)(uint32_t(selfValue) & uint32_t(inValue));
}

constexpr enum GameObject::Flag operator ~(
    const enum GameObject::Flag selfValue)
{
    return (enum GameObject::Flag)(~uint32_t(selfValue));
}

inline bool GameObject::IsEnabled()
{
    return m_enabled;
}

inline int GameObject::GetLayer() const
{
    return m_layer;
}

inline void GameObject::SetLayer(int layer)
{
    m_layer = layer;
}

inline void GameObject::SetVisible(bool isVisible, bool applyToChildren)
{
    m_scene->m_objectManager.SetVisible(this, isVisible);

    if (applyToChildren == false) return;
    for (auto child : m_children)
    {
        child->SetVisible(isVisible, true);
    }
}

inline void GameObject::SetEnabled(bool isEnabled, bool applyToChildren)
{
    m_scene->m_objectManager.SetEnabled(this, isEnabled);
    if (applyToChildren == false) return;
    for (auto child : m_children)
    {
        child->SetEnabled(isEnabled, true);
    }
}

inline void GameObject::Delete()
{
    m_scene->m_objectManager.DeleteObject(this);
}

inline GameObject *GameObject::GetParent() const
{
    return m_parent;
}

inline void GameObject::SetName(const std::string &name)
{
    m_name = name;
}

inline std::string &GameObject::GetName()
{
    return m_name;
}

inline int GameObject::GetDepth() const
{
    return m_depth;
}

inline int GameObject::GetID() const
{
    return m_objectID;
}

inline const std::set<GameObject *> &GameObject::GetChildren()
{
    return m_children;
}

inline void GameObject::AddUpdateDelay(float *delay, bool useTimeScale)
{
    m_delays.insert(std::make_pair(delay, useTimeScale));
}

inline void GameObject::RemoveUpdateDelay(float *delay)
{
    m_delays.erase(delay);
}

inline void GameObject::AddFixedUpdateDelay(float *delay)
{
    m_fixedDelays.insert(delay);
}

inline void GameObject::RemoveFixedUpdateDelay(float *delay)
{
    m_fixedDelays.erase(delay);
}

inline void GameObject::AddFlags(GameObject::Flag flags)
{
    m_flags = m_flags | flags;
}

inline void GameObject::SubFlags(GameObject::Flag flags)
{
    m_flags = m_flags & (~flags);
}

inline bool GameObject::TestFlag(GameObject::Flag flag) const
{
    return (flag == (m_flags & flag));
}
