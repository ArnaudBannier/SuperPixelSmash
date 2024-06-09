/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "Settings.h"
#include "Camera.h"
#include "GameObject.h"
#include "SpriteSheet.h"
#include "SpriteAnim.h"
#include "LerpAnim.h"

class Particle;

class ParticleSystem : public GameObject
{
public:
    ParticleSystem(Scene *scene, int layer);
    virtual ~ParticleSystem();

    Particle *EmitParticle(
        SpriteGroup *spriteGroup, const b2Vec2 &position, float pixPerUnit);

    virtual void Update() override;
    virtual void Render() override;

    int GetParticleCount() const;

protected:
    std::set<Particle *> m_particles;
};

inline int ParticleSystem::GetParticleCount() const
{
    return (int)m_particles.size();
}

class Particle
{
public:
    Particle(SpriteGroup *spriteGroup, const b2Vec2 &position, float pixPerUnit);
    virtual ~Particle();

    virtual bool Update(float dt);
    virtual void Render(SDL_Renderer *renderer, Camera *camera);

    void SetLifetimeFromAnim();
    void SetLifetime(float lifetime);
    void SetFlip(SDL_RendererFlip flip);
    void SetOpacity(float opacity);
    void SetAnchor(Anchor anchor);
    void SetVelocity(const b2Vec2 &velocity);
    void SetGravity(const b2Vec2 &gravity);
    void SetDamping(float damping);
    void SetDamping(const b2Vec2 &damping);
    void SetBlendMode(SDL_BlendMode blendMode);
    void SetAngularVelocity(float angularVelocityDeg);

    LerpAnim<float> *CreateAlphaAnimation(float value0, float value1);
    LerpAnim<float> *CreateScaleAnimation(float value0, float value1);

    SpriteAnim *GetSpriteAnimation();
    LerpAnim<float> *GetAlphaAnimation() const;
    LerpAnim<float> *GetScaleAnimation() const;

protected:
    float m_lifetime;
    float m_remainingLifetime;
    float m_startSize;
    float m_opacity;
    float m_angle;
    float m_angularVelocity;
    float m_pixPerUnit;
    bool m_started;

    SpriteAnim m_spriteAnim;
    LerpAnim<float> *m_scaleAnim;
    LerpAnim<float> *m_alphaAnim;
    Anchor m_anchor;

    SDL_RendererFlip m_flip;
    SDL_BlendMode m_blendMode;

    b2Vec2 m_velocity;
    b2Vec2 m_position;
    b2Vec2 m_gravity;
    b2Vec2 m_damping;
};

inline void Particle::SetLifetimeFromAnim()
{
    SetLifetime(m_spriteAnim.GetTotalTime());
}

inline void Particle::SetLifetime(float lifetime)
{
    m_remainingLifetime = lifetime;
    m_lifetime = lifetime;
}

inline SpriteAnim *Particle::GetSpriteAnimation()
{
    return &m_spriteAnim;
}

inline LerpAnim<float> *Particle::GetAlphaAnimation() const
{
    return m_alphaAnim;
}

inline LerpAnim<float> *Particle::GetScaleAnimation() const
{
    return m_scaleAnim;
}

inline void Particle::SetFlip(SDL_RendererFlip flip)
{
    m_flip = flip;
}

inline void Particle::SetOpacity(float opacity)
{
    m_opacity = opacity;
}

inline void Particle::SetAnchor(Anchor anchor)
{
    m_anchor = anchor;
}

inline void Particle::SetVelocity(const b2Vec2 &velocity)
{
    m_velocity = velocity;
}

inline void Particle::SetGravity(const b2Vec2 &gravity)
{
    m_gravity = gravity;
}

inline void Particle::SetDamping(float damping)
{
    m_damping.Set(damping, damping);
}

inline void Particle::SetDamping(const b2Vec2 &damping)
{
    m_damping = damping;
}

inline void Particle::SetBlendMode(SDL_BlendMode blendMode)
{
    m_blendMode = blendMode;
}

inline void Particle::SetAngularVelocity(float angularVelocityDeg)
{
    m_angularVelocity = angularVelocityDeg;
}
