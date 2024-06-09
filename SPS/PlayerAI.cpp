/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "PlayerAI.h"
#include "StageManager.h"


PlayerAI::PlayerAI(Player *player) :
    m_player(player), m_input(), m_target(nullptr)
{
    m_scene = m_player->m_scene;

    m_stageBox.lowerBound = b2Vec2(-8.0f, 0.0f);
    m_stageBox.upperBound = b2Vec2(+8.0f, 2.0f);
}

PlayerAI::~PlayerAI()
{
}

void PlayerAI::FixedUpdate()
{
    b2Body *body = m_player->GetBody();
    b2Vec2 position = m_player->GetPosition();
    b2Vec2 velocity = m_player->GetVelocity();

    m_input.jumpPressed = false;
    m_input.attackPressed = false;
    m_input.attackDown = false;

    if (IsInDanger())
    {
        ComeBack();
        return;
    }

    ChooseTarget();
    if (m_target == nullptr) return;

    // Déplacement
    m_input.axisX = 0.f;
    b2Vec2 targetPosition = m_target->GetPosition();
    if (targetPosition.x + 2.f < position.x)
    {
        m_input.axisX = -1.f;
    }
    else if (position.x < targetPosition.x - 2.f)
    {
        m_input.axisX = +1.0f;
    }

    // Attaque
    //float targetDist = b2Distance(targetPosition, position);
    //if (targetDist < 3.0f)
    //{
    //    m_input.attackPressed = true;
    //    m_input.attackDown = true;
    //}
}


void PlayerAI::OnStateChanged(Player::State state, Player::State prevState)
{
}

bool PlayerAI::IsInDanger() const
{
    b2Vec2 position = m_player->GetPosition();

    bool inDanger = false;
    if (position.x > m_stageBox.upperBound.x)
    {
        inDanger = true;
    }
    else if (position.x < m_stageBox.lowerBound.x)
    {
        m_player->m_hDirection = 1.0f;
        inDanger = true;
    }
    return inDanger;
}

void PlayerAI::ChooseTarget()
{
    StageManager *stageManager = StageManager::GetFromScene(m_scene);
    m_target = stageManager->GetPlayer(0);
}

void PlayerAI::ComeBack()
{
    b2Vec2 position = m_player->GetPosition();
    b2Vec2 velocity = m_player->GetVelocity();

    if (position.x > m_stageBox.upperBound.x)
    {
        m_input.axisX = -1.0f;
    }
    else if (position.x < m_stageBox.lowerBound.x)
    {
        m_input.axisX = 1.0f;
    }
    else
    {
        m_input.axisX = 0.0f;
    }

    if (velocity.y < 0.0f && position.y < m_stageBox.lowerBound.y + 1.0f)
    {
        m_input.jumpPressed = true;
    }

    if (velocity.y > 0.0f)
    {
        m_input.jumpDown = true;
    }
}
