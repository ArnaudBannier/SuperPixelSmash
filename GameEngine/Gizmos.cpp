/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "Gizmos.h"
#include "Camera.h"

Gizmos::Gizmos(Scene *scene) : m_scene(scene)
{
}

void Gizmos::DrawPoint(const b2Vec2 &point)
{
    Camera *camera = m_scene->GetActiveCamera();
    m_color.SetAsRenderColor();

    float x, y;
    camera->WorldToView(point, x, y);

    int w = 3;
    SDL_RenderDrawLine(
        g_renderer, (int)roundf(x) - w, (int)roundf(y) - w,
        (int)roundf(x) + w, (int)roundf(y) + w);
    SDL_RenderDrawLine(
        g_renderer, (int)roundf(x) + w, (int)roundf(y) - w,
        (int)roundf(x) - w, (int)roundf(y) + w);
}

void Gizmos::DrawLine(const b2Vec2 &pointA, const b2Vec2 &pointB)
{
    Camera *camera = m_scene->GetActiveCamera();
    m_color.SetAsRenderColor();

    float xA, yA, xB, yB;
    camera->WorldToView(pointA, xA, yA);
    camera->WorldToView(pointB, xB, yB);

    SDL_RenderDrawLine(
        g_renderer, (int)roundf(xA), (int)roundf(yA),
        (int)roundf(xB), (int)roundf(yB));
}

void Gizmos::DrawAABB(const b2AABB &aabb)
{
    Camera *camera = m_scene->GetActiveCamera();
    m_color.SetAsRenderColor();

    float xL, yL, xU, yU;
    camera->WorldToView(aabb.lowerBound, xL, yL);
    camera->WorldToView(aabb.upperBound, xU, yU);

    SDL_Rect rect = { 0 };
    rect.x = (int)roundf(xL);
    rect.y = (int)roundf(yU);
    rect.w = (int)roundf(fabsf(xU - xL));
    rect.h = (int)roundf(fabsf(yU - yL));

    SDL_RenderDrawRect(g_renderer, &rect);
}

void Gizmos::DrawCircle(const b2Vec2 &center, float radius)
{
    b2Transform xf(center, b2Rot(0.f));
    b2CircleShape circleShape;
    circleShape.m_p = b2Vec2_zero;
    circleShape.m_radius = radius;

    GizmosShape gShape(circleShape, xf);
    DrawShape(gShape);
}

void Gizmos::DrawBox(const b2Vec2 &center, const b2Vec2 &halfExtents, float angleDeg)
{
    b2Transform xf(b2Vec2_zero, b2Rot(0.f));
    b2PolygonShape boxShape;
    boxShape.SetAsBox(halfExtents.x, halfExtents.y, center, angleDeg * b2_pi / 180.f);

    GizmosShape gShape(boxShape, xf);
    DrawShape(gShape);
}

void Gizmos::DrawShape(const b2Shape &shape, const b2Transform &transform)
{
    GizmosShape gShape(shape, transform);
    DrawShape(gShape);
}

void Gizmos::DrawShape(const GizmosShape &shape)
{
    Camera *camera = m_scene->GetActiveCamera();
    m_color.SetAsRenderColor();

    const std::vector<b2Vec2> &vertices = shape.m_vertices;
    if (vertices.size() < 2) return;

    SDL_Point points[2] = { 0 };

    float x, y;
    camera->WorldToView(vertices.front(), x, y);
    points[0].x = (int)x;
    points[0].y = (int)y;

    for (int i = 1; i < shape.m_vertices.size(); ++i)
    {
        camera->WorldToView(vertices[i], x, y);
        points[1].x = (int)x;
        points[1].y = (int)y;

        SDL_RenderDrawLines(g_renderer, points, 2);
        points[0] = points[1];
    }
}

GizmosShape::GizmosShape() : m_vertices()
{
}

GizmosShape::GizmosShape(const b2Shape &shape, const b2Transform &transform) :
    m_vertices()
{
    Set(shape, transform);
}

GizmosShape::GizmosShape(b2Vec2 point1, b2Vec2 point2) :
    m_vertices()
{
    Set(point1, point2);
}

GizmosShape::GizmosShape(const b2AABB &aabb) :
    m_vertices()
{
    Set(aabb);
}

GizmosShape::GizmosShape(const b2Fixture *fixture) :
    m_vertices()
{
    Set(fixture);
}

void GizmosShape::Set(const b2Shape &shape, const b2Transform &transform)
{
    m_vertices.clear();
    switch (shape.m_type)
    {
    case b2Shape::e_polygon:
        SetAsPolygon(dynamic_cast<const b2PolygonShape &>(shape), transform);
        break;
    case b2Shape::e_circle:
        SetAsCircle(dynamic_cast<const b2CircleShape &>(shape), transform);
        break;
    case b2Shape::e_edge:
        SetAsEdge(dynamic_cast<const b2EdgeShape &>(shape), transform);
        break;
    case b2Shape::e_chain:
        SetAsChain(dynamic_cast<const b2ChainShape &>(shape), transform);
        break;
    default:
        break;
    }
}

void GizmosShape::Set(const b2AABB &aabb)
{
    m_vertices.clear();
    m_vertices.push_back(aabb.lowerBound);
    m_vertices.push_back(b2Vec2(aabb.upperBound.x, aabb.lowerBound.y));
    m_vertices.push_back(aabb.upperBound);
    m_vertices.push_back(b2Vec2(aabb.lowerBound.x, aabb.upperBound.y));
    m_vertices.push_back(m_vertices.front());
}

void GizmosShape::Set(const b2Fixture *fixture)
{
    const b2Transform &xf = fixture->GetBody()->GetTransform();
    const b2Shape &fixtureShape = *(fixture->GetShape());
    Set(fixtureShape, xf);
}

void GizmosShape::Set(b2Vec2 point1, b2Vec2 point2)
{
    m_vertices.clear();
    m_vertices.push_back(point1);
    m_vertices.push_back(point2);
}

void GizmosShape::SetAsPolygon(const b2PolygonShape &polygon, const b2Transform &transform)
{
    assert(polygon.m_count > 1);

    for (int i = 0; i < polygon.m_count; i++)
    {
        m_vertices.push_back(b2Mul(transform, polygon.m_vertices[i]));
    }
    m_vertices.push_back(m_vertices.front());
}

void GizmosShape::SetAsCircle(const b2CircleShape &circle, const b2Transform &transform)
{
    constexpr int CIRCLE_SEG_COUNT = 16;
    const float inc = 2.f * b2_pi / (float)CIRCLE_SEG_COUNT;
    float theta = 0.f;

    for (int i = 0; i < CIRCLE_SEG_COUNT; i++, theta += inc)
    {
        m_vertices.push_back(
            b2Mul(
                transform,
                circle.m_p + circle.m_radius * b2Vec2(cosf(theta), sinf(theta))
            )
        );
    }
    m_vertices.push_back(m_vertices.front());
}

void GizmosShape::SetAsEdge(const b2EdgeShape &edge, const b2Transform &transform)
{
    m_vertices.push_back(b2Mul(transform, edge.m_vertex1));
    m_vertices.push_back(b2Mul(transform, edge.m_vertex2));
}

void GizmosShape::SetAsChain(const b2ChainShape &chain, const b2Transform &transform)
{
    for (int32 i = 0; i < chain.GetChildCount(); ++i)
    {
        b2EdgeShape edge;
        chain.GetChildEdge(&edge, i);
        m_vertices.push_back(b2Mul(transform, edge.m_vertex1));
        m_vertices.push_back(b2Mul(transform, edge.m_vertex2));
    }
}
