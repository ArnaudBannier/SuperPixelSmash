/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "GameAssets.h"
#include "GameCommon.h"

const Color Colors::Black = Color(0, 0, 0, 255);
const Color Colors::White = Color(255, 255, 255, 255);
const Color Colors::Red = Color(255, 0, 0, 255);
const Color Colors::Green = Color(0, 255, 0, 255);
const Color Colors::Blue = Color(0, 0, 255, 255);
const Color Colors::Yellow = Color(255, 255, 0, 255);
const Color Colors::Magenta = Color(255, 0, 255, 255);
const Color Colors::Cyan = Color(0, 255, 255, 255);

// Ajoutez vos couleurs personnalisées
const Color Colors::Gold = Color(225, 185, 143);
const Color Colors::DarkBlue = Color(10, 10, 25);

void InitAssets(AssetManager *assets)
{
    // Stage
    assets->AddSpriteSheet(
        SHEET_TILESET_ROCKY,
        "../Assets/Background/Tileset_rocky.json");

    // UI
    assets->AddSpriteSheet(
        SHEET_UI,
        "../Assets/Atlas/UI.json");

    // Player
    assets->AddSpriteSheet(
        SHEET_FIRE_WARRIOR,
        "../Assets/Fighter/Fire_warrior.json");
    assets->AddSpriteSheet(
        SHEET_LIGHTNING_WARRIOR,
        "../Assets/Fighter/Lightning_warrior.json");
    assets->AddSpriteSheet(
        SHEET_SHIELD,
        "../Assets/Atlas/Shield.json");

    // Item
    assets->AddSpriteSheet(
        SHEET_ITEM_POTION,
        "../Assets/Atlas/Potion.json");

    // Effects
    assets->AddSpriteSheet(
        SHEET_VFX_DUST,
        "../Assets/Atlas/VFX_dust.json");
    assets->AddSpriteSheet(
        SHEET_VFX_IMPACT_DUST,
        "../Assets/Atlas/VFX_impact_dust.json");
    assets->AddSpriteSheet(
        SHEET_VFX_HIT,
        "../Assets/Atlas/VFX_hit.json");
    assets->AddSpriteSheet(
        SHEET_VFX_PARTICLES,
        "../Assets/Atlas/VFX_particles.json");
}

void InitBackground(AssetManager * assets, int type)
{
    if (true)// Ajoutez des conditions si vous avez plusieurs fonds
    {
        const int layerCount = 3;
        for (int i = 0; i < layerCount; ++i)
        {
            std::string path = "../Assets/Background/Background_rocky_"
                + std::to_string(i) + ".dat";
            assets->AddBackgroundLayer(path);
        }
    }
}

void InitFonts(AssetManager * assets)
{
    assets->AddFont(
        FONT_SMALL,
        "../Assets/Font/FutilePro.dat",
        32
    );
    assets->AddFont(
        FONT_LARGE,
        "../Assets/Font/FutilePro.dat",
        48
    );
    assets->AddFont(
        FONT_TEXT,
        "../Assets/Font/m5x7.dat",
        48
    );
    assets->AddFont(
        FONT_NORMAL,
        "../Assets/Font/m5x7.dat",
        48
    );
    assets->AddFont(
        FONT_TIME,
        "../Assets/Font/FutilePro.dat",
        64
    );
}

void InitSFX(AssetManager * assets)
{
    assets->InitSoundFXChannels(0, 14);

    // Player
    assets->AddSound(
        SFX_HIT,
        "../Assets/SoundFX/Player/Hit.dat");
    assets->AddSound(
        SFX_WHOOSH,
        "../Assets/SoundFX/Player/Whoosh.dat");
    assets->AddSound(
        SFX_LAND,
        "../Assets/SoundFX/Player/Dirt_land.dat");
    assets->AddSound(
        SFX_JUMP_GROUND,
        "../Assets/SoundFX/Player/Dirt_jump.dat");

    // UI
    assets->AddSound(
        SFX_UI_CLICK,
        "../Assets/SoundFX/UI/DM-CGS-21.dat");
    assets->AddSound(
        SFX_UI_ITEM,
        "../Assets/SoundFX/UI/DM-CGS-40.dat");
    assets->AddSound(
        SFX_UI_SELECTED,
        "../Assets/SoundFX/UI/DM-CGS-01.dat");
    assets->AddSound(
        SFX_UI_VALIDATE,
        "../Assets/SoundFX/UI/DM-CGS-32.dat");
    assets->AddSound(
        SFX_UI_BACK,
        "../Assets/SoundFX/UI/DM-CGS-16.dat");
    assets->AddSound(
        SFX_UI_PAUSE,
        "../Assets/SoundFX/UI/DM-CGS-16.dat");
    assets->AddSound(
        SFX_UI_UNPAUSE,
        "../Assets/SoundFX/UI/DM-CGS-16.dat");
}

void InitMusic(AssetManager * assets)
{
    assets->AddMusic(
        MUSIC_MENU,
        "../Assets/Music/TomMusic_Call_to_Adventure.dat"
    );
    assets->AddMusic(
        MUSIC_ROCKY_PASS,
        "../Assets/Music/TomMusic_The_Wilderness.dat"
    );
}
