/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "GameObject.h"
#include "Scene.h"
#include "ObjectManager.h"


GameObject::GameObject(Scene *scene, int layer) :
    m_scene(scene), m_enabled(false), m_layer(layer),
    m_children(), m_parent(nullptr), m_depth(0), m_name(), m_flags(Flag::NONE),
    m_objectID(-1), m_delays(), m_fixedDelays()
{
    SetName("GameObject");
    scene->m_objectManager.AddObject(this);
}

void GameObject::AddChild(GameObject *child)
{
    assert(child);

    m_children.insert(child);
    child->m_parent = this;
    child->UpdateDepth();
}

void GameObject::RemoveChild(GameObject *child)
{
    assert(child);

    auto it = m_children.find(child);
    if (it == m_children.end())
    {
        assert(false);
        return;
    }

    m_children.erase(child);
    child->m_parent = nullptr;
    child->UpdateDepth();
}

void GameObject::UpdateDepth()
{
    m_depth = (m_parent == nullptr) ? 0 : m_parent->m_depth + 1;
    for (GameObject *child : m_children)
    {
        child->UpdateDepth();
    }
}

void GameObject::SetParent(GameObject *parent)
{
    if (parent == this || m_parent == parent) return;

    if (m_parent) m_parent->RemoveChild(this);
    if (parent) parent->AddChild(this);
    
    m_parent = parent;
}

GameObject::~GameObject()
{
    // C'est la scene qui s'occupe de supprimer les objets fils
}

void GameObject::DrawGizmos(Gizmos &gizmos)
{
}

void GameObject::FixedUpdate()
{
    for (float *delay : m_fixedDelays)
    {
        *delay -= m_scene->GetDelta();
        if (*delay < 0.f) *delay = -1.f;
    }
}

void GameObject::OnDisable()
{
}

void GameObject::OnEnable()
{
}

void GameObject::Render()
{
}

void GameObject::Start()
{
}

void GameObject::Update()
{
    for (auto it = m_delays.begin(); it != m_delays.end(); ++it)
    {
        float *delay = it->first;
        *delay -= (it->second) ? m_scene->GetDelta() : m_scene->GetTime().GetUnscaledDelta();
        if (*delay < 0.f) *delay = -1.f;
    }
}

void GameObject::OnDelete()
{
}
