#include "Scene.h"
#include "ObjectManager.h"
#include "GameObject.h"
#include "GameBody.h"
#include "Settings.h"
#include "Camera.h"
#include "Gizmos.h"
#include "ParticleSystem.h"
#include "SceneManager.h"
#include "UICanvas.h"

#define TIME_STEP_MS 20

GameCollision::GameCollision(b2Contact *contact, bool first) :
    m_contact(contact), m_first(first)
{
    contact->GetWorldManifold(&manifold);

    if (first)
    {
        fixture = contact->GetFixtureA();
        otherFixture = contact->GetFixtureB();
    }
    else
    {
        manifold.normal = -manifold.normal;
        fixture = contact->GetFixtureB();
        otherFixture = contact->GetFixtureA();
    }

    gameBody = GameBody::GetFromBody(otherFixture->GetBody());
}

Scene::Scene(SceneManager *manager, InputManager *inputManager) :
    m_sceneManager(manager), m_inputManager(inputManager),
    m_stepAccuMS(0), m_alpha(0.f), m_drawPhysics(false), m_drawGizmos(false), m_drawGrid(false),
    m_makeStep(false), m_mode(UpdateMode::REALTIME),
    m_objectManager(), m_quit(false), m_timeStepMS(TIME_STEP_MS), m_inFixedUpdate(false),
    m_time(), m_assetManager(),
    m_contactListener(), m_particleSystemMap(),
    m_world(b2Vec2(0.f, -40.f)), m_queryGizmos(), m_gizmos(this), m_updateID(0)
{
    m_world.SetContactListener(&m_contactListener);
    m_activeCam = nullptr;
    m_canvas = new UICanvas(this);

    if (inputManager == nullptr)
    {
        assert(false);
        abort();
    }
}

Scene::~Scene()
{
    m_objectManager.DeleteObjects();
}

class DrawQueryCallback : public b2QueryCallback
{
public:
    DrawQueryCallback(Scene *scene) : m_scene(scene) { }

    bool ReportFixture(b2Fixture *fixture)
    {
        b2Body *body = fixture->GetBody();
        b2Shape *shape = fixture->GetShape();
        b2Transform xf = body->GetTransform();
        GameBody *gameBody = GameBody::GetFromBody(body);
        Gizmos &gizmos = m_scene->GetGizmos();
        if (gameBody)
        {
            xf.p = gameBody->GetInterpolatedPosition();
            gizmos.SetColor(gameBody->GetDebugColor());
        }

        gizmos.DrawPoint(xf.p);
        gizmos.DrawShape(*shape, xf);

        return true;
    }

private:
    Scene *m_scene;
};

