/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "Background.h"
#include "DebugCamera.h"

Background::Background(Scene *scene, int layer) :
    GameObject(scene, layer),
    m_layers(), m_layerModes(), m_shiftFactors(),
    m_worldDim(b2Vec2(1.f, 1.f)), m_worldCenter(b2Vec2_zero)
{
    SetName("Background");
}

Background::~Background()
{
}

void Background::Update()
{
    SetVisible(true);
}

void Background::Render()
{
    Camera *camera = m_scene->GetActiveCamera();
    AssertNew(camera);

    b2Vec2 viewCenter = camera->GetWorldView().GetCenter();

    // Dimension du fond dans le référentiel monde
    float scale = camera->GetWorldToViewScale();
    float layerW = scale * m_worldDim.x;
    float layerH = scale * m_worldDim.y;

    int logicalW, logicalH;
    SDL_RenderGetLogicalSize(g_renderer, &logicalW, &logicalH);

    // Dessine les différents calques du fond (parallax)
    for (int i = 0; i < m_layers.size(); ++i)
    {
        SDL_Texture *layer = m_layers[i];
        if (layer == nullptr) continue;

        b2Vec2 shiftFactor = m_shiftFactors[i];
        RenderMode mode = m_layerModes[i];

        b2Vec2 layerWest = viewCenter - m_worldCenter;
        layerWest.x *= (1.f - shiftFactor.x);
        layerWest.y *= (1.f - shiftFactor.y);
        layerWest += m_worldCenter;

        float x, y;
        camera->WorldToView(layerWest, x, y);
        x = fmodf(x, layerW);

        for (x = x - layerW; x < (float)logicalW; x += layerW)
        {
            SDL_FRect dstRect = { 0 };
            dstRect.x = x;
            dstRect.y = y;
            dstRect.w = layerW;
            dstRect.h = layerH;

            switch (mode)
            {
            default:
            case Background::RenderMode::NO_FILL:
                break;
            case Background::RenderMode::FILL_ABOVE:
                FillLayerAbove(layer, dstRect);
                break;
            case Background::RenderMode::FILL_BELOW:
                FillLayerBelow(layer, dstRect);
                break;
            case Background::RenderMode::FILL_VERTICAL:
                FillLayerAbove(layer, dstRect);
                FillLayerBelow(layer, dstRect);
                break;
            }
            RenderCopyF(g_renderer, layer, NULL, &dstRect, Anchor::WEST);
        }
    }
}

void Background::AddLayer(SDL_Texture *texture, b2Vec2 shiftFactor, RenderMode mode)
{
    m_layerModes.push_back(mode);
    m_layers.push_back(texture);
    m_shiftFactors.push_back(shiftFactor);
}

inline void Background::FillLayerAbove(SDL_Texture *layer, SDL_FRect dstRect)
{
    int textureW, textureH;
    SDL_QueryTexture(layer, nullptr, nullptr, &textureW, &textureH);

    SDL_Rect srcRect = { 0 };
    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = textureW;
    srcRect.h = 1;

    dstRect.h = dstRect.y - 0.5f * dstRect.h;
    dstRect.y = 0.f;

    if (dstRect.h > 0.f)
    {
        SDL_RenderCopyF(g_renderer, layer, &srcRect, &dstRect);
    }
}

inline void Background::FillLayerBelow(SDL_Texture *layer, SDL_FRect dstRect)
{
    int textureW, textureH;
    SDL_QueryTexture(layer, nullptr, nullptr, &textureW, &textureH);

    int logicalW, logicalH;
    SDL_RenderGetLogicalSize(g_renderer, &logicalW, &logicalH);

    SDL_Rect srcRect = { 0 };
    srcRect.x = 0;
    srcRect.y = textureH - 1;
    srcRect.w = textureW;
    srcRect.h = 1;

    dstRect.y += 0.5f * dstRect.h;
    dstRect.h = logicalH - dstRect.y;

    if (dstRect.h > 0.f)
    {
        SDL_RenderCopyF(g_renderer, layer, &srcRect, &dstRect);
    }
}

void Background::SetPixelsPerUnit(float pixelsPerUnit)
{
    if (m_layers.empty() || m_layers[0] == nullptr)
        return;

    int w, h;
    SDL_QueryTexture(m_layers[0], nullptr, nullptr, &w, &h);
    m_worldDim.Set((float)w / pixelsPerUnit, (float)h / pixelsPerUnit);
}
