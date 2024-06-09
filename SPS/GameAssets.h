/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "GameSettings.h"

void InitAssets(AssetManager *assets);
void InitBackground(AssetManager *assets, int type);
void InitFonts(AssetManager *assets);
void InitSFX(AssetManager *assets);
void InitMusic(AssetManager *assets);

class Colors 
{
public:
    // Basic colors
    static const Color Black;
    static const Color White;
    static const Color Red;
    static const Color Green;
    static const Color Blue;
    static const Color Yellow;
    static const Color Magenta;
    static const Color Cyan;

    // Game colors
    static const Color Gold;
    static const Color DarkBlue;
};

enum SoundID : int
{
    SFX_HIT, SFX_WHOOSH,
    SFX_LAND, SFX_JUMP_GROUND,

    SFX_UI_SELECTED, SFX_UI_CLICK, SFX_UI_ITEM,
    SFX_UI_BACK, SFX_UI_VALIDATE,
    SFX_UI_PAUSE, SFX_UI_UNPAUSE
};

enum MusicID : int
{
    MUSIC_MENU, MUSIC_ROCKY_PASS
};

enum SheetID : int
{
    SHEET_FIRE_WARRIOR,
    SHEET_LIGHTNING_WARRIOR,
    SHEET_SHIELD,

    SHEET_TILESET_ROCKY,
    SHEET_UI,
    SHEET_ITEM_POTION,

    SHEET_VFX_HIT,
    SHEET_VFX_IMPACT_DUST,
    SHEET_VFX_PARTICLES,
    SHEET_VFX_DUST,
};

enum FontID : int
{
    FONT_SMALL,
    FONT_NORMAL,
    FONT_LARGE,

    FONT_DAMAGE = FONT_LARGE,
    FONT_TEXT,
    FONT_TIME,
};
