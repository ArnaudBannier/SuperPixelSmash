/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "Terrain.h"
#include "Player.h"

Terrain::Terrain(Scene *scene, int layer) :
    GameBody(scene, layer), m_tiles(), m_scale(1.f), m_isOneWay(false)
{
    SetName("Terrain");

    // Couleur des colliders en debug
    m_debugColor.Set(255, 200, 0);
}

Terrain::~Terrain()
{
}

void Terrain::SetScale(float scale)
{
    m_scale = scale;
    for (Tile &tile : m_tiles)
    {
        tile.position *= m_scale;
        tile.pixelsPerUnit /= m_scale;
    }
}

void Terrain::ScaleTiles(float scale)
{
    for (Tile &tile : m_tiles)
    {
        tile.position *= scale;
        tile.pixelsPerUnit /= scale;
    }
}

void Terrain::Render()
{
    Camera *camera = m_scene->GetActiveCamera();
    b2Body *body = GetBody();

    if (body == nullptr) return;

    const float angle = body->GetAngle() / b2_pi * 180.f;
    for (Tile &tile : m_tiles)
    {
        if (tile.texture == nullptr) continue;

        b2Vec2 position = body->GetWorldPoint(tile.position);
        SDL_FRect dstRect = { 0 };
        camera->WorldToView(position, &tile.srcRect, tile.pixelsPerUnit, dstRect);

        RenderCopyExF(
            g_renderer, tile.texture, &tile.srcRect, &dstRect, tile.anchor,
            tile.angle - angle, tile.center, tile.flip
        );
    }
}

void Terrain::Update()
{
    GameBody::Update();
    SetVisible(true);
}

void Terrain::AddTile(const Tile &tile)
{
    m_tiles.push_back(tile);
    Tile &back = m_tiles.back();
    back.position *= m_scale;
    back.pixelsPerUnit /= m_scale;
}

Tile::Tile() :
    m_spriteGroup(nullptr), position(b2Vec2_zero),
    flip(SDL_FLIP_NONE), center(0.0f, 0.0f), angle(0.f), anchor(Anchor::NORTH_WEST),
    texture(nullptr), pixelsPerUnit(1.f)
{
    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = 1;
    srcRect.h = 1;
}

void Tile::Reset(float pixelsPerUnitIn)
{
    m_spriteGroup = nullptr;
    texture = nullptr;
    pixelsPerUnit = pixelsPerUnitIn;
    position = b2Vec2_zero;
    flip = SDL_FLIP_NONE;
    center.Set(0.0f, 1.0f);
    angle = 0.f;
    anchor = Anchor::NORTH_WEST;

    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = 1;
    srcRect.h = 1;
}

void Tile::SetSprite(SpriteGroup *spriteGroup, int spriteIdx)
{
    texture = spriteGroup->GetTexture();
    const SDL_Rect *rect = spriteGroup->GetSourceRect(spriteIdx);
    if (rect) srcRect = *rect;
}
