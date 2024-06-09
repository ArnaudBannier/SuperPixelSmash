/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "Settings.h"
#include "Common.h"
#include "Color.h"

class Scene;
class GizmosShape;

class Gizmos
{
public:
    Gizmos(Scene *scene);

    void DrawPoint(const b2Vec2 &point);
    void DrawLine(const b2Vec2 &pointA, const b2Vec2 &pointB);
    void DrawVector(const b2Vec2 &origin, const b2Vec2 &vector);
    void DrawAABB(const b2AABB &aabb);
    void DrawCircle(const b2Vec2 &center, float radius);
    void DrawBox(const b2Vec2 &center, const b2Vec2 &halfExtents, float angleDeg);
    void DrawShape(const b2Shape &shape, const b2Transform &transform);
    void DrawShape(const GizmosShape &shape);


    void SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255);
    void SetColor(Color color);

private:
    Scene *m_scene;
    Color m_color;
};

inline void Gizmos::DrawVector(const b2Vec2 &origin, const b2Vec2 &vector)
{
    DrawLine(origin, origin + vector);
}

class GizmosShape
{
public:
    GizmosShape();
    GizmosShape(const b2Shape &shape, const b2Transform &transform);
    GizmosShape(b2Vec2 point1, b2Vec2 point2);
    GizmosShape(const b2AABB &aabb);
    GizmosShape(const b2Fixture *fixture);

    void Set(const b2Shape &shape, const b2Transform &transform);
    void Set(const b2AABB &aabb);
    void Set(const b2Fixture *fixture);
    void Set(b2Vec2 point1, b2Vec2 point2);

private:
    friend class Gizmos;
    void SetAsPolygon(const b2PolygonShape &polygon, const b2Transform &transform);
    void SetAsCircle(const b2CircleShape &circle, const b2Transform &transform);
    void SetAsEdge(const b2EdgeShape &edge, const b2Transform &transform);
    void SetAsChain(const b2ChainShape &chain, const b2Transform &transform);

    std::vector<b2Vec2> m_vertices;
};

inline void Gizmos::SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    m_color.Set(r, g, b, a);
}

inline void Gizmos::SetColor(Color color)
{
    m_color = color;
}