void Scene::Render()
{
    if (m_activeCam == nullptr)
    {
        return;
    }

    if (m_sceneManager) m_sceneManager->OnSceneRender();

    // Détermine les objets visibles par la caméra
    b2AABB worldView = m_activeCam->GetWorldView();
    m_objectManager.AddVisibleBodies(m_world, worldView);

    // Dessine les objets visibles par la caméra
    // Message : Render()
    m_objectManager.ProcessVisibleObjects();
    for (auto it = m_objectManager.visibleObjectsBegin();
         it != m_objectManager.visibleObjectsEnd(); ++it)
    {
        GameObject *object = *it;
        object->Render();
    }

    // Dessine la grille
    if (m_drawGrid)
    {
        b2AABB worldView = m_activeCam->GetWorldView();
        for (int i = (int)worldView.lowerBound.y; i < worldView.upperBound.y; i++)
        {
            if (i == 0)
                SDL_SetRenderDrawColor(g_renderer, 255, 255, 255, 255);
            else if ((i % 10) == 0)
                SDL_SetRenderDrawColor(g_renderer, 0, 255, 255, 255);
            else
                SDL_SetRenderDrawColor(g_renderer, 0, 255, 255, 63);

            float x, y;
            m_activeCam->WorldToView(b2Vec2(0.f, (float)i), x, y);
            SDL_RenderDrawLine(
                g_renderer, 0, (int)y, m_activeCam->GetRasterWidth(), (int)y
            );
        }
        for (int i = (int)worldView.lowerBound.x; i < worldView.upperBound.x; i++)
        {
            if (i == 0)
                SDL_SetRenderDrawColor(g_renderer, 255, 255, 255, 255);
            else if ((i % 10) == 0)
                SDL_SetRenderDrawColor(g_renderer, 0, 255, 255, 255);
            else
                SDL_SetRenderDrawColor(g_renderer, 0, 255, 255, 63);

            float x, y;
            m_activeCam->WorldToView(b2Vec2((float)i, 0.f), x, y);
            SDL_RenderDrawLine(
                g_renderer, (int)x, 0, (int)x, m_activeCam->GetRasterHeight()
            );
        }
    }

    // Dessine les corps présents dans le moteur physique
    if (m_drawPhysics && m_activeCam != nullptr)
    {
        b2AABB worldView = m_activeCam->GetWorldView();
        DrawQueryCallback drawCallback(this);
        m_world.QueryAABB(&drawCallback, worldView);
    }

    // Dessine les gizmos
    // Message : DrawGizmos()
    if (m_drawGizmos)
    {
        Gizmos gizmos(this);

        for (auto it = m_objectManager.visibleObjectsBegin();
             it != m_objectManager.visibleObjectsEnd(); ++it)
        {
            GameObject *object = *it;
            object->DrawGizmos(gizmos);
        }

        // Dessine les gizmos automatiques
        for (auto it = m_queryGizmos.begin(); it != m_queryGizmos.end(); ++it)
        {
            QueryGizmos *autoGizmos = *it;
            gizmos.SetColor(autoGizmos->m_color);
            gizmos.DrawShape(autoGizmos->m_shape);
        }
    }
}

void Scene::PrintGameObjects() const
{
    m_objectManager.PrintObjects();
}

void Scene::Update()
{
    m_updateID++;

    // Appelle les méthodes asynchrones
    // Messages : Start(), OnEnable(), OnDisable(), Delete()
    m_objectManager.ProcessObjects();

    // Met à jour les entrées de l'utilisateur
    m_inputManager->ProcessEvents();

    // Met à jour les objets
    // Messages : Update(), FixedUpdate()
    UpdateGameObjects();
}


void Scene::MakeFixedStep()
{
    const float timeStep = (float)m_timeStepMS / 1000.f;
    m_inFixedUpdate = true;

    for (QueryGizmos *autoGizmos : m_queryGizmos)
    {
        delete autoGizmos;
    }
    m_queryGizmos.clear();

    if (m_sceneManager) m_sceneManager->OnSceneFixedUpdate();

    int32 velocityIterations = 6;
    int32 positionIterations = 2;
    m_world.Step(timeStep, velocityIterations, positionIterations);

    for (auto it = m_objectManager.begin(); it != m_objectManager.end(); ++it)
    {
        GameObject *object = *it;
        if (object->IsEnabled())
        {
            object->FixedUpdate();

            GameBody *gameBody = dynamic_cast<GameBody *>(object);
            if (gameBody)
            {
                gameBody->UpdateInterpolation();
            }
        }
    }
    m_inFixedUpdate = false;
}

void Scene::UpdateGameObjects()
{
    // Appelle la méthode FixedUpdate de chaque GameObject
    if (m_mode == UpdateMode::REALTIME)
    {
        // Mode temps réel
        m_time.Update();

        m_stepAccuMS += m_time.GetDeltaMS();
        while (m_stepAccuMS >= m_timeStepMS)
        {
            MakeFixedStep();
            m_stepAccuMS -= m_timeStepMS;
        }
        m_alpha = (float)m_stepAccuMS / (float)m_timeStepMS;
    }
    else
    {
        // Mode pas à pas
        if (m_makeStep)
        {
            m_time.Update(m_timeStepMS);
            MakeFixedStep();
        }
        else
        {
            m_time.Update(0.f);
        }
        m_makeStep = false;
        m_alpha = 1.f;
    }

    if (m_sceneManager) m_sceneManager->OnSceneUpdate();

    // Appelle la méthode Update de chaque GameObject
    int enabledCount = 0;
    for (auto it = m_objectManager.begin();
         it != m_objectManager.end(); ++it)
    {
        GameObject *object = *it;
        if (object->IsEnabled())
        {
            object->Update();
            enabledCount++;
        }
    }
}

