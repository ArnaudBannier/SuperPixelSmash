/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "UITitleBackground.h"

UITitleBackground::UITitleBackground(Scene *scene) :
    UIObject(scene)
{
    SetName("UI Title Background");
    SetLayer(LAYER_UI_BACKGROUND);
}

UITitleBackground::~UITitleBackground()
{
}

void UITitleBackground::Update()
{
    UIObject::Update();
    SetVisible(true);
}

void UITitleBackground::Render()
{
    AssetManager *assets = m_scene->GetAssetManager();
    for (SDL_Texture *texture : assets->GetBackgrounds())
    {
        SDL_RenderCopyF(g_renderer, texture, NULL, NULL);
    }
}
