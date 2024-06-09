/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "Settings.h"
#include "Timer.h"
#include "Common.h"
#include "inputManager.h"
#include "ObjectManager.h"
#include "AssetManager.h"
#include "Gizmos.h"

class SceneManager;
class UICanvas;
class Camera;
class GameBody;
class ParticleSystem;
struct QueryGizmos;

struct SceneStats
{
    /// @brief Nombre d'objets visibles par la caméra active.
    int visibleCount;

    /// @brief Nombre d'objets présents dans la scène.
    int objectCount;

    /// @brief Nombre d'objets inactifs.
    int disabledCount;

    /// @brief Nombre d'objets à réinitialiser quand le joueur meurt.
    int toRespawnCount;

    /// @brief Nombre de corps présents dans le moteur physique.
    int bodyCount;
};

class SceneContactListener : public b2ContactListener
{
public:
    SceneContactListener() {}
    virtual void BeginContact(b2Contact *contact) override;
    virtual void EndContact(b2Contact *contact) override;
    virtual void PreSolve(b2Contact *contact, const b2Manifold *oldManifold) override;
};

class GameCollision
{
public:
    GameCollision(b2Contact *contact, bool first);

    b2WorldManifold manifold;
    b2Fixture *fixture;
    b2Fixture *otherFixture;
    GameBody *gameBody;

    bool IsEnabled() const;
    void SetEnabled(bool enabled);

private:
    b2Contact *m_contact;
    bool m_first;
};

inline bool GameCollision::IsEnabled() const
{
    return m_contact->IsEnabled();
}

inline void GameCollision::SetEnabled(bool enabled)
{
    m_contact->SetEnabled(enabled);
}

class RayHit
{
public:
    RayHit() :
        fixture(nullptr), gameBody(nullptr), fraction(0.f), normal(), hitPoint()
    {}

    b2Fixture *fixture;
    GameBody *gameBody;
    b2Vec2 normal;
    b2Vec2 hitPoint;
    float fraction;
};

struct QueryFilter
{
    QueryFilter() : maskBits((uint16)(-1)), solidOnly(true) {}
    QueryFilter(uint16 mask) : maskBits(mask), solidOnly(true) {}
    uint16 maskBits;
    bool solidOnly;
};

struct OverlapResult
{
    OverlapResult() : fixture(nullptr), gameBody(nullptr) {}

    b2Fixture *fixture;
    GameBody *gameBody;
};

class Scene
{
public:
    Scene(SceneManager *manager, InputManager *inputManager);
    Scene(Scene const&) = delete;
    Scene& operator=(Scene const&) = delete;
    virtual ~Scene();

    void Render();
    void Update();

    SceneManager *GetSceneManager();
    InputManager *GetInputManager();
    AssetManager *GetAssetManager();
    b2World &GetWorld();

    ParticleSystem *GetParticleSystem(int layer);

    bool Contains(GameObject *gameObject) const;

    float GetAlpha();

    Gizmos &GetGizmos();

    void SetActiveCamera(Camera *camera);
    Camera *GetActiveCamera();
    void ShakeCamera(int intensity);

    UICanvas *GetCanvas();
    float GetUIPixelsPerUnit() const;

    const Timer &GetTime() const;
    Timer &GetTime();
    const float GetElapsed() const;
    const Uint64 GetElapsedMS() const;
    const float GetDelta() const;
    const Uint64 GetDeltaMS() const;

    RayHit RayCastFirst(
        b2Vec2 point1, b2Vec2 point2, const QueryFilter &filter
    );
    RayHit RayCastFirstGizmos(
        b2Vec2 point1, b2Vec2 point2, const QueryFilter &filter,
        Color defaultColor = Color(0, 255, 0),
        Color hitColor = Color(0, 255, 255)
    );

    void RayCast(
        b2Vec2 point1, b2Vec2 point2,
        const QueryFilter &filter, std::vector<RayHit> &result
    );
    void RayCastGizmos(
        b2Vec2 point1, b2Vec2 point2,
        const QueryFilter &filter, std::vector<RayHit> &result,
        Color defaultColor = Color(0, 255, 0),
        Color hitColor = Color(0, 255, 255)
    );

    void OverlapAABB(
        const b2AABB &aabb,
        const QueryFilter &filter, std::vector<OverlapResult> &result
    );
    void OverlapAABBGizmos(
        const b2AABB &aabb,
        const QueryFilter &filter, std::vector<OverlapResult> &result,
        Color defaultColor = Color(255, 0, 0),
        Color hitColor = Color(255, 0, 255)
    );

    void OverlapCircle(
        b2Vec2 center, float radius,
        const QueryFilter &filter, std::vector<OverlapResult> &result
    );
    void OverlapCircleGizmos(
        b2Vec2 center, float radius,
        const QueryFilter &filter, std::vector<OverlapResult> &result,
        Color defaultColor = Color(255, 0, 0),
        Color hitColor = Color(255, 0, 255)
    );

    void OverlapBox(
        b2Vec2 center, b2Vec2 halfExtents, float angleDeg,
        const QueryFilter &filter, std::vector<OverlapResult> &result
    );
    void OverlapBoxGizmos(
        b2Vec2 center, b2Vec2 halfExtents, float angleDeg,
        const QueryFilter &filter, std::vector<OverlapResult> &result,
        Color defaultColor = Color(255, 0, 0),
        Color hitColor = Color(255, 0, 255)
    );

    void OverlapPolygon(
        const b2Vec2 *vertices, int vertexCount,
        const QueryFilter &filter, std::vector<OverlapResult> &result
    );
    void OverlapPolygonGizmos(
        const b2Vec2 *vertices, int vertexCount,
        const QueryFilter &filter, std::vector<OverlapResult> &result,
        Color defaultColor = Color(255, 0, 0),
        Color hitColor = Color(255, 0, 255)
    );