ParticleSystem *Scene::GetParticleSystem(int layer)
{
    auto it = m_particleSystemMap.find(layer);
    if (it != m_particleSystemMap.end())
    {
        return it->second;
    }

    ParticleSystem *particleSystem = new ParticleSystem(this, layer);
    m_particleSystemMap.insert(std::make_pair(layer, particleSystem));

    return particleSystem;
}

void Scene::ShakeCamera(int intensity)
{
    if (m_activeCam) m_activeCam->Shake(intensity);
}

void Scene::PushQueryGizmos(Color color, b2Vec2 point1, b2Vec2 point2)
{
    m_queryGizmos.insert(
        new QueryGizmos(color, GizmosShape(point1, point2))
    );
}

void Scene::PushQueryGizmos(Color color, b2Fixture *fixture)
{
    const b2Transform &xf = fixture->GetBody()->GetTransform();
    const b2Shape &fixtureShape = *(fixture->GetShape());
    m_queryGizmos.insert(
        new QueryGizmos(color, GizmosShape(fixtureShape, xf))
    );
}

void Scene::PushQueryGizmos(Color color, const b2AABB &aabb)
{
    m_queryGizmos.insert(
        new QueryGizmos(color, GizmosShape(aabb))
    );
}

void Scene::PushQueryGizmos(Color color, const GizmosShape &shape)
{
    m_queryGizmos.insert(new QueryGizmos(color, shape));
}

class SceneRayCastCallback : public b2RayCastCallback
{
public:
    SceneRayCastCallback(const QueryFilter &filter) :
        m_filter(filter)
    {
        firstHit.gameBody = nullptr;
        firstHit.fixture = nullptr;
    }
    virtual ~SceneRayCastCallback() {}

    virtual float ReportFixture(
        b2Fixture* fixture, const b2Vec2& point,
        const b2Vec2& normal, float fraction) override
    {
        if (firstHit.fixture != nullptr && firstHit.fraction < fraction)
        {
            return true;
        }
        if (m_filter.solidOnly && fixture->IsSensor())
        {
            return true;
        }

        if ((fixture->GetFilterData().categoryBits & m_filter.maskBits) != 0)
        {
            firstHit.fixture = fixture;
            firstHit.fraction = fraction;
            firstHit.gameBody = GameBody::GetFromBody(fixture->GetBody());
            firstHit.hitPoint = point;
            firstHit.normal = normal;
        }

        return true;
    }

    RayHit firstHit;

private:
    const QueryFilter &m_filter;
};

RayHit Scene::RayCastFirst(b2Vec2 point1, b2Vec2 point2, const QueryFilter &filter)
{
    SceneRayCastCallback callback(filter);
    m_world.RayCast(&callback, point1, point2);
    return callback.firstHit;
}

RayHit Scene::RayCastFirstGizmos(
    b2Vec2 point1, b2Vec2 point2, const QueryFilter &filter,
    Color defaultColor, Color hitColor)
{
    RayHit firstHit = RayCastFirst(point1, point2, filter);
    if (m_drawGizmos)
    {
        b2Fixture *fixture = firstHit.fixture;
        if (fixture)
        {
            m_queryGizmos.insert(new QueryGizmos(hitColor, GizmosShape(fixture)));
            m_queryGizmos.insert(new QueryGizmos(hitColor, GizmosShape(point1, point2)));
        }
        else
        {
            m_queryGizmos.insert(new QueryGizmos(defaultColor, GizmosShape(point1, point2)));

        }
    }
    return firstHit;
}

class SceneRayCastAllCallback : public b2RayCastCallback
{
public:
    SceneRayCastAllCallback(const QueryFilter &filter, std::vector<RayHit> &result) :
        m_filter(filter), m_result(result)
    { }
    virtual ~SceneRayCastAllCallback() {}

