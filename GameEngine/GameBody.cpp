/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "GameBody.h"
#include "Scene.h"
#include "Gizmos.h"
#include "Camera.h"

GameBody::GameBody(Scene *scene, int layer) :
    GameObject(scene, layer), m_startPos(b2Vec2_zero),
    m_body(nullptr), m_debugColor(0, 200, 255),
    m_currXf(b2Vec2(0.f, 0.f), b2Rot(0.f)),
    m_lastXf(b2Vec2(0.f, 0.f), b2Rot(0.f))
{
    SetName("GameBody");
}

GameBody::~GameBody()
{
    DeleteBody();
}

void GameBody::OnCollisionEnter(GameCollision &collision)
{
}

void GameBody::OnCollisionStay(GameCollision &collision)
{
}

void GameBody::OnCollisionExit(GameCollision &collision)
{
}

GameBody *GameBody::GetFromBody(b2Body *body)
{
    assert(body);
    return (GameBody *)(body->GetUserData().pointer);
}

b2Body *GameBody::CreateBody(b2BodyDef *bodyDef)
{
    assert(bodyDef && m_body == nullptr);
    bodyDef->userData.pointer = (uintptr_t)this;

    b2World &world = m_scene->GetWorld();
    m_body = world.CreateBody(bodyDef);
    AssertNew(m_body);

    m_currXf = b2Transform(bodyDef->position, b2Rot(bodyDef->angle));
    m_lastXf = m_currXf;

    return m_body;
}

b2Fixture *GameBody::CreateFixture(b2FixtureDef *fixtureDef)
{
    assert(fixtureDef && m_body);
    if (m_body == nullptr) return nullptr;

    b2Fixture *fixture = m_body->CreateFixture(fixtureDef);
    AssertNew(fixture);
    return fixture;
}

void GameBody::DeleteBody()
{
    if (m_body == nullptr)
    {
        return;
    }
    m_scene->GetWorld().DestroyBody(m_body);
    m_body = nullptr;
}

b2Vec2 GameBody::GetPosition() const
{
    if (m_body)
    {
        return m_body->GetPosition();
    }
    else
    {
        assert(false);
        return b2Vec2_zero;
    }
}

b2Vec2 GameBody::GetInterpolatedPosition() const
{
    if (m_body)
    {
        float alpha = m_scene->GetAlpha();
        return alpha * m_currXf.p + (1.f - alpha) * m_lastXf.p;
    }
    else
    {
        assert(false);
        return b2Vec2_zero;
    }
}

b2Vec2 GameBody::GetVelocity() const
{
    if (m_body)
    {
        return m_body->GetLinearVelocity();
    }
    else
    {
        assert(false);
        return b2Vec2_zero;
    }
}

b2Transform GameBody::GetTransform() const
{
    if (m_body)
    {
        return m_body->GetTransform();
    }
    else
    {
        assert(false);
        b2Transform xf(b2Vec2_zero, b2Rot(0.f));
        return xf;
    }
}

b2Transform GameBody::GetInterpolatedTransform() const
{
    if (m_body)
    {
        float alpha = m_scene->GetAlpha();
        b2Vec2 p = alpha * m_currXf.p + (1.f - alpha) * m_lastXf.p;
        b2Rot q(alpha * m_currXf.q.GetAngle() + (1.f - alpha) * m_lastXf.q.GetAngle());
        return b2Transform(p, q);
    }
    else
    {
        assert(false);
        return b2Transform(b2Vec2_zero, b2Rot(0.f));
    }
}

b2AABB GameBody::GetAABB() const
{
    if (m_body == nullptr)
    {
        b2AABB res;
        res.lowerBound = b2Vec2_zero;
        res.upperBound = b2Vec2_zero;
        return res;
    }

    b2Transform xf = GetInterpolatedTransform();

    b2AABB aabb;
    aabb.lowerBound = xf.p;
    aabb.upperBound = xf.p;

    for (const b2Fixture *fixture = m_body->GetFixtureList();
         fixture != nullptr; fixture = fixture->GetNext())
    {
        const b2Shape *shape = fixture->GetShape();
        int childCount = shape->GetChildCount();
        for (int i = 0; i < childCount; i++)
        {
            b2AABB fixtureAABB;
            shape->ComputeAABB(&fixtureAABB, xf, i);
            aabb.Combine(fixtureAABB);
        }
    }
    return aabb;
}
