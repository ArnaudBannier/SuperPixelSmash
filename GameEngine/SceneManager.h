/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "Settings.h"
#include "Common.h"
#include "Scene.h"
#include "UIFillRect.h"

class SceneManager : public UIAnimListener
{
public:
    SceneManager(InputManager *inputManager);
    virtual ~SceneManager();

    virtual void OnSceneUpdate();
    virtual void OnSceneFixedUpdate() {}
    virtual void OnSceneRender() {}

    Scene *GetScene()
    {
        return &m_scene;
    }

    bool ShouldQuitScene() const
    {
        return m_shouldQuitScene;
    }

    bool ShouldQuitGame() const
    {
        return m_shouldQuitGame;
    }

    void QuitScene();
    void QuitGame();

    // Callbacks
    virtual void OnFadeInEnd(UIObject *which) override;

private:
    Scene m_scene;

    bool m_willQuitScene;
    bool m_willQuitGame;

    bool m_shouldQuitScene;
    bool m_shouldQuitGame;
    UIFillRect *m_fillFader;
};
