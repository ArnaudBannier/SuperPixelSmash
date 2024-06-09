/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "SpriteSheet.h"
#include "Settings.h"
#include "AssetManager.h"
#include <cstdio>

void Memcpy(
    void *destination, long long destinationSize,
    const void *source, long long sourceSize)
{
#ifdef _WIN32
    memcpy_s(destination, destinationSize, source, sourceSize);
#else
    memcpy(destination, source, sourceSize);
#endif
}

char *Parser_GetDir(const char *path)
{
    size_t length = strlen(path);
    size_t pos = 0;
    for (int i = (int)length - 1; i > 0; --i)
    {
        if (path[i] == '\\' || path[i] == '/')
        {
            pos = i;
            break;
        }
    }

    char *dir = (char *)calloc(pos + 1, sizeof(char));
    AssertNew(dir);

    Memcpy(dir, pos + 1, path, pos);

    dir[pos] = '\0';
    return dir;
}

char *Parser_MakePath(const char *dir, const char *fname)
{
    size_t count1 = strlen(dir);
    size_t count2 = strlen(fname);
    char *path = (char *)calloc(count1 + count2 + 2, sizeof(char));
    AssertNew(path);

    Memcpy(path, count1 + count2 + 2, dir, count1);
    path[count1] = '/';
    Memcpy(path + count1 + 1, count2 + 1, fname, count2);
    path[count1 + count2 + 1] = '\0';

    return path;
}


void SpriteSheet::LoadRect(cJSON *jRect, int i)
{
    SDL_Rect *rect = m_rects + i;
    cJSON *jTmp = NULL;

    jTmp = cJSON_GetObjectItem(jRect, "x");
    if (cJSON_IsNumber(jTmp))
    {
        rect->x = jTmp->valueint;
    }

    jTmp = cJSON_GetObjectItem(jRect, "y");
    if (cJSON_IsNumber(jTmp))
    {
        rect->y = jTmp->valueint;
    }

    jTmp = cJSON_GetObjectItem(jRect, "w");
    if (cJSON_IsNumber(jTmp))
    {
        rect->w = jTmp->valueint;
    }

    jTmp = cJSON_GetObjectItem(jRect, "h");
    if (cJSON_IsNumber(jTmp))
    {
        rect->h = jTmp->valueint;
    }
}

void SpriteSheet::LoadPart(cJSON *jPart, int i)
{
    SpriteGroup *part = new SpriteGroup(*this);
    m_groups[i] = part;
    cJSON *jTmp = NULL;

    jTmp = cJSON_GetObjectItem(jPart, "name");
    if (cJSON_IsString(jTmp))
    {
        part->m_name.assign(cJSON_GetStringValue(jTmp));
    }

    jTmp = cJSON_GetObjectItem(jPart, "frames");
    if (cJSON_IsArray(jTmp))
    {
        int frameCount = cJSON_GetArraySize(jTmp);
        part->m_spriteCount = frameCount;
        part->m_spriteIndices = (int *)calloc(frameCount, sizeof(int));
        AssertNew(part->m_spriteIndices);

        int i = 0;
        cJSON *jIdx = NULL;
        cJSON_ArrayForEach(jIdx, jTmp)
        {
            if (cJSON_IsNumber(jIdx))
            {
                (part->m_spriteIndices)[i] = jIdx->valueint;
            }
            i++;
        }
    }
}

void SpriteSheet::LoadGeometry(cJSON *jGeo)
{
    cJSON *jRow = cJSON_GetObjectItem(jGeo, "rowCount");
    cJSON *jCol = cJSON_GetObjectItem(jGeo, "colCount");
    cJSON *jWidth = cJSON_GetObjectItem(jGeo, "width");
    cJSON *jHeight = cJSON_GetObjectItem(jGeo, "height");
    cJSON *jOffset = cJSON_GetObjectItem(jGeo, "padding");
    cJSON *jBorder = cJSON_GetObjectItem(jGeo, "spacing");
    cJSON *jOffsetX = cJSON_GetObjectItem(jGeo, "paddingX");
    cJSON *jOffsetY = cJSON_GetObjectItem(jGeo, "paddingY");
    cJSON *jBorderX = cJSON_GetObjectItem(jGeo, "spacingX");
    cJSON *jBorderY = cJSON_GetObjectItem(jGeo, "spacingY");

    if (cJSON_IsNumber(jRow) && cJSON_IsNumber(jCol)
        && cJSON_IsNumber(jWidth) && cJSON_IsNumber(jHeight))
    {
        int rowCount = jRow->valueint;
        int colCount = jCol->valueint;
        int w = jWidth->valueint;
        int h = jHeight->valueint;
        int offsetX = cJSON_IsNumber(jOffsetX) ? jOffsetX->valueint : 0;
        int offsetY = cJSON_IsNumber(jOffsetY) ? jOffsetY->valueint : 0;
        int borderX = cJSON_IsNumber(jBorderX) ? jBorderX->valueint : 0;
        int borderY = cJSON_IsNumber(jBorderY) ? jBorderY->valueint : 0;

        if (cJSON_IsNumber(jOffset))
        {
            offsetX = jOffset->valueint;
            offsetY = jOffset->valueint;
        }
        if (cJSON_IsNumber(jBorder))
        {
            borderX = jBorder->valueint;
            borderY = jBorder->valueint;
        }

        int rectCount = rowCount * colCount;

        m_rectCount = rectCount;
        m_rects = (SDL_Rect *)calloc(rectCount, sizeof(SDL_Rect));
        AssertNew(m_rects);

        for (int i = 0; i < rowCount; ++i)
        {
            for (int j = 0; j < colCount; ++j)
            {
                SDL_Rect *rect = &(m_rects[i * colCount + j]);
                rect->x = offsetX + j * (w + borderX);
                rect->y = offsetY + i * (h + borderY);
                rect->w = w;
                rect->h = h;
            }
        }
    }
}

