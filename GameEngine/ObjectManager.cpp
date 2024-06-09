/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "ObjectManager.h"
#include "GameObject.h"
#include "GameBody.h"
#include "UIObject.h"

//#define DEBUG_OBJECT_ON_DELETE

ObjectManager::ObjectManager() :
    m_objects(), m_toProcess(), m_visibleObjects(), m_nextID(0)
{
}

ObjectManager::~ObjectManager()
{
    assert(m_objects.size() == 0);
}

void ObjectManager::AddObject(GameObject *object)
{
    // Teste si l'objet est déjà présent
    if (Contains(object)) return;

    // Teste si l'objet est en attente de Start()
    if (m_toProcess.find(object) != m_toProcess.end()) return;

    object->AddFlags(GameObject::Flag::TO_START);
    object->m_objectID = m_nextID++;

    m_toProcess.insert(object);
}

void ObjectManager::DeleteObject(GameObject *object)
{
    m_toProcess.insert(object);

    object->AddFlags(GameObject::Flag::TO_DELETE);

    for (GameObject *child : object->GetChildren())
    {
        DeleteObject(child);
    }
}

void ObjectManager::SetEnabled(GameObject *object, bool enabled)
{
    if (enabled)
    {
        m_toProcess.insert(object);
        object->SubFlags(GameObject::Flag::TO_DISABLE);
        object->AddFlags(GameObject::Flag::TO_ENABLE);
    }
    else
    {
        m_toProcess.insert(object);
        object->SubFlags(GameObject::Flag::TO_ENABLE);
        object->AddFlags(GameObject::Flag::TO_DISABLE);
    }
}

void ObjectManager::SetVisible(GameObject *object, bool visible)
{
    if (visible)
        m_visibleSet.insert(object);
    else
        m_visibleSet.erase(object);
}

void ObjectManager::DeleteObjects()
{
    ProcessObjects();
    for (GameObject *gameObject : m_objects)
    {
        delete gameObject;
    }
    m_objects.clear();
    m_toProcess.clear();
    m_visibleObjects.clear();
    m_visibleSet.clear();
}

bool ObjectManager::Contains(GameObject *object) const
{
    return m_objects.find(object) != m_objects.end();
}

void ObjectManager::ClearVisibleObjects()
{
    m_visibleObjects.clear();
}


bool CompareDepth(const GameObject *objectA, const GameObject *objectB)
{
    if (objectA->GetDepth() == objectB->GetDepth())
    {
        return objectA->GetID() < objectB->GetID();
    }
    return objectA->GetDepth() < objectB->GetDepth();
}

void ObjectManager::ProcessObjects()
{
    std::set<GameObject *> toDelete;
    std::vector<GameObject *> toProcessCopy(m_toProcess.begin(), m_toProcess.end());

    // Remet à zéro le conteneur
    m_toProcess.clear();
    m_visibleSet.clear();

    // Trie les objets selon leurs profondeurs
    std::sort(toProcessCopy.begin(), toProcessCopy.end(), CompareDepth);

    for (auto &gameObject : toProcessCopy)
    {
        // START
        if (gameObject->TestFlag(GameObject::Flag::TO_START))
        {
            // Insertion dans la liste des objets
            m_objects.insert(gameObject);

            gameObject->SubFlags(GameObject::Flag::TO_START);
            gameObject->Start();

            gameObject->SubFlags(GameObject::Flag::TO_ENABLE);
            gameObject->m_enabled = true;
            gameObject->OnEnable();
        }

        // ENABLE
        if (gameObject->TestFlag(GameObject::Flag::TO_ENABLE))
        {
            gameObject->SubFlags(GameObject::Flag::TO_ENABLE);
            gameObject->m_enabled = true;
            gameObject->OnEnable();
        }

        // DISABLE
        if (gameObject->TestFlag(GameObject::Flag::TO_DISABLE))
        {
            gameObject->SubFlags(GameObject::Flag::TO_DISABLE);
            gameObject->m_enabled = false;
            gameObject->OnDisable();
        }

        // DELETE
        if (gameObject->TestFlag(GameObject::Flag::TO_DELETE))
        {
            toDelete.insert(gameObject);
        }

        // DEBUG
    #ifdef DEBUG_OBJECT_ON_DELETE
        if (gameObject->TestFlag(GameObject::Flag::DEBUG_DELETED))
        {
            assert(false);
        }
    #endif
    }

    for (auto gameObject : toDelete)
    {
        gameObject->OnDelete();
    }
    for (auto gameObject : toDelete)
    {
        gameObject->SetParent(nullptr);
    }
    for (auto &gameObject : toDelete)
    {
        // On détruit toutes les références de l'objet
        m_toProcess.erase(gameObject);
        m_objects.erase(gameObject);
        m_visibleSet.erase(gameObject);

    #ifdef DEBUG_OBJECT_ON_DELETE
        gameObject->AddFlags(GameObject::Flag::DEBUG_DELETED);
    #else
        delete gameObject;
    #endif
    }
    m_visibleObjects.clear();
}

