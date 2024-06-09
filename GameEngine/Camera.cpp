/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "Camera.h"

Camera::Camera(Scene *scene) :
    GameObject(scene, DEFAULT_UI_LAYER),
    m_rasterWidth(0), m_rasterHeight(0), m_viewScale(1.f)
{
    SetName("Camera");

    // Dimensions de la vue dans le référentiel monde
    // La hauteur est calculée pour un ratio 16/9
    float worldW = 24.f;
    float worldH = 9.f / 16.f * worldW;

    SDL_RenderGetLogicalSize(g_renderer, &m_rasterWidth, &m_rasterHeight);

    m_worldView.lowerBound = -0.5f * b2Vec2(worldW, worldH);
    m_worldView.upperBound = +0.5f * b2Vec2(worldW, worldH);
    m_worldBounds.lowerBound = b2Vec2(-worldW, -worldH);
    m_worldBounds.upperBound = b2Vec2(+worldW, +worldH);
    m_worldExtents = m_worldView.GetExtents();
}

Camera::~Camera()
{
}

float Camera::GetWorldToViewScale() const
{
    b2Vec2 viewDim = m_worldView.upperBound - m_worldView.lowerBound;
    return (float)m_rasterWidth / viewDim.x;
}

void Camera::WorldToView(b2Vec2 position, float &x, float &y) const
{
    b2Vec2 viewDim = m_worldView.upperBound - m_worldView.lowerBound;
    float xScale = (float)m_rasterWidth  / viewDim.x;
    float yScale = (float)m_rasterHeight / viewDim.y;
    x = (position.x - m_worldView.lowerBound.x) * xScale;
    y = (position.y - m_worldView.lowerBound.y) * yScale;
    y = (float)m_rasterHeight - y;
}

void Camera::ViewToWorld(float x, float y, b2Vec2 &position) const
{
    y = m_rasterHeight - y;
    float ratioX = x / (float)m_rasterWidth;
    float ratioY = y / (float)m_rasterHeight;
    b2Vec2 viewDim = m_worldView.upperBound - m_worldView.lowerBound;
    position.x = m_worldView.lowerBound.x + ratioX * viewDim.x;
    position.y = m_worldView.lowerBound.y + ratioY * viewDim.y;
}

void Camera::WorldToView(b2Vec2 position, const SDL_Rect *src, float pixPerUnit, SDL_FRect &dst) const
{
    b2Vec2 viewDim = m_worldView.upperBound - m_worldView.lowerBound;
    float xScale = (float)m_rasterWidth  / viewDim.x;
    float yScale = (float)m_rasterHeight / viewDim.y;
    dst.x = (position.x - m_worldView.lowerBound.x) * xScale;
    dst.y = (position.y - m_worldView.lowerBound.y) * yScale;
    dst.y = (float)m_rasterHeight - dst.y;

    if (src)
    {
        dst.w = xScale * (float)src->w / pixPerUnit;
        dst.h = yScale * (float)src->h / pixPerUnit;
    }
}

void Camera::ApplyWorldBounds()
{
    b2Vec2 boundsExtents = m_worldBounds.GetExtents();
    float maxScale = fminf(
        boundsExtents.x / m_worldExtents.x,
        boundsExtents.y / m_worldExtents.y
    );
    if (m_viewScale > maxScale) SetViewScale(maxScale);

    b2Vec2 dispLower = m_worldBounds.lowerBound - m_worldView.lowerBound;
    b2Vec2 dispUpper = m_worldBounds.upperBound - m_worldView.upperBound;
    TranslateWorldView(b2Max(dispLower, b2Vec2_zero));
    TranslateWorldView(b2Min(dispUpper, b2Vec2_zero));
}

void Camera::Shake(int intensity)
{
    // A implémenter dans les classes filles si nécessaire
}
