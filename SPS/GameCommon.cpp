/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "GameCommon.h"

PlayerConfig::PlayerConfig() :
    type(Type::FIRE_WARRIOR), skinID(0), playerID(-1), teamID(0),
    enabled(true), isCPU(false),
    teamMask(CATEGORY_TEAM_0),
    otherTeamMask(CATEGORY_ALL_TEAMS & (~CATEGORY_TEAM_0))
{
}

void PlayerConfig::SetTeamID(int teamIDIn)
{
    teamID = teamIDIn;
    switch (teamID)
    {
    default:
    case 0: teamMask = CATEGORY_TEAM_0; break;
    case 1: teamMask = CATEGORY_TEAM_1; break;
    case 2: teamMask = CATEGORY_TEAM_2; break;
    case 3: teamMask = CATEGORY_TEAM_3; break;
    }

    otherTeamMask = CATEGORY_ALL_TEAMS & (~teamMask);
}

StageConfig::StageConfig() :
    type(Type::ROCKY_PASS), mode(Mode::LIMITED_TIME),
    duration(3), lifeCount(3)
    // TODO : initialiser le membre de fréquence d'apparition des potions
{
}

void InitInputConfig(InputManager *inputManager)
{
    ApplicationInput *applicationInput = new ApplicationInput();
    MouseInput *mouseInput = new MouseInput();
    ControlsInput *controlsInput = new ControlsInput();
    DebugInput *debugInput = new DebugInput();

    inputManager->AddInputGroup(
        (int)InputID::APPLICATION, applicationInput
    );
    inputManager->AddInputGroup(
        (int)InputID::MOUSE, mouseInput
    );
    inputManager->AddInputGroup(
        (int)InputID::CONTROLS, controlsInput
    );
    inputManager->AddInputGroup(
        (int)InputID::DEBUG, debugInput
    );
}

PlayerStats::PlayerStats() :
    fallCount(0), koCount(0), lifeCount(3),
    damageGiven(0.f), damageTaken(0.f), maxSpeed(0.f)
{
}
