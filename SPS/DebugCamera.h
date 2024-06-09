/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "GameSettings.h"
#include "GameCommon.h"

class DebugCamera : public Camera
{
public:
    DebugCamera(Scene *scene);

    virtual void Update() override;
    virtual void Render() override;

protected:

    enum class Mode
    {
        ZOOM, DRAW_CIRCLE, DRAW_BOX
    };
    Mode m_mode;
    b2Vec2 m_mousePos;
    b2Vec2 m_gizmosCenter;
    float m_circleRadius;
    b2Vec2 m_boxHalfExtents;
    float m_boxOrientation;
    int m_boxMode;
};