    virtual float ReportFixture(
        b2Fixture* fixture, const b2Vec2& point,
        const b2Vec2& normal, float fraction) override
    {
        if (m_filter.solidOnly && fixture->IsSensor())
        {
            return true;
        }

        if ((fixture->GetFilterData().categoryBits & m_filter.maskBits) != 0)
        {
            RayHit rayHit;
            rayHit.fixture = fixture;
            rayHit.fraction = fraction;
            rayHit.gameBody = GameBody::GetFromBody(fixture->GetBody());
            rayHit.hitPoint = point;
            rayHit.normal = normal;

            m_result.push_back(rayHit);
        }

        return true;
    }

private:
    const QueryFilter &m_filter;
    std::vector<RayHit> &m_result;
};

void Scene::RayCast(
    b2Vec2 point1, b2Vec2 point2,
    const QueryFilter &filter, std::vector<RayHit> &result)
{
    result.clear();

    SceneRayCastAllCallback callback(filter, result);
    m_world.RayCast(&callback, point1, point2);
}

void Scene::RayCastGizmos(
    b2Vec2 point1, b2Vec2 point2,
    const QueryFilter &filter, std::vector<RayHit> &result,
    Color defaultColor, Color hitColor)
{
    RayCast(point1, point2, filter, result);

    if (m_drawGizmos)
    {
        Color color = result.empty() ? defaultColor : hitColor;
        m_queryGizmos.insert(new QueryGizmos(color, GizmosShape(point1, point2)));
        for (auto it = result.begin(); it != result.end(); ++it)
        {
            const RayHit &rayHit = *it;
            m_queryGizmos.insert(new QueryGizmos(color, GizmosShape(rayHit.fixture)));
        }
    }
}

class SceneOverlapAABBCallback : public b2QueryCallback
{
public:
    SceneOverlapAABBCallback(
        const b2AABB &aabb, const QueryFilter &filter, std::vector<OverlapResult> &result) :
        m_aabb(aabb), m_filter(filter), m_result(result)
    { }
    virtual ~SceneOverlapAABBCallback() {}

    virtual bool ReportFixture(b2Fixture *fixture) override
    {
        if (m_filter.solidOnly && fixture->IsSensor())
        {
            return true;
        }

        if ((fixture->GetFilterData().categoryBits & m_filter.maskBits) != 0)
        {
            bool overlap = false;
            b2Shape *shape = fixture->GetShape();
            const b2Transform &xf = fixture->GetBody()->GetTransform();
            b2AABB aabb;
            for (int32 i = 0; i < shape->GetChildCount(); i++)
            {
                shape->ComputeAABB(&aabb, xf, i);
                if (b2TestOverlap(m_aabb, aabb))
                {
                    overlap = true;
                    break;
                }
            }
            if (overlap)
            {
                OverlapResult overlapRes;
                overlapRes.fixture = fixture;
                overlapRes.gameBody = GameBody::GetFromBody(fixture->GetBody());
                m_result.push_back(overlapRes);
            }
        }
        return true;
    }

private:
    const b2AABB &m_aabb;
    const QueryFilter &m_filter;
    std::vector<OverlapResult> &m_result;
};

class SceneOverlapShapeCallback : public b2QueryCallback
{
public:
    SceneOverlapShapeCallback(
        const b2Shape *shape, const b2Transform &xf,
        const QueryFilter &filter, std::vector<OverlapResult> &result) :
        m_shape(shape), m_xf(xf), m_filter(filter), m_result(result)
    { }
    virtual ~SceneOverlapShapeCallback() {}

    virtual bool ReportFixture(b2Fixture *fixture) override
    {
        if (m_filter.solidOnly && fixture->IsSensor())
        {
            return true;
        }

        if ((fixture->GetFilterData().categoryBits & m_filter.maskBits) != 0)
        {
            bool overlap = false;
            b2Shape *shape = fixture->GetShape();
            const b2Transform &xf = fixture->GetBody()->GetTransform();
            for (int32 i = 0; i < shape->GetChildCount(); i++)
            {
                if (b2TestOverlap(m_shape, (int32)0, shape, i, m_xf, xf))
                {
                    overlap = true;
                    break;
                }
            }
            if (overlap)
            {
                OverlapResult overlapRes;
                overlapRes.fixture = fixture;
                overlapRes.gameBody = GameBody::GetFromBody(fixture->GetBody());
                m_result.push_back(overlapRes);
            }
        }
        return true;
    }

private:
    const b2Shape *m_shape;
    const b2Transform &m_xf;
    const QueryFilter &m_filter;
    std::vector<OverlapResult> &m_result;
};

