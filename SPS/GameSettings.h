/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "../GameEngine/GameEngine.h"

#define HD_WIDTH 1280
#define HD_HEIGHT 720

#define FHD_WIDTH 1920
#define FHD_HEIGHT 1080

#define MAX_PLAYER_COUNT 4

enum Layer : int
{
    LAYER_BACKGROUND,
    LAYER_TERRAIN_BACKGROUND,
    LAYER_TERRAIN,
    LAYER_TERRAIN_FOREGROUND,
    LAYER_PLAYER,
    LAYER_PARTICLES,
    LAYER_FOREGROUND,
    LAYER_UI_BACKGROUND = DEFAULT_UI_LAYER - 1,
    LAYER_UI,
    LAYER_UI_FOREGROUND
};

enum Category : uint16
{
    CATEGORY_TEAM_0     = 1 << 0,
    CATEGORY_TEAM_1     = 1 << 1,
    CATEGORY_TEAM_2     = 1 << 2,
    CATEGORY_TEAM_3     = 1 << 3,
    CATEGORY_PROJECTILE = 1 << 4,
    CATEGORY_TERRAIN    = 1 << 5,
    CATEGORY_ITEM       = 1 << 6,
    CATEGORY_ALL_TEAMS  = CATEGORY_TEAM_0 | CATEGORY_TEAM_1 | CATEGORY_TEAM_2 | CATEGORY_TEAM_3,
    CATEGORY_ALL        = 0xFFFF
};
