/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "AssetManager.h"
#include "Common.h"
#include "Utils.h"

AssetManager::AssetManager() :
    m_sheetMap(), m_fontMap(), m_soundMap(), m_musicMap(),
    m_backgrounds(), m_sfxChannels(), 
    m_sfxIndex(0)
{
    for (int i = 4; i < 8; i++)
    {
        m_sfxChannels.push_back(i);
    }
}

AssetManager::~AssetManager()
{
    for (SDL_Texture *texture : m_backgrounds)
    {
        if (texture) SDL_DestroyTexture(texture);
    }
    for (auto it = m_sheetMap.begin(); it != m_sheetMap.end(); ++it)
    {
        SheetData *sheetData = it->second;
        if (sheetData) delete sheetData;
    }
    for (auto it = m_soundMap.begin(); it != m_soundMap.end(); ++it)
    {
        SoundData *soundData = it->second;
        if (soundData) delete soundData;
    }
    for (auto it = m_musicMap.begin(); it != m_musicMap.end(); ++it)
    {
        MusicData *musicData = it->second;
        if (musicData) delete musicData;
    }
    for (auto it = m_fontMap.begin(); it != m_fontMap.end(); ++it)
    {
        FontData *fontData = it->second;
        if (fontData) delete fontData;
    }
}

void AssetManager::AddBackgroundLayer(const std::string &path)
{
    void *rwopsBuffer = NULL;
    SDL_RWops *rwops = NULL;
    CreateRWops(path, &rwops, &rwopsBuffer);

    SDL_Texture *texture = IMG_LoadTexture_RW(g_renderer, rwops, 0);
    if (texture == NULL)
    {
        std::cout
            << "ERROR - Load texture " << path << std::endl
            << "      - " << IMG_GetError() << std::endl;
        assert(false);
        abort();
    }
    m_backgrounds.push_back(texture);

    DestroyRWops(rwops, rwopsBuffer);
}

void AssetManager::AddSpriteSheet(int sheetID, const std::string &path)
{
    auto it = m_sheetMap.find(sheetID);
    if (it != m_sheetMap.end())
    {
        std::cout
            << "ERROR - Add sprite sheet " << path << std::endl
            << "      - The sprite sheet already exists" << std::endl;
        return;
    }

    m_sheetMap.insert(std::make_pair(sheetID, new SheetData(path)));
}

void AssetManager::AddFont(int fontID, const std::string &path, int size)
{
    auto it = m_fontMap.find(fontID);
    if (it != m_fontMap.end())
    {
        std::cout
            << "ERROR - Add font " << path << std::endl
            << "      - The font already exists" << std::endl;
        return;
    }

    m_fontMap.insert(std::make_pair(fontID, new FontData(path, size)));
}

void AssetManager::AddSound(int soundID, const std::string &path)
{
    auto it = m_soundMap.find(soundID);
    if (it != m_soundMap.end())
    {
        std::cout
            << "ERROR - Add sound " << path << std::endl
            << "      - The sound already exists" << std::endl;
        return;
    }

    m_soundMap.insert(std::make_pair(soundID, new SoundData(path)));
}

void AssetManager::AddMusic(int musicID, const std::string &path)
{    
    auto it = m_musicMap.find(musicID);
    if (it != m_musicMap.end())
    {
        std::cout
            << "ERROR - Add music " << path << std::endl
            << "      - The music already exists" << std::endl;
        return;
    }

    m_musicMap.insert(std::make_pair(musicID, new MusicData(path)));
}

int AssetManager::LoadSpriteSheets(const std::vector<int> &sheetIDs)
{
    int exitStatus = EXIT_SUCCESS;
    for (int sheetID : sheetIDs)
    {
        SpriteSheet *spriteSheet = GetSpriteSheet(sheetID);
        if (spriteSheet == nullptr) exitStatus = EXIT_FAILURE;
    }
    return exitStatus;
}

SpriteSheet *AssetManager::GetSpriteSheet(int sheetID)
{
    auto it = m_sheetMap.find(sheetID);
    if (it != m_sheetMap.end())
    {
        return it->second->GetSpriteSheet();
    }
    return nullptr;
}

TTF_Font *AssetManager::GetFont(int fontID)
{
    auto it = m_fontMap.find(fontID);
    if (it != m_fontMap.end())
    {
        return it->second->GetFont();
    }
    return nullptr;
}