void Scene::OverlapAABB(
    const b2AABB &aabb,
    const QueryFilter &filter, std::vector<OverlapResult> &result)
{
    result.clear();

    SceneOverlapAABBCallback callback(aabb, filter, result);
    m_world.QueryAABB(&callback, aabb);
}

void Scene::OverlapAABBGizmos(
    const b2AABB &aabb,
    const QueryFilter &filter, std::vector<OverlapResult> &result,
    Color defaultColor, Color hitColor)
{
    OverlapAABB(aabb, filter, result);

    if (m_drawGizmos)
    {
        Color color = result.empty() ? defaultColor : hitColor;
        m_queryGizmos.insert(new QueryGizmos(color, GizmosShape(aabb)));
        for (auto it = result.begin(); it != result.end(); ++it)
        {
            const OverlapResult &overlap = *it;
            m_queryGizmos.insert(new QueryGizmos(color, GizmosShape(overlap.fixture)));
        }
    }
}

void Scene::OverlapCircle(
    b2Vec2 center, float radius,
    const QueryFilter &filter, std::vector<OverlapResult> &result)
{
    result.clear();

    b2AABB aabb;
    b2CircleShape circleShape;
    b2Transform xf(b2Vec2_zero, b2Rot(0.f));
    circleShape.m_p = center;
    circleShape.m_radius = radius;
    circleShape.ComputeAABB(&aabb, xf, 0);

    SceneOverlapShapeCallback callback(&circleShape, xf, filter, result);
    m_world.QueryAABB(&callback, aabb);
}

void Scene::OverlapCircleGizmos(
    b2Vec2 center, float radius,
    const QueryFilter &filter, std::vector<OverlapResult> &result,
    Color defaultColor, Color hitColor)
{
    OverlapCircle(center, radius, filter, result);

    if (m_drawGizmos)
    {
        b2Transform xf(b2Vec2_zero, b2Rot(0.f));
        b2CircleShape circleShape;
        circleShape.m_p = center;
        circleShape.m_radius = radius;

        Color color = result.empty() ? defaultColor : hitColor;
        m_queryGizmos.insert(new QueryGizmos(color, GizmosShape(circleShape, xf)));
        for (auto it = result.begin(); it != result.end(); ++it)
        {
            const OverlapResult &overlap = *it;
            m_queryGizmos.insert(new QueryGizmos(color, GizmosShape(overlap.fixture)));
        }
    }
}

void Scene::OverlapBox(
    b2Vec2 center, b2Vec2 halfExtents, float angleDeg,
    const QueryFilter &filter, std::vector<OverlapResult> &result)
{
    result.clear();

    b2AABB aabb;
    b2PolygonShape boxShape;
    b2Transform xf(b2Vec2_zero, b2Rot(0.f));
    boxShape.SetAsBox(halfExtents.x, halfExtents.y, center, angleDeg * b2_pi / 180.f);
    boxShape.ComputeAABB(&aabb, xf, 0);

    SceneOverlapShapeCallback callback(&boxShape, xf, filter, result);
    m_world.QueryAABB(&callback, aabb);
}

void Scene::OverlapBoxGizmos(
    b2Vec2 center, b2Vec2 halfExtents, float angleDeg,
    const QueryFilter &filter, std::vector<OverlapResult> &result,
    Color defaultColor, Color hitColor)
{
    OverlapBox(center, halfExtents, angleDeg, filter, result);

    if (m_drawGizmos)
    {
        b2Transform xf(b2Vec2_zero, b2Rot(0.f));
        b2PolygonShape boxShape;
        boxShape.SetAsBox(halfExtents.x, halfExtents.y, center, angleDeg * b2_pi / 180.f);

        Color color = result.empty() ? defaultColor : hitColor;
        m_queryGizmos.insert(new QueryGizmos(color, GizmosShape(boxShape, xf)));
        for (auto it = result.begin(); it != result.end(); ++it)
        {
            const OverlapResult &overlap = *it;
            m_queryGizmos.insert(new QueryGizmos(color, GizmosShape(overlap.fixture)));
        }
    }
}

