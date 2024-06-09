/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "UIFillRect.h"

UIFillRect::UIFillRect(Scene *scene, Color color) :
    UIObject(scene)
{
    SetName("UIFillRect");
    SetColor(color);
}

void UIFillRect::Update()
{
    UIObject::Update();
    SetVisible(true);
}

void UIFillRect::Render()
{
    if (IsUIEnabled() == false) return;

    float alpha = GetAlpha();
    SDL_FRect rect = GetRenderRect();
    Color color = GetColor();
    SDL_SetRenderDrawColor(
        g_renderer, color.r, color.g, color.b,
        (Uint8)(color.a * alpha)
    );
    SDL_RenderFillRectF(g_renderer, &rect);
}
