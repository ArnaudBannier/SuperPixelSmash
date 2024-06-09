/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "DebugCamera.h"

DebugCamera::DebugCamera(Scene *scene) :
    Camera(scene), m_mode(Mode::ZOOM), m_circleRadius(1.f),
    m_boxHalfExtents(1.f, 1.f), m_boxOrientation(0.f), m_boxMode(0), m_gizmosCenter(0.f, 0.f),
    m_mousePos(0.f, 0.f)
{
    SetName("DebugCamera");
}

void DebugCamera::Update()
{
    if (m_scene->GetActiveCamera() != this)
        return;

    SetVisible(true);

    InputManager *inputManager = m_scene->GetInputManager();
    MouseInput *mouse = MouseInput::GetFromManager(inputManager);

    b2Vec2 mouseDelta;
    ViewToWorld(mouse->viewPos.x, mouse->viewPos.y, m_mousePos);
    ViewToWorld(mouse->lastViewPos.x, mouse->lastViewPos.y, mouseDelta);
    mouseDelta -= m_mousePos;

    m_gizmosCenter.x = 0.1f * roundf(10.f * m_mousePos.x);
    m_gizmosCenter.y = 0.1f * roundf(10.f * m_mousePos.y);

    if (mouse->rightClickCount > 0)
    {
        switch (m_mode)
        {
        case Mode::ZOOM:
            m_mode = Mode::DRAW_CIRCLE;
            break;
        case Mode::DRAW_CIRCLE:
            m_mode = Mode::DRAW_BOX;
            break;
        case Mode::DRAW_BOX:
            m_mode = Mode::ZOOM;
            break;
        default:
            break;
        }
    }

    switch (m_mode)
    {
    case Mode::ZOOM:
    {
        if (mouse->middleClick)
        {
            m_viewScale = 1.f;
            SetViewScale(m_viewScale);
        }
        else if (mouse->wheel > 0)
        {
            m_viewScale *= 0.8f;
            m_viewScale = Math::Clamp(m_viewScale, 0.05f, 3.f);
            SetViewScale(m_viewScale);
        }
        else if (mouse->wheel < 0)
        {
            m_viewScale /= 0.8f;
            m_viewScale = Math::Clamp(m_viewScale, 0.05f, 3.f);
            SetViewScale(m_viewScale);
        }

        if (mouse->leftClickCount > 1)
        {
            std::cout << std::fixed << std::setprecision(1);
            std::cout << "[DebugCamera, Point] Center = ("
                << m_gizmosCenter.x << ", "
                << m_gizmosCenter.y << ")" << std::endl;
        }
        break;
    }
    case Mode::DRAW_CIRCLE:
    {
        m_circleRadius += mouse->wheel * 0.05f;
        m_circleRadius = Math::Clamp(m_circleRadius, 0.1f, 5.f);

        if (mouse->leftClickCount > 1)
        {
            std::cout << std::fixed << std::setprecision(1);
            std::cout << "[DebugCamera, Circle] Center = ("
                << m_gizmosCenter.x << ", "
                << m_gizmosCenter.y << "), ";
            std::cout << "Radius = "
                << m_circleRadius << std::endl;
        }
        break;
    }
    case Mode::DRAW_BOX:
    {
        if (mouse->middleClick)
        {
            m_boxMode = (m_boxMode + 1) % 3;
        }

        switch (m_boxMode)
        {
        case 0:
            m_boxHalfExtents.x += mouse->wheel * 0.05f;
            break;
        case 1:
            m_boxHalfExtents.y += mouse->wheel * 0.05f;
            break;
        case 2:
            m_boxOrientation += mouse->wheel * 5.f;
            break;
        default:
            break;
        }

        m_boxHalfExtents.x = Math::Clamp(m_boxHalfExtents.x, 0.1f, 5.f);
        m_boxHalfExtents.y = Math::Clamp(m_boxHalfExtents.y, 0.1f, 5.f);

        if (mouse->leftClickCount > 1)
        {
            std::cout << std::fixed << std::setprecision(1);
            std::cout << "[DebugCamera, Box] Center = ("
                << m_gizmosCenter.x << ", "
                << m_gizmosCenter.y << "), ";
            std::cout << "Half extents = ("
                << m_boxHalfExtents.x << ", "
                << m_boxHalfExtents.y << "), ";
            std::cout << "Angle = "
                << m_boxOrientation <<  std::endl;
        }
        break;
    }
    default:
        break;
    }

    if (mouse->leftDown)
    {
        TranslateWorldView(mouseDelta);
    }
}

void DebugCamera::Render()
{
    Gizmos &gizmos = m_scene->GetGizmos();
    gizmos.SetColor(Colors::White);

    if (m_mode == Mode::DRAW_CIRCLE)
    {
        gizmos.DrawCircle(m_gizmosCenter, m_circleRadius);
    }
    else if (m_mode == Mode::DRAW_BOX)
    {
        gizmos.DrawBox(m_gizmosCenter, m_boxHalfExtents, m_boxOrientation);
    }
}