void Scene::OverlapPolygon(
    const b2Vec2 *vertices, int vertexCount,
    const QueryFilter &filter, std::vector<OverlapResult> &result)
{
    assert(vertices && (2 < vertexCount) && (vertexCount < b2_maxPolygonVertices));
    result.clear();

    b2AABB aabb;
    b2Transform xf(b2Vec2_zero, b2Rot(0.f));

    b2Hull hull = b2ComputeHull(vertices, vertexCount);
    assert(b2ValidateHull(hull));

    b2PolygonShape polygonShape;
    polygonShape.Set(hull);
    polygonShape.ComputeAABB(&aabb, xf, 0);

    SceneOverlapShapeCallback callback(&polygonShape, xf, filter, result);
    m_world.QueryAABB(&callback, aabb);
}

void Scene::OverlapPolygonGizmos(
    const b2Vec2 *vertices, int vertexCount,
    const QueryFilter &filter, std::vector<OverlapResult> &result,
    Color defaultColor, Color hitColor)
{
    OverlapPolygon(vertices, vertexCount, filter, result);

    if (m_drawGizmos)
    {
        b2Transform xf(b2Vec2_zero, b2Rot(0.f));
        b2PolygonShape polygonShape;
        polygonShape.Set(b2ComputeHull(vertices, vertexCount));

        Color color = result.empty() ? defaultColor : hitColor;
        m_queryGizmos.insert(new QueryGizmos(color, GizmosShape(polygonShape, xf)));
        for (auto it = result.begin(); it != result.end(); ++it)
        {
            const OverlapResult &overlap = *it;
            m_queryGizmos.insert(new QueryGizmos(color, GizmosShape(overlap.fixture)));
        }
    }
}

float Scene::GetUIPixelsPerUnit() const
{
    if (m_canvas) return m_canvas->GetPixelsPerUnit();
    return 1.0f;
}

void SceneContactListener::BeginContact(b2Contact *contact)
{
    GameBody *gameBodyA = GameBody::GetFromBody(contact->GetFixtureA()->GetBody());
    GameBody *gameBodyB = GameBody::GetFromBody(contact->GetFixtureB()->GetBody());
    assert(gameBodyA && gameBodyB);

    GameCollision gameCollisionA(contact, true);
    GameCollision gameCollisionB(contact, false);

    gameBodyA->OnCollisionEnter(gameCollisionA);
    gameBodyB->OnCollisionEnter(gameCollisionB);
}

void SceneContactListener::EndContact(b2Contact *contact)
{
    GameBody *gameBodyA = GameBody::GetFromBody(contact->GetFixtureA()->GetBody());
    GameBody *gameBodyB = GameBody::GetFromBody(contact->GetFixtureB()->GetBody());
    assert(gameBodyA && gameBodyB);

    GameCollision gameCollisionA(contact, true);
    GameCollision gameCollisionB(contact, false);

    gameBodyA->OnCollisionExit(gameCollisionA);
    gameBodyB->OnCollisionExit(gameCollisionB);
}

void SceneContactListener::PreSolve(b2Contact *contact, const b2Manifold *oldManifold)
{
    GameBody *gameBodyA = GameBody::GetFromBody(contact->GetFixtureA()->GetBody());
    GameBody *gameBodyB = GameBody::GetFromBody(contact->GetFixtureB()->GetBody());
    assert(gameBodyA && gameBodyB);

    GameCollision gameCollisionA(contact, true);
    GameCollision gameCollisionB(contact, false);

    gameBodyA->OnCollisionStay(gameCollisionA);
    gameBodyB->OnCollisionStay(gameCollisionB);
}