char *SpriteSheet::ParseJSON(cJSON *json)
{
    char *fname = NULL;

    cJSON *jTex = cJSON_GetObjectItem(json, "texture");
    if (cJSON_IsString(jTex))
    {
        fname = cJSON_GetStringValue(jTex);
    }

    cJSON *jRects = cJSON_GetObjectItem(json, "rectangles");

    if (cJSON_IsArray(jRects))
    {
        int rectCount = cJSON_GetArraySize(jRects);
        m_rectCount = rectCount;
        m_rects = (SDL_Rect *)calloc(rectCount, sizeof(SDL_Rect));
        AssertNew(m_rects);

        int i = 0;
        cJSON *jRect = NULL;
        cJSON_ArrayForEach(jRect, jRects)
        {
            LoadRect(jRect, i++);
        }
    }
    else
    {
        cJSON *jGeo = cJSON_GetObjectItem(json, "geometry");

        if (jGeo)
        {
            LoadGeometry(jGeo);
        }
        else
        {
            assert(false);
        }

    }

    cJSON *jParts = cJSON_GetObjectItem(json, "parts");

    if (cJSON_IsArray(jParts))
    {
        int partCount = cJSON_GetArraySize(jParts);
        m_groupCount = partCount;
        m_groups = (SpriteGroup **)calloc(partCount, sizeof(SpriteGroup *));
        AssertNew(m_groups);

        int i = 0;
        cJSON *jPart = NULL;
        cJSON_ArrayForEach(jPart, jParts)
        {
            LoadPart(jPart, i++);
        }
    }

    return fname;
}

SpriteSheet::SpriteSheet(SDL_Renderer *renderer, const std::string &path) :
    m_renderer(renderer)
{
    // Lit le fichier et récupère le contenu dans un buffer
    FILE *file = fopen(path.c_str(), "rb");
    AssertNew(file);

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);

    char *buffer = (char *)calloc(fileSize, sizeof(char));
    AssertNew(buffer);

    rewind(file);
    fread(buffer, 1, fileSize, file);

    fclose(file);

    // Parse le buffer et crée une structure json
    cJSON *json = cJSON_ParseWithLength(buffer, fileSize);
    AssertNew(json);

    // Parse la structure json
    char *fname = ParseJSON(json);

    char *dir = Parser_GetDir(path.c_str());
    char *texPath = Parser_MakePath(dir, fname);

    void *rwopsBuffer = NULL;
    SDL_RWops *rwops = NULL;
    AssetManager::CreateRWops(std::string(texPath), &rwops, &rwopsBuffer);

    SDL_Texture *texture = IMG_LoadTexture_RW(renderer, rwops, 0);
    if (!texture)
    {
        printf("ERROR - Loading texture %s\n", texPath);
        printf("      - %s\n", SDL_GetError());
        assert(false);
        abort();
    }
    m_texture = texture;
    free(texPath);
    free(dir);

    AssetManager::DestroyRWops(rwops, rwopsBuffer);

    // Libère la mémoire temporaire
    cJSON_Delete(json);
    free(buffer);
}

SpriteSheet::~SpriteSheet()
{
    if (m_texture)
    {
        SDL_DestroyTexture(m_texture);
    }

    if (m_groups)
    {
        for (int i = 0; i < m_groupCount; ++i)
        {
            delete m_groups[i];
        }
        free(m_groups);
    }

    free(m_rects);
}


SpriteGroup *SpriteSheet::GetGroup(const std::string &name)
{
    for (int i = 0; i < m_groupCount; ++i)
    {
        if (name == m_groups[i]->m_name)
        {
            return m_groups[i];
        }
    }
    return nullptr;
}

SpriteGroup *SpriteSheet::GetGroup(int i)
{
    assert(0 <= i && i < m_groupCount);
    return m_groups[i];
}

int SpriteSheet::GetGroupCount() const
{
    return m_groupCount;
}

const std::string &SpriteGroup::GetName() const
{
    return m_name;
}

SpriteGroup::SpriteGroup(SpriteSheet &spriteSheet) :
    m_spriteSheet(spriteSheet), m_spriteCount(0), m_spriteIndices(nullptr), m_name("")
{}

SpriteGroup::~SpriteGroup()
{
    if (m_spriteIndices) free(m_spriteIndices);
}
