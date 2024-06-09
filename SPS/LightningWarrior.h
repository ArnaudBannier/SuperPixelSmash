/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "Player.h"

class LightningWarrior : public Player
{
public:
    LightningWarrior(Scene *scene, const PlayerConfig *config, PlayerStats *stats);
    virtual ~LightningWarrior();

    virtual void Start() override;
    virtual void OnStateChanged(Player::State state, Player::State prevState) override;

    virtual void OnAnimationEnd(Animation *which, const std::string &name) override;
    virtual void OnFrameChanged(Animation *which, const std::string &name, int frameID) override;
};