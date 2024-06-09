/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "GameSettings.h"
#include "GameCommon.h"

class BaseSceneManager : public SceneManager
{
public:
    BaseSceneManager(InputManager *inputManager);
    virtual ~BaseSceneManager();

    virtual void OnSceneUpdate() override;

protected:
    void SetMainCamera(Camera *camera);

private:
    std::array<Camera *, 2> m_cameras;
    int m_camIndex;
};
