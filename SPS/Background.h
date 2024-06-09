/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "GameSettings.h"
#include "GameCommon.h"

class Background : public GameObject
{
public:
    Background(Scene *scene, int layer);
    virtual ~Background();

    enum class RenderMode
    {
        NO_FILL, FILL_ABOVE, FILL_BELOW, FILL_VERTICAL
    };

    virtual void Update() override;
    virtual void Render() override;

    void SetPixelsPerUnit(float pixelsPerUnit);
    void SetWorldDimensions(const b2Vec2 &dimensions);
    void SetWorldHeight(float height);
    b2Vec2 GetWorldDimensions() const;

    void AddLayer(SDL_Texture *texture, b2Vec2 shiftFactor, RenderMode mode = RenderMode::NO_FILL);
    void SetWorldCenter(b2Vec2 center);

private:
    b2Vec2 m_worldDim;
    b2Vec2 m_worldCenter;
    std::vector<SDL_Texture *> m_layers;
    std::vector<b2Vec2> m_shiftFactors;
    std::vector<RenderMode> m_layerModes;

    void FillLayerAbove(SDL_Texture *layer, SDL_FRect dstRect);
    void FillLayerBelow(SDL_Texture *layer, SDL_FRect dstRect);
};

inline void Background::SetWorldDimensions(const b2Vec2 &dimensions)
{
    m_worldDim = dimensions;
}

inline void Background::SetWorldHeight(float height)
{
    int w, h;
    SDL_QueryTexture(m_layers[0], nullptr, nullptr, &w, &h);
    m_worldDim.Set(height * (float)w / (float)h, height);
}

inline b2Vec2 Background::GetWorldDimensions() const
{
    return m_worldDim;
}

inline void Background::SetWorldCenter(b2Vec2 center)
{
    m_worldCenter = center;
}
