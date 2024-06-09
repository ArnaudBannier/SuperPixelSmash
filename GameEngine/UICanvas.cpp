/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "UICanvas.h"

UICanvas::UICanvas(Scene *scene) :
    UIObject(scene)
{
    SetName("UICanvas");
    m_rect.offsetMin.Set(0.f, 0.f);
    m_rect.offsetMax.Set(640.f, 360.f);
    m_rect.anchorMin.Set(0.f, 0.f);
    m_rect.anchorMax.Set(0.f, 0.f);

    int width, height;
    SDL_RenderGetLogicalSize(g_renderer, &width, &height);

    m_pixelPerUnit = (float)width / 640.f;
}

void UICanvas::Update()
{
    int width, height;
    SDL_RenderGetLogicalSize(g_renderer, &width, &height);

    m_pixelPerUnit = (float)width / 640.f;
}
