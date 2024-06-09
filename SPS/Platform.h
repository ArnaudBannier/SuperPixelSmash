/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "Terrain.h"

class Platform : public Terrain
{
public:
    Platform(Scene *scene, int layer);
    virtual ~Platform();

    virtual void FixedUpdate() override;

    void SetTargetPosition(b2Vec2 position, float smoothTime, float maxSpeed);
    void SetTargetAngle(float angle, float smoothTime, float maxSpeed);
    void ApplyExternalVelocity();
    const std::set<GameBody *> &GetBodies() const;

    void AddGameBody(GameBody *gameBody);

protected:
    std::set<GameBody *>m_bodies;

    b2Vec2 m_positionTarget;
    float m_positionSmoothTime;
    float m_positionMaxSpeed;

    float m_angleTarget;
    float m_angleSmoothTime;
    float m_angleMaxSpeed;
};

inline void Platform::AddGameBody(GameBody *gameBody)
{
    m_bodies.insert(gameBody);
}
