/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "GameSettings.h"
#include "GameCommon.h"

struct Tile
{
    Tile();

    void Reset(float pixelsPerUnit = 1.f);

    void SetSprite(SpriteGroup *spriteGroup, int spriteIdx);

    float pixelsPerUnit;
    SDL_Texture *texture;
    SDL_Rect srcRect;

    SpriteGroup *m_spriteGroup;
    b2Vec2 position;
    float angle;
    SDL_RendererFlip flip;
    b2Vec2 center;
    Anchor anchor;
};

class Terrain : public GameBody
{
public:
    Terrain(Scene *scene, int layer);
    virtual ~Terrain();

    void SetScale(float scale);
    float GetScale() const;
    void ScaleTiles(float scale);
    void SetOneWay(bool isOneWay);
    bool IsOneWay() const;

    virtual void Render() override;
    virtual void Update() override;

    void AddTile(const Tile &tile);

protected:
    std::vector<Tile> m_tiles;

private:
    float m_scale;
    bool m_isOneWay;
};

inline float Terrain::GetScale() const
{
    return m_scale;
}

inline void Terrain::SetOneWay(bool isOneWay)
{
    m_isOneWay = isOneWay;
}

inline bool Terrain::IsOneWay() const
{
    return m_isOneWay;
}