const std::vector<SDL_Texture *> &AssetManager::GetBackgrounds()
{
    return m_backgrounds;
}

Mix_Chunk *AssetManager::GetSound(int soundID)
{
    auto it = m_soundMap.find(soundID);
    if (it != m_soundMap.end())
    {
        return it->second->GetSound();
    }
    return nullptr;
}

Mix_Music *AssetManager::GetMusic(int musicID)
{
    auto it = m_musicMap.find(musicID);
    if (it != m_musicMap.end())
    {
        return it->second->GetMusic();
    }
    return nullptr;
}

void AssetManager::InitSoundFXChannels(int startID, int endID)
{
    m_sfxChannels.clear();
    for (int i = startID; i <= endID; i++)
    {
        m_sfxChannels.push_back(i);
    }
    m_sfxIndex = 0;
}

void AssetManager::SetSoundVolume(int soundID, float volume)
{
    volume = Math::Clamp(volume, 0.f, 1.f);
    Mix_Chunk *chunk = GetSound(soundID);
    if (chunk == nullptr)
    {
        assert(false);
        return;
    }

    Mix_VolumeChunk(chunk, (int)(volume * MIX_MAX_VOLUME));
}

void AssetManager::SetMusicVolume(float volume)
{
    volume = Math::Clamp(volume, 0.f, 1.f);
    Mix_VolumeMusic((int)(volume * MIX_MAX_VOLUME));
}

void AssetManager::SetSoundFXVolume(float volume)
{
    volume = Math::Clamp(volume, 0.f, 1.f);
    int v = (int)(volume * MIX_MAX_VOLUME);
    for (int channel : m_sfxChannels)
    {
        Mix_Volume(channel, v);
    }
}

void AssetManager::PlaySoundFX(int soundID, int loop)
{
    assert(m_sfxChannels.size() > 0);
    assert(m_sfxIndex < m_sfxChannels.size());

    Mix_Chunk *chunk = GetSound(soundID);
    if (chunk == nullptr)
    {
        assert(false);
        return;
    }

    int channelID = m_sfxChannels[m_sfxIndex];
    int exitStatus = Mix_PlayChannel(channelID, chunk, loop);

    m_sfxIndex = ((size_t)m_sfxIndex + 1) % m_sfxChannels.size();
}

void AssetManager::PlaySound(int soundID, int channelID, int loops)
{
    Mix_Chunk *chunk = GetSound(soundID);
    if (chunk == nullptr)
    {
        assert(false);
        return;
    }

    int exitStatus = Mix_PlayChannel(channelID, chunk, loops);
}

void AssetManager::PlayMusic(int musicID, int loops)
{
    Mix_Music *music = GetMusic(musicID);
    if (music == nullptr)
    {
        assert(false);
        return;
    }

    int exitStatus = Mix_PlayMusic(music, loops);
}

void AssetManager::FadeInMusic(int musicID, int loops, int ms, double position)
{
    Mix_Music *music = GetMusic(musicID);
    if (music == nullptr)
    {
        assert(false);
        return;
    }

    int exitStatus = Mix_FadeInMusicPos(music, loops, ms, position);
}

void AssetManager::FadeOutMusic(int ms)
{
    Mix_FadeOutMusic(ms);
}

void AssetManager::CreateRWops(const std::string &path, SDL_RWops **rwops, void **buffer)
{
    FILE *file = fopen(path.c_str(), "rb");
    if (file == nullptr)
    {
        std::cout << "ERROR - The file " << path << " cannot be opened" << std::endl;
        assert(false);
        abort();
    }

    fseek(file, 0, SEEK_END);
    size_t size = (size_t)ftell(file);
    rewind(file);
    uint8_t *mem = (uint8_t *)calloc(size, sizeof(uint8_t));
    AssertNew(mem);

    *buffer = (void *)mem;

    size_t freadCount = fread(mem, 1, size, file);
    assert(freadCount == size);
    fclose(file); file = nullptr;

    if (size > 2 && mem[0] == (uint8_t)0x0B && mem[1] == (uint8_t)0xF7)
    {
        // Utilisation du magic number 0x0BF7 pour les fichiers obsfusqués
        mem += 2;
        size -= 2;
        RetriveMem((void *)mem, size);
    }

    *rwops = SDL_RWFromConstMem(mem, (int)size);

    if (*rwops == nullptr)
    {
        std::cout
            << "ERROR - Load RWops " << path << std::endl
            << "      - " << SDL_GetError() << std::endl;
        assert(false);
        abort();
    }
}