    enum class UpdateMode
    {
        REALTIME,
        STEP_BY_STEP
    };
    UpdateMode GetUpdateMode() const;
    void SetUpdateMode(UpdateMode mode);

    void SetDrawGizmos(bool drawGizmos);
    bool GetDrawGizmos() const;
    void SetDrawGrid(bool drawGrid);
    bool GetDrawGrid() const;
    void SetDrawPhysics(bool drawPhysics);
    bool GetDrawPhysics() const;

    void PrintGameObjects() const;

    void MakeStep();
    Uint64 GetUpdateID() const;

protected:
    friend class GameObject;

    SceneManager *m_sceneManager;

    /// @brief Mode de mise à jour du moteur physique de la scène (temps réel ou pas-à-pas).
    UpdateMode m_mode;

    Timer m_time;

    ///// @brief Moteur physique.
    b2World m_world;

    ///// @brief Gestionnaire des instances de GameObject.
    ObjectManager m_objectManager;

    ///// @brief Gestionnaire des entrées utilisateur.
    InputManager *m_inputManager;

    ///// @brief Gestionnaire des ressources de la scène.
    AssetManager m_assetManager;

    ///// @brief Pointeur vers la caméra utilisée pour rendre la scène.
    Camera *m_activeCam;

    UICanvas *m_canvas;

    /// @brief Pas de temps fixe.
    Uint64 m_timeStepMS;

    /// @brief Accumulateur pour la mise à jour à pas de temps fixe.
    Uint64 m_stepAccuMS;

    Uint64 m_updateID;

    /// @brief Paramètre d'interpolation pour les positions des corps physiques.
    float m_alpha;

    /// @brief Booléen indiquant s'il faut afficher les "gizmos" (debug).
    bool m_drawGizmos;

    /// @brief Booléen indiquant s'il faut afficher corps (debug).
    bool m_drawPhysics;

    bool m_drawGrid;

    /// @brief Booléen indiquant s'il faut effectuer un pas fixe en mode pas-à-pas.
    bool m_makeStep;

    /// @brief Booléen indiquant s'il faut quitter la scène.
    bool m_quit;

    Gizmos m_gizmos;

    SceneContactListener m_contactListener;

private:
    void UpdateGameObjects();
    void MakeFixedStep();
    void PushQueryGizmos(Color color, b2Vec2 point1, b2Vec2 point2);
    void PushQueryGizmos(Color color, b2Fixture *fixture);
    void PushQueryGizmos(Color color, const b2AABB &aabb);
    void PushQueryGizmos(Color color, const GizmosShape &shape);

    bool m_inFixedUpdate;

    std::set<QueryGizmos *> m_queryGizmos;
    std::map<int, ParticleSystem *> m_particleSystemMap;
};

inline SceneManager *Scene::GetSceneManager()
{
    return m_sceneManager;
}

inline InputManager *Scene::GetInputManager()
{
    return m_inputManager;
}

inline AssetManager *Scene::GetAssetManager()
{
    return &m_assetManager;
}

inline b2World &Scene::GetWorld()
{
    return m_world;
}

inline float Scene::GetAlpha()
{
    return m_alpha;
}

inline Gizmos &Scene::GetGizmos()
{
    return m_gizmos;
}

inline void Scene::SetActiveCamera(Camera *camera)
{
    m_activeCam = camera;
}

inline Camera *Scene::GetActiveCamera()
{
    return m_activeCam;
}

inline UICanvas *Scene::GetCanvas()
{
    return m_canvas;
}

inline const Timer &Scene::GetTime() const
{
    return m_time;
}

inline Timer &Scene::GetTime()
{
    return m_time;
}

inline const float Scene::GetElapsed() const
{
    return m_time.GetElapsed();
}

inline const Uint64 Scene::GetElapsedMS() const
{
    return m_time.GetElapsedMS();
}

inline const float Scene::GetDelta() const
{
    return m_inFixedUpdate ? ((float)m_timeStepMS / 1000.f) : m_time.GetDelta();
}

inline const Uint64 Scene::GetDeltaMS() const
{
    return m_inFixedUpdate ? m_timeStepMS : m_time.GetDeltaMS();
}

inline Scene::UpdateMode Scene::GetUpdateMode() const
{
    return m_mode;
}

inline void Scene::SetUpdateMode(UpdateMode mode)
{
    m_mode = mode;
}

inline void Scene::SetDrawGizmos(bool drawGizmos)
{
    m_drawGizmos = drawGizmos;
}

inline bool Scene::GetDrawGizmos() const
{
    return m_drawGizmos;
}

inline void Scene::SetDrawGrid(bool drawGrid)
{
    m_drawGrid = drawGrid;
}

inline bool Scene::GetDrawGrid() const
{
    return m_drawGrid;
}

inline void Scene::SetDrawPhysics(bool drawPhysics)
{
    m_drawPhysics = drawPhysics;
}

inline bool Scene::GetDrawPhysics() const
{
    return m_drawPhysics;
}

inline void Scene::MakeStep()
{
    m_makeStep = true;
}

inline bool Scene::Contains(GameObject *gameObject) const
{
    return m_objectManager.Contains(gameObject);
}

inline Uint64 Scene::GetUpdateID() const
{
    return m_updateID;
}

struct QueryGizmos
{
    QueryGizmos(Color color, GizmosShape shape) :
        m_color(color), m_shape(shape) { }

    Color m_color;
    GizmosShape m_shape;
};

