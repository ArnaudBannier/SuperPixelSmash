/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "MainCamera.h"
#include "StageManager.h"

MainCamera::MainCamera(Scene *scene) :
    Camera(scene),
    m_center(b2Vec2_zero), m_target(b2Vec2_zero),
    m_velocity(b2Vec2_zero)
{
    SetName("MainCamera");

    m_playersBox.lowerBound = b2Vec2_zero;
    m_playersBox.upperBound = b2Vec2_zero;

    m_worldBounds.lowerBound = b2Vec2(-17.f, -5.0f);
    m_worldBounds.upperBound = b2Vec2(+17.f, +13.f);

    m_minWidth = 18.f;
    m_maxWidth = 26.f;
}

MainCamera::~MainCamera()
{
}

void MainCamera::Update()
{
    Camera::Update();
    SetVisible(true);

    UpdatePlayersBox();

    m_target = m_playersBox.GetCenter();

    b2Vec2 center = m_target + b2Vec2(0.f, 1.5f);

    const float smoothTimeX = 0.2f;
    const float smoothTimeY = 0.4f;
    m_center.x = Math::SmoothDamp(
        m_center.x, center.x, m_velocity.x,
        smoothTimeX, 100.f, m_scene->GetDelta()
    );
    m_center.y = Math::SmoothDamp(
        m_center.y, center.y, m_velocity.y,
        smoothTimeY, 100.f, m_scene->GetDelta()
    );

    float width = (m_minWidth + m_maxWidth) / 2.f;
    ScaleViewToFitWidth(width);

    b2Vec2 shift = m_center - m_worldView.GetCenter();
    TranslateWorldView(shift);

    ApplyWorldBounds();
}

void MainCamera::DrawGizmos(Gizmos &gizmos)
{
    //gizmos.SetColor(Colors::Magenta);
    //gizmos.DrawAABB(m_playersBox);
    //
    //gizmos.SetColor(Colors::Red);
    //gizmos.DrawPoint(m_center);
}

void MainCamera::Shake(int intensity)
{
}

void MainCamera::UpdatePlayersBox()
{
    StageManager *stageManager = StageManager::GetFromScene(m_scene);
    AssertNew(stageManager);

    const int playerCount = stageManager->GetPlayerCount();
    int i = 0;
    for (; i < playerCount; i++)
    {
        Player *player = stageManager->GetPlayer(i);
        if (player->IsEnabled() == false) continue;

        m_playersBox = player->GetAABB();
        break;
    }
    for (; i < playerCount; i++)
    {
        Player *player = stageManager->GetPlayer(i);
        if (player->IsEnabled() == false) continue;

        m_playersBox.Combine(player->GetAABB());
    }

    m_playersBox.lowerBound = b2Max(
        m_playersBox.lowerBound,
        m_worldBounds.lowerBound + b2Vec2(3.f, 0.f)
    );
    m_playersBox.upperBound = b2Min(
        m_playersBox.upperBound,
        m_worldBounds.upperBound - b2Vec2(3.f, 0.f)
    );
}
