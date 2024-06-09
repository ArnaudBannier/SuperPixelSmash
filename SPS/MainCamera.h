/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "GameSettings.h"
#include "GameCommon.h"

class MainCamera : public Camera
{
public:
    MainCamera(Scene *scene);
    virtual ~MainCamera();

    virtual void Update() override;
    virtual void DrawGizmos(Gizmos &gizmos) override;
    virtual void Shake(int intensity) override;

protected:
    b2Vec2 m_center;
    b2Vec2 m_target;
    b2Vec2 m_velocity;
    b2AABB m_playersBox;

    float m_minWidth;
    float m_maxWidth;

    void UpdatePlayersBox();
};
