/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "Settings.h"
#include "GameObject.h"

class Camera : public GameObject
{
public:
    Camera(Scene *scene);
    virtual ~Camera();

    b2AABB GetWorldView() const;
    void SetWorldView(const b2AABB &worldView);
    void TranslateWorldView(const b2Vec2 &displacement);

    b2AABB GetWorldBounds() const;
    void SetWorldBounds(const b2AABB &bounds);

    void ScaleViewToFitWidth(float width);
    void SetViewScale(float scale);
    float GetViewScale() const;

    float GetViewWidth() const;
    float GetViewHeight() const;
    float GetUnscaledViewWidth() const;
    float GetUnscaledViewHeight() const;

    int GetRasterWidth() const;
    int GetRasterHeight() const;

    float GetWorldToViewScale() const;

    void WorldToView(b2Vec2 position, float &x, float &y) const;
    void ViewToWorld(float x, float y, b2Vec2 &position) const;

    void WorldToView(b2Vec2 position, const SDL_Rect *src, float pixPerUnit, SDL_FRect &dst) const;

    void ApplyWorldBounds();

    virtual void Shake(int intensity);


protected:
    /// @brief Rectangle représentant la vue de la caméra.
    /// Ces coordonnées sont dans le référentiel monde.
    b2AABB m_worldView;
    b2Vec2 m_worldExtents;
    float m_viewScale;

    b2AABB m_worldBounds;

private:
    /// @brief Largeur en pixels de la caméra.
    int m_rasterWidth;

    /// @brief Hauteur en pixels de la caméra.
    int m_rasterHeight;
};

inline b2AABB Camera::GetWorldView() const
{
    return m_worldView;
}

inline void Camera::SetWorldView(const b2AABB &worldView)
{
    m_worldView = worldView;
    m_worldExtents = worldView.GetExtents();
    m_viewScale = 1.f;
}

inline void Camera::TranslateWorldView(const b2Vec2 &displacement)
{
    m_worldView.lowerBound += displacement;
    m_worldView.upperBound += displacement;
}

inline b2AABB Camera::GetWorldBounds() const
{
    return m_worldBounds;
}

inline void Camera::SetWorldBounds(const b2AABB &bounds)
{
    m_worldBounds = bounds;
}

inline void Camera::ScaleViewToFitWidth(float width)
{
    SetViewScale(0.5f * width / m_worldExtents.x);
}

inline void Camera::SetViewScale(float scale)
{
    m_viewScale = scale;
    b2Vec2 center = m_worldView.GetCenter();
    m_worldView.lowerBound = center - m_viewScale * m_worldExtents;
    m_worldView.upperBound = center + m_viewScale * m_worldExtents;
}

inline float Camera::GetViewScale() const
{
    return m_viewScale;
}

inline float Camera::GetViewWidth() const
{
    return m_worldView.upperBound.x - m_worldView.lowerBound.x;
}

inline float Camera::GetViewHeight() const
{
    return m_worldView.upperBound.y - m_worldView.lowerBound.y;
}

inline float Camera::GetUnscaledViewWidth() const
{
    return 2.f * m_worldExtents.x;
}

inline float Camera::GetUnscaledViewHeight() const
{
    return 2.f * m_worldExtents.y;
}

inline int Camera::GetRasterWidth() const
{
    return m_rasterWidth;
}

inline int Camera::GetRasterHeight() const
{
    return m_rasterHeight;
}