void ObjectManager::PrintObjects() const
{
    // Trie les objets hierarchiquement
    std::cout << "Scene Game Objects:" << std::endl;
    for (GameObject *gameObject : m_objects)
    {
        if (gameObject->GetParent() != nullptr) continue;
        PrintObjectsRec(gameObject);
    }
}

void ObjectManager::PrintObjectsRec(GameObject *gameObject) const
{
    for (int i = 0; i < gameObject->GetDepth(); i++)
        std::cout << std::string("    ");

    std::cout << "[" << gameObject->GetDepth() << "] ";

    if (gameObject->IsEnabled() == false)
        std::cout << "[DISABLED] ";

    UIObject *uiObject = dynamic_cast<UIObject *>(gameObject);
    if (uiObject && uiObject->IsUIEnabled() == false)
    {
        std::cout << "[UI DISABLED] ";
    }

    std::cout << gameObject->GetName() << " "
        << "(ID=" << gameObject->m_objectID << ")"
        << std::endl;

    for (GameObject *child : gameObject->GetChildren())
    {
        PrintObjectsRec(child);
    }
}

void ObjectManager::ValidateWorldView(const b2AABB &worldView) const
{
    const float maxValue = 10000.f;
    const float maxSize = 100.f;
    assert(isnan(worldView.lowerBound.x) == false);
    assert(isnan(worldView.lowerBound.y) == false);
    assert(isnan(worldView.upperBound.x) == false);
    assert(isnan(worldView.upperBound.y) == false);
    assert(-maxValue < worldView.lowerBound.x);
    assert(-maxValue < worldView.lowerBound.y);
    assert(-maxValue < worldView.upperBound.x);
    assert(-maxValue < worldView.upperBound.y);
    assert(worldView.lowerBound.x < maxValue);
    assert(worldView.lowerBound.y < maxValue);
    assert(worldView.upperBound.x < maxValue);
    assert(worldView.upperBound.y < maxValue);
    assert(worldView.upperBound.x - worldView.lowerBound.x < maxSize);
    assert(worldView.upperBound.y - worldView.lowerBound.y < maxSize);
}

class VisibleQueryCallback : public b2QueryCallback
{
public:
    VisibleQueryCallback(ObjectManager &objectManager) :
        m_objManager(objectManager)
    {
    }

    bool ReportFixture(b2Fixture *fixture)
    {
        const b2Body *body = fixture->GetBody();
        const b2BodyUserData &userData = body->GetUserData();
        GameBody *object = (GameBody *)userData.pointer;

        // Ajoute l'objet aux objects visibles
        m_objManager.SetVisible(object, true);

        // On continue la recherche
        return true;
    }

private:
    ObjectManager &m_objManager;
};

void ObjectManager::AddVisibleBodies(b2World &world, const b2AABB &worldView)
{
    ValidateWorldView(worldView);

    // Ajoute chaque GameBody visible par la caméra
    VisibleQueryCallback callback(*this);
    world.QueryAABB(&callback, worldView);
}

bool CompareLayers(const GameObject *objectA, const GameObject *objectB)
{
    if (objectA->GetLayer() == objectB->GetLayer())
    {
        if (objectA->GetDepth() == objectB->GetDepth())
        {
            return objectA->GetID() < objectB->GetID();
        }
        return objectA->GetDepth() < objectB->GetDepth();
    }
    return objectA->GetLayer() < objectB->GetLayer();
}

void ObjectManager::ProcessVisibleObjects()
{
    m_visibleObjects.clear();
    for (GameObject *gameObject : m_visibleSet)
    {
        m_visibleObjects.push_back(gameObject);
    }
    
    // Trie les objets selon leurs layers
    std::sort(m_visibleObjects.begin(), m_visibleObjects.end(), CompareLayers);
}
