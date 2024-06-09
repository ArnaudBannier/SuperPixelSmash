/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "BaseSceneManager.h"
#include "DebugCamera.h"

BaseSceneManager::BaseSceneManager(InputManager *inputManager) :
    SceneManager(inputManager), m_camIndex(1)
{
    ApplicationInput::GetFromManager(inputManager)->SetEnabled(true);
    MouseInput::GetFromManager(inputManager)->SetEnabled(true);
    ControlsInput::GetFromManager(inputManager)->SetEnabled(true);
    DebugInput::GetFromManager(inputManager)->SetEnabled(true);

    Scene *scene = GetScene();
    m_cameras[0] = nullptr;
    m_cameras[1] = new DebugCamera(scene);
    scene->SetActiveCamera(m_cameras[1]);
}

BaseSceneManager::~BaseSceneManager()
{
}

void BaseSceneManager::OnSceneUpdate()
{
    Scene *scene = GetScene();
    InputManager *inputManager = scene->GetInputManager();
    DebugInput *debugInput = DebugInput::GetFromManager(inputManager);

    if (debugInput->bodyPressed)
    {
        bool drawPhysics = scene->GetDrawPhysics();
        scene->SetDrawPhysics(!drawPhysics);
    }
    if (debugInput->gizmosPressed)
    {
        bool drawGizmos = scene->GetDrawGizmos();
        scene->SetDrawGizmos(!drawGizmos);
    }
    if (debugInput->gridPressed)
    {
        bool drawGrid = scene->GetDrawGrid();
        scene->SetDrawGrid(!drawGrid);
    }
    if (debugInput->camPressed)
    {
        m_camIndex = (m_camIndex + 1) % m_cameras.size();
        scene->SetActiveCamera(m_cameras[m_camIndex]);
    }
    if (debugInput->infoPressed)
    {
        scene->PrintGameObjects();
    }

    Scene::UpdateMode mode = scene->GetUpdateMode();
    if (mode != Scene::UpdateMode::STEP_BY_STEP && debugInput->nextStepPressed)
    {
        mode = Scene::UpdateMode::STEP_BY_STEP;
        scene->SetUpdateMode(mode);
    }
    else if (debugInput->quitStepPressed)
    {
        mode = Scene::UpdateMode::REALTIME;
        scene->SetUpdateMode(mode);
    }

    // Mode pas-à-pas
    if (mode == Scene::UpdateMode::STEP_BY_STEP)
    {
        if (debugInput->nextStepPressed)
        {
            scene->MakeStep();
        }
    }
}

void BaseSceneManager::SetMainCamera(Camera *camera)
{
    assert(camera);
    GetScene()->SetActiveCamera(camera);
    m_cameras[0] = camera;
    m_camIndex = 0;
}
