/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "SceneManager.h"

SceneManager::SceneManager(InputManager *inputManager) :
    m_scene(this, inputManager),
    m_shouldQuitScene(false), m_shouldQuitGame(false),
    m_willQuitScene(false), m_willQuitGame(false)
{
    m_fillFader = new UIFillRect(&m_scene, Color(0, 0, 0));
    m_fillFader->SetName("UI SceneManager Fill Fader");
    m_fillFader->SetLayer(0x7FFFFFFF);
    m_fillFader->AddAnimListener(this);

    UIFadeDef fadeDef;
    fadeDef.fadeOpacity = true;
    fadeDef.duration = 1.0f;
    fadeDef.easing = EasingFct_Out;
    m_fillFader->SetFadeInAnimation(fadeDef);
    m_fillFader->SetFadeOutAnimation(fadeDef);

    m_fillFader->PlayFadeOut();
}

SceneManager::~SceneManager()
{
}

void SceneManager::OnSceneUpdate()
{
}

void SceneManager::QuitScene()
{
    if (m_fillFader->IsFadingIn() || m_willQuitScene) return;

    m_fillFader->PlayFadeIn();
    m_willQuitScene = true;
    Mix_FadeOutMusic(1000);
}

void SceneManager::QuitGame()
{
    if (m_fillFader->IsFadingIn() || m_willQuitScene) return;

    m_fillFader->PlayFadeIn();
    m_willQuitScene = true;
    m_willQuitGame = true;
    Mix_FadeOutMusic(1000);
}

void SceneManager::OnFadeInEnd(UIObject *which)
{
    if (which != m_fillFader) return;

    m_shouldQuitScene = m_willQuitScene;
    m_shouldQuitGame = m_willQuitGame;
}
