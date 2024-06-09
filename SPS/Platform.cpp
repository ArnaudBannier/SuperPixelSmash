/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "Platform.h"
#include "Player.h"

Platform::Platform(Scene *scene, int layer) :
    Terrain(scene, layer), m_bodies(),
    m_angleTarget(0.f), m_angleMaxSpeed(100.f), m_angleSmoothTime(1.f),
    m_positionTarget(b2Vec2_zero), m_positionMaxSpeed(100.f), m_positionSmoothTime(1.f)
{
    SetName("Platform");
    SetOneWay(true);
}

Platform::~Platform()
{
}

void Platform::FixedUpdate()
{
    Terrain::FixedUpdate();

    b2Body *body = GetBody();
    if (body == nullptr) return;

    b2Vec2 velocity = GetVelocity();
    b2Vec2 position = GetPosition();

    float targetDist = (m_positionTarget - position).Length();
    if (fabsf(targetDist) > 0.01f)
    {
        float speed = velocity.Length();

        Math::SmoothDamp(
            0.f, targetDist, speed,
            m_positionSmoothTime, m_positionMaxSpeed, m_scene->GetDelta()
        );
        velocity = speed / targetDist * (m_positionTarget - position);
        body->SetLinearVelocity(velocity);
    }
    else
    {
        body->SetLinearVelocity(b2Vec2_zero);
    }

    float angle = body->GetAngle();
    float angularVelocity = body->GetAngularVelocity();

    if (fabsf(angle - m_angleTarget) > 0.005f)
    {
        Math::SmoothDamp(
            angle, m_angleTarget, angularVelocity,
            m_positionSmoothTime, m_positionMaxSpeed, m_scene->GetDelta()
        );
        body->SetAngularVelocity(angularVelocity);
    }
    else
    {
        body->SetAngularVelocity(0.f);
    }
}

void Platform::SetTargetPosition(b2Vec2 position, float smoothTime, float maxSpeed)
{
    m_positionTarget = position;
    m_positionSmoothTime = smoothTime;
    m_positionMaxSpeed = maxSpeed;
}

void Platform::SetTargetAngle(float angle, float smoothTime, float maxSpeed)
{
    m_angleTarget = angle;
    m_angleSmoothTime = smoothTime;
    m_angleMaxSpeed = maxSpeed;
}

void Platform::ApplyExternalVelocity()
{
    if (GetBody() == nullptr) return;

    b2Vec2 extVelocity = GetVelocity();
    extVelocity.y *= 0.5f;

    for (GameBody *gameBody : m_bodies)
    {
        if (m_scene->Contains(gameBody) == false) continue;

        Player *player = dynamic_cast<Player *>(gameBody);
        if (player)
        {
            player->AddExternalVelocity(extVelocity);
        }
    }

    m_bodies.clear();
}

const std::set<GameBody *> &Platform::GetBodies() const
{
    return m_bodies;
}
