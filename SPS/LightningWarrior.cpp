/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "LightningWarrior.h"

LightningWarrior::LightningWarrior(Scene *scene, const PlayerConfig *config, PlayerStats *stats) :
    Player(scene, config, stats)
{
    SetName("LightningWarrior");
    AssetManager *assets = scene->GetAssetManager();
    SpriteSheet *spriteSheet = nullptr;
    SpriteGroup *spriteGroup = nullptr;
    SpriteAnim *anim = nullptr;

    spriteSheet = assets->GetSpriteSheet(SHEET_LIGHTNING_WARRIOR);

    // Animation "Idle"
    spriteGroup = spriteSheet->GetGroup("Idle");
    AssertNew(spriteGroup);
    anim = m_animator.CreateAnimation("Idle", spriteGroup);
    anim->SetCycleCount(-1);
    anim->SetFPS(15.f);

    // TODO : Toutes les animations

    m_animator.PlayAnimation("Idle");

    // Physique
    m_accAir = 30.f;
    m_accGround = 60.f;
    m_maxSpeed = 8.f;

    // Render
    m_renderShift.Set(0.4f, -0.75f);
}

LightningWarrior::~LightningWarrior()
{
}

void LightningWarrior::Start()
{
    Player::Start();

    b2World &world = m_scene->GetWorld();
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = GetStartPosition();
    bodyDef.fixedRotation = true;
    bodyDef.allowSleep = false;
    bodyDef.bullet = true;

    b2Body *body = CreateBody(&bodyDef);

    // TODO : Modifer les paramètres
    b2PolygonShape box;
    box.SetAsBox(1.f, 1.f, b2Vec2(-0.5f, 0.5), 0.f);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &box;
    fixtureDef.density = 1.f;
    fixtureDef.friction = 0.f;
    fixtureDef.restitution = 0.f;
    fixtureDef.filter.categoryBits = m_config->teamMask;
    fixtureDef.filter.maskBits = m_config->otherTeamMask | CATEGORY_PROJECTILE;

    m_bodyFixture = CreateFixture(&fixtureDef);

    // TODO : Modifer les paramètres
    b2CircleShape circle;
    circle.m_p = b2Vec2(-0.5f, +0.5f);
    circle.m_radius = 0.15f;

    fixtureDef.shape = &circle;
    fixtureDef.density = 2.f;
    fixtureDef.friction = 0.2f;
    fixtureDef.restitution = 0.f;
    fixtureDef.filter.categoryBits = m_config->teamMask;
    fixtureDef.filter.maskBits = CATEGORY_TERRAIN;

    m_feetFixture = CreateFixture(&fixtureDef);
}

void LightningWarrior::OnStateChanged(Player::State state, Player::State prevState)
{
    Player::OnStateChanged(state, prevState);

    switch (state)
    {
    case State::IDLE:        m_animator.PlayAnimation("Idle");         break;
        // TODO : Plein d'états
    default:
        break;
    }
}

void LightningWarrior::OnAnimationEnd(Animation *which, const std::string &name)
{
    if (m_scene->GetUpdateMode() == Scene::UpdateMode::STEP_BY_STEP && GetPlayerID() == 0)
    {
        std::cout << "[OnAnimationEnd] "
            << "Animation = " << name << std::endl;
    }

    m_autoVelocity = 0.0f;

   // TODO : PLein de fin d'animations
}

void LightningWarrior::OnFrameChanged(Animation *which, const std::string &name, int frameID)
{
    if (m_scene->GetUpdateMode() == Scene::UpdateMode::STEP_BY_STEP && GetPlayerID() == 0)
    {
        std::cout << "[OnFrameChanged] "
            << "Animation = " << name << ", "
            << "FrameID = " << frameID << std::endl;
    }

    m_autoVelocity = 0.f;

    QueryFilter filter;
    filter.maskBits = m_config->otherTeamMask | CATEGORY_ITEM;
    filter.solidOnly = true;

    const float lockTime = 0.2f;
    float s = m_facingRight ? 1.f : -1.f;

    // TODO : Plein de changement de frames
}
