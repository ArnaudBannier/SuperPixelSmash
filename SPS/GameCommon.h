/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "GameSettings.h"
#include "GameAssets.h"

#include "ApplicationInput.h"
#include "MouseInput.h"
#include "ControlsInput.h"
#include "DebugInput.h"

#include "../GameEngine/Damage.h"

#define MAX_PLAYER_COUNT 4

enum class InputID : int
{
    APPLICATION, MOUSE, CONTROLS, DEBUG
};

void InitInputConfig(InputManager *inputManager);

class PlayerConfig
{
public:
    PlayerConfig();

    enum class Type : int
    {
        FIRE_WARRIOR = 0, LIGHTNING_WARRIOR
    };

    void SetTeamID(int teamID);

    bool isCPU;
    bool enabled;
    int playerID;
    Type type;
    int skinID;

    int teamID;
    uint16 teamMask;
    uint16 otherTeamMask;
};

class StageConfig
{
public:
    StageConfig();

    enum class Type : int
    {
        ROCKY_PASS = 0
    };
    enum class Mode : int
    {
        LIMITED_LIVES = 0, LIMITED_TIME
    };

    Type type;
    Mode mode;
    // TODO : Ajouter un membre pour gérer la fréquence d'apparition des potions

    int duration;
    int lifeCount;
    
};

class PlayerStats
{
public:
    PlayerStats();

    int lifeCount;
    int fallCount;
    int koCount;
    float damageGiven;
    float damageTaken;
    float maxSpeed;
};
