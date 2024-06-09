/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "ParticleSystem.h"
#include "Utils.h"
#include "Camera.h"

Particle::Particle(SpriteGroup *spriteGroup, const b2Vec2 &position, float pixPerUnit) :
    m_spriteAnim("ParticleAnim", spriteGroup), m_alphaAnim(nullptr), m_scaleAnim(nullptr),
    m_angle(0.f), m_angularVelocity(0.f), m_opacity(1.f), m_startSize(1.f),
    m_lifetime(0.f), m_remainingLifetime(1.f),
    m_position(position), m_velocity(b2Vec2_zero),
    m_pixPerUnit(pixPerUnit), m_flip(SDL_FLIP_NONE), m_anchor(Anchor::CENTER), m_started(false),
    m_gravity(b2Vec2_zero), m_damping(b2Vec2_zero), m_blendMode(SDL_BLENDMODE_BLEND)
{
}

Particle::~Particle()
{
    if (m_alphaAnim) delete m_alphaAnim;
    if (m_scaleAnim) delete m_scaleAnim;
}

bool Particle::Update(float dt)
{
    if (m_started == false)
    {
        m_spriteAnim.Play();
        m_started = true;
    }

    m_spriteAnim.Update(dt);
    if (m_spriteAnim.IsDelayed()) return true;

    m_remainingLifetime -= dt;
    if (m_remainingLifetime < 0.f) return false;

    m_velocity += dt * m_gravity;

    m_velocity.x *= 1.0f / (1.0f + dt * m_damping.x);
    m_velocity.y *= 1.0f / (1.0f + dt * m_damping.y);

    m_position += dt * m_velocity;

    m_angle += dt * m_angularVelocity;
    
    if (m_scaleAnim) m_scaleAnim->Update(dt);
    if (m_alphaAnim) m_alphaAnim->Update(dt);

    return true;
}

void Particle::Render(SDL_Renderer *renderer, Camera *camera)
{
    SDL_Texture *texture = m_spriteAnim.GetTexture();
    if ((texture == nullptr) || m_spriteAnim.IsDelayed() || m_spriteAnim.IsStopped()) return;

    const SDL_Rect *src = m_spriteAnim.GetSourceRect();
    SDL_FRect rect = { 0 };
    camera->WorldToView(m_position, src, m_pixPerUnit, rect);

    float alpha = 255.f * m_opacity;
    if (m_alphaAnim)
    {
        alpha *= m_alphaAnim->GetValue();
    }

    alpha = Math::Clamp(alpha, 0.f, 255.f);
    int code = SDL_SetTextureAlphaMod(texture, (Uint8)alpha);
    assert(code >= 0);
    code = SDL_SetTextureBlendMode(texture, m_blendMode);
    assert(code >= 0);

    float scale = 1.f;
    if (m_scaleAnim) scale *= m_scaleAnim->GetValue();
    rect.w *= scale;
    rect.h *= scale;

    RenderCopyExF(
        renderer, texture, src, &rect, 
        m_anchor, m_angle, b2Vec2(0.5f, 0.5f), m_flip
    );
}

LerpAnim<float> *Particle::CreateAlphaAnimation(float value0, float value1)
{
    if (m_alphaAnim) delete m_alphaAnim;
    m_alphaAnim = new LerpAnim<float>("AlphaAnim", value0, value1);
    m_alphaAnim->SetCycleTime(m_lifetime);
    m_alphaAnim->SetCycleCount(1);
    m_alphaAnim->Play();
    return m_alphaAnim;
}

LerpAnim<float> *Particle::CreateScaleAnimation(float value0, float value1)
{
    if (m_scaleAnim) delete m_scaleAnim;
    m_scaleAnim = new LerpAnim<float>("ScaleAnim", value0, value1);
    m_scaleAnim->SetCycleTime(m_lifetime);
    m_scaleAnim->SetCycleCount(1);
    m_scaleAnim->Play();
    return m_scaleAnim;
}

ParticleSystem::ParticleSystem(Scene *scene, int layer) :
    GameObject(scene, layer), m_particles()
{
    SetName("ParticleSystem of layer " + std::to_string((int)layer));
}

ParticleSystem::~ParticleSystem()
{
    for (auto particle : m_particles)
    {
        delete particle;
    }
    m_particles.clear();
}

Particle *ParticleSystem::EmitParticle(SpriteGroup *spriteGroup, const b2Vec2 &position, float pixPerUnit)
{
    Particle *particle = new Particle(spriteGroup, position, pixPerUnit);
    m_particles.insert(particle);
    return particle;
}

void ParticleSystem::Update()
{
    float dt = m_scene->GetTime().GetDelta();
    std::list<Particle *> toDelete;

    SetVisible(true);

    for (auto particle : m_particles)
    {
        bool isAlive = particle->Update(dt);
        if (isAlive == false)
        {
            toDelete.push_front(particle);
        }
    }

    for (auto &particle : toDelete)
    {
        m_particles.erase(particle);
        delete particle;
    }
}

void ParticleSystem::Render()
{
    Camera *camera = m_scene->GetActiveCamera();
    for (auto particle : m_particles)
    {
        particle->Render(g_renderer, camera);
    }
}
