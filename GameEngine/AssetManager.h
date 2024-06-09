/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "Settings.h"
#include "Common.h"
#include "SpriteSheet.h"
#include "Color.h"

class AssetManager
{
public:
    AssetManager();
    AssetManager(AssetManager const&) = delete;
    AssetManager& operator=(AssetManager const&) = delete;
    ~AssetManager();

    void AddBackgroundLayer(const std::string &path);
    void AddSpriteSheet(int sheetID, const std::string &path);
    void AddFont(int fontID, const std::string &path, int size);
    void AddSound(int soundID, const std::string &path);
    void AddMusic(int musicID, const std::string &path);

    int LoadSpriteSheets(const std::vector<int> &sheetIDs);
    SpriteSheet *GetSpriteSheet(int sheetID);
    TTF_Font *GetFont(int fontID);
    const std::vector<SDL_Texture *> &GetBackgrounds();
    Mix_Chunk *GetSound(int soundID);
    Mix_Music *GetMusic(int musicID);

    void InitSoundFXChannels(int startID, int endID);
    void SetSoundVolume(int soundID, float volume);
    void SetMusicVolume(float volume);
    void SetSoundFXVolume(float volume);
    void PlaySoundFX(int soundID, int loop = 0);
    void PlaySound(int soundID, int channelID, int loops = 0);
    void PlayMusic(int musicID, int loops = -1);
    void FadeInMusic(int musicID, int loops = -1, int ms = 500, double position = 0.0);
    void FadeOutMusic(int ms = 500);

    static void CreateRWops(const std::string &path, SDL_RWops **rwops, void **buffer);
    static void DestroyRWops(SDL_RWops *rwops, void *buffer);

    static void ObfuscateMem(void *memory, size_t size);
    static void RetriveMem(void *memory, size_t size);

private:
    class MusicData
    {
    public:
        MusicData(const std::string &path);
        ~MusicData();

        Mix_Music *GetMusic();

    private:
        std::string m_path;
        Mix_Music *m_music;
        SDL_RWops *m_rwops;
        void *m_rwopsBuffer;
    };

    class FontData
    {
    public:
        FontData(const std::string &path, int size);
        ~FontData();

        TTF_Font *GetFont();

    private:
        std::string m_path;
        TTF_Font *m_font;
        int m_size;
        SDL_RWops *m_rwops;
        void *m_rwopsBuffer;
    };

    class SoundData
    {

    public:
        SoundData(const std::string &path);
        ~SoundData();

        Mix_Chunk *GetSound();

    private:
        Mix_Chunk *m_chunk;
    };

    class SheetData
    {

    public:
        SheetData(const std::string &path);
        ~SheetData();

        SpriteSheet *GetSpriteSheet();

    private:
        SpriteSheet *m_sheet;
        std::string m_path;
    };

    std::vector<SDL_Texture *> m_backgrounds;
    std::map<int, SheetData *> m_sheetMap;
    std::map<int, SoundData *> m_soundMap;
    std::map<int, FontData *>  m_fontMap;
    std::map<int, MusicData *> m_musicMap;

    std::vector<int> m_sfxChannels;
    int m_sfxIndex;
};