void AssetManager::DestroyRWops(SDL_RWops *rwops, void *buffer)
{
    if (rwops) SDL_RWclose(rwops);
    free(buffer);
}

void AssetManager::ObfuscateMem(void *memory, size_t size)
{
    Uint8 *buffer = (Uint8 *)memory;
    buffer[0] ^= 0x73;
    buffer[0] = 0xBB * buffer[0] + 0xC9;
    for (size_t i = 1; i < size; i++)
    {
        buffer[i] ^= buffer[i - 1];
        buffer[i] = 0xBB * buffer[i] + 0xC9;
    }
}

void AssetManager::RetriveMem(void *memory, size_t size)
{
    Uint8 *buffer = (Uint8 *)memory;
    for (size_t i = size - 1; i > 0; i--)
    {
        buffer[i] = 0x73 * (buffer[i] + 0x37);
        buffer[i] ^= buffer[i - 1];
    }
    buffer[0] = 0x73 * (buffer[0] + 0x37);
    buffer[0] ^= 0x73;
}

AssetManager::MusicData::MusicData(const std::string &path) :
    m_path(path), m_music(nullptr), m_rwops(nullptr), m_rwopsBuffer(nullptr)
{
}

AssetManager::MusicData::~MusicData()
{
    if (m_music) Mix_FreeMusic(m_music);
    DestroyRWops(m_rwops, m_rwopsBuffer);
}

Mix_Music *AssetManager::MusicData::GetMusic()
{
    if (m_music != nullptr) return m_music;

    CreateRWops(m_path, &m_rwops, &m_rwopsBuffer);
    m_music = Mix_LoadMUS_RW(m_rwops, 0);
    if (m_music == NULL)
    {
        std::cout
            << "ERROR - Load music " << m_path << std::endl
            << "      - " << Mix_GetError() << std::endl;
        assert(false);
        abort();
    }

    return m_music;
}

AssetManager::FontData::FontData(const std::string &path, int size) :
    m_path(path), m_font(nullptr), m_size(size), m_rwops(nullptr), m_rwopsBuffer(nullptr)
{
    CreateRWops(m_path, &m_rwops, &m_rwopsBuffer);
    m_font = TTF_OpenFontRW(m_rwops, 0, size);
    if (m_font == NULL)
    {
        std::cout
            << "ERROR - Open font " << path << std::endl
            << "      - " << TTF_GetError() << std::endl;
        assert(false);
        abort();
    }
}

AssetManager::FontData::~FontData()
{
    if (m_font) TTF_CloseFont(m_font);
    DestroyRWops(m_rwops, m_rwopsBuffer);
}

TTF_Font *AssetManager::FontData::GetFont()
{
    return m_font;
}

AssetManager::SoundData::SoundData(const std::string &path) :
    m_chunk(nullptr)
{
    void *rwopsBuffer = nullptr;
    SDL_RWops *rwops = nullptr;
    CreateRWops(path, &rwops, &rwopsBuffer);

    m_chunk = Mix_LoadWAV_RW(rwops, 0);
    if (m_chunk == NULL)
    {
        std::cout
            << "ERROR - Loading audio " << path << std::endl
            << "      - " << Mix_GetError() << std::endl;
        assert(false);
        abort();
    }

    DestroyRWops(rwops, rwopsBuffer);
}

AssetManager::SoundData::~SoundData()
{
    if (m_chunk) Mix_FreeChunk(m_chunk);
}

Mix_Chunk *AssetManager::SoundData::GetSound()
{
    return m_chunk;
}

AssetManager::SheetData::SheetData(const std::string &path) :
    m_path(path), m_sheet(nullptr)
{
}

AssetManager::SheetData::~SheetData()
{
    if (m_sheet) delete m_sheet;
}

SpriteSheet *AssetManager::SheetData::GetSpriteSheet()
{
    if (m_sheet) return m_sheet;

    m_sheet = new SpriteSheet(g_renderer, m_path);
    return m_sheet;
}
