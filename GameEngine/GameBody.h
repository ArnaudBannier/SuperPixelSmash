/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "Settings.h"
#include "GameObject.h"

class GameCollision;

class GameBody : public GameObject
{
public:
    GameBody(Scene *scene, int layer);
    virtual ~GameBody();

    virtual void OnDisable() override;
    virtual void OnEnable() override;

    virtual void OnCollisionEnter(GameCollision &collision);
    virtual void OnCollisionStay(GameCollision &collision);
    virtual void OnCollisionExit(GameCollision &collision);

    static GameBody *GetFromBody(b2Body *body);

    b2Body *CreateBody(b2BodyDef *bodyDef);
    b2Fixture *CreateFixture(b2FixtureDef *fixtureDef);
    b2Body *GetBody();

    void SetStartPosition(const b2Vec2 &position);
    void SetStartPosition(float x, float y);
    b2Vec2 GetStartPosition() const;

    void DeleteBody();
    void SetBodyEnabled(bool bodyEnabled);
    bool IsBodyEnabled() const;

    b2Vec2 GetPosition() const;
    b2Vec2 GetInterpolatedPosition() const;
    b2Vec2 GetVelocity() const;
    b2Transform GetTransform() const;
    b2Transform GetInterpolatedTransform() const;
    b2AABB GetAABB() const;

    void SetDebugColor(Color color);
    Color GetDebugColor() const;

    void UpdateInterpolation();

protected:
    Color m_debugColor;

private:
    friend class Scene;
    
    b2Body *m_body;
    b2Vec2 m_startPos;
    b2Transform m_lastXf;
    b2Transform m_currXf;
};

inline b2Body *GameBody::GetBody()
{
    return m_body;
}

inline void GameBody::SetStartPosition(const b2Vec2 &position)
{
    m_startPos = position;
}

inline void GameBody::SetStartPosition(float x, float y)
{
    m_startPos.Set(x, y);
}

inline b2Vec2 GameBody::GetStartPosition() const
{
    return m_startPos;
}

inline void GameBody::SetBodyEnabled(bool bodyEnabled)
{
    if (m_body)
    {
        m_body->SetEnabled(bodyEnabled);
    }
}

inline bool GameBody::IsBodyEnabled() const
{
    if (m_body) return m_body->IsEnabled();
    return false;
}

inline void GameBody::OnDisable()
{
    SetBodyEnabled(false);
}

inline void GameBody::OnEnable()
{
    SetBodyEnabled(true);
}

inline void GameBody::SetDebugColor(Color color)
{
    m_debugColor = color;
}

inline Color GameBody::GetDebugColor() const
{
    return m_debugColor;
}

inline void GameBody::UpdateInterpolation()
{
    m_lastXf = m_currXf;
    if (m_body)
    {
        m_currXf = m_body->GetTransform();
    }
}
