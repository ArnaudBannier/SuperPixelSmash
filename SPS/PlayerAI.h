/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "GameSettings.h"
#include "GameCommon.h"
#include "Player.h"
#include "ControlsInput.h"

class PlayerAI
{
public:
    PlayerAI(Player *player);
    virtual ~PlayerAI();

    virtual void FixedUpdate();

    const PlayerInput &GetInput() const;


    void OnStateChanged(Player::State state, Player::State prevState);

protected:
    Scene *m_scene;
    PlayerInput m_input;
    Player *m_player;
    Player *m_target;
    b2AABB m_stageBox;


    bool IsInDanger() const;
    void ChooseTarget();
    void ComeBack();
};

inline const PlayerInput &PlayerAI::GetInput() const
{
    return m_input;
}