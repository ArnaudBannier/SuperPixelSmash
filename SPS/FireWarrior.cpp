/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "FireWarrior.h"

FireWarrior::FireWarrior(Scene *scene, const PlayerConfig *config, PlayerStats *stats) :
    Player(scene, config, stats)
{
    SetName("FireWarrior");
    AssetManager *assets = scene->GetAssetManager();

    SpriteSheet *spriteSheet = nullptr;
    SpriteGroup *spriteGroup = nullptr;
    SpriteAnim *anim = nullptr;

    spriteSheet = assets->GetSpriteSheet(SHEET_FIRE_WARRIOR);

    // Animation "Idle"
    spriteGroup = spriteSheet->GetGroup("Idle");
    AssertNew(spriteGroup);
    anim = m_animator.CreateAnimation("Idle", spriteGroup);
    anim->SetCycleCount(-1);
    anim->SetFPS(15.f);

    // TODO : Animation "Run"

    // TODO : Animation "JumpUp"

    // TODO : Animation "JumpTop"

    // TODO : Animation "JumpDown"

    // TODO : Animation "Attack" (c'est fait, c'est cadeau)
    const float attackFPS = 1.0f / (float)ATTACK_FRAME_TIME;
    spriteGroup = spriteSheet->GetGroup("Attack1");
    AssertNew(spriteGroup);
    anim = m_animator.CreateAnimation("Attack1", spriteGroup);
    anim->SetCycleCount(1);
    anim->SetFPS(attackFPS);

    // TODO : Animations Attack2 et Attack3

    // TODO : Anmisation "Defend"

    // TODO : Anmisation "TakeHit"

    // TODO : Décommenter pour lancer l'animation initiale
    m_animator.PlayAnimation("Idle"); 

    // Physique
    m_accAir = 30.f;
    m_accGround = 60.f;
    m_maxSpeed = 50.f; // TODO : adapter

    // Render
    m_renderShift.Set(0.7f, 0.f);
}

FireWarrior::~FireWarrior()
{
}

void FireWarrior::Start()
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
    box.SetAsBox(1.f, 1.f, b2Vec2(0.5f, -0.5), 0.f);

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
    circle.m_p = b2Vec2(0.5f, -0.5f);
    circle.m_radius = 0.15f;

    fixtureDef.shape = &circle;
    fixtureDef.density = 2.f;
    fixtureDef.friction = 0.2f;
    fixtureDef.restitution = 0.f;
    fixtureDef.filter.categoryBits = m_config->teamMask;
    fixtureDef.filter.maskBits = CATEGORY_TERRAIN;

    m_feetFixture = CreateFixture(&fixtureDef);
}

void FireWarrior::OnStateChanged(Player::State state, Player::State prevState)
{
    Player::OnStateChanged(state, prevState);

    switch (state) // TODO : décommenter, compléter
    {
    case State::IDLE:        m_animator.PlayAnimation("Idle");         break;
    //case State::RUN:         m_animator.PlayAnimation("Run");          break;
    case State::ATTACK:      m_animator.PlayAnimation("Attack1");      break;
    // TODO : Gérer d'autres animations
    default:
        break;
    }
}

void FireWarrior::OnAnimationEnd(Animation *which, const std::string &name)
{
    if (m_scene->GetUpdateMode() == Scene::UpdateMode::STEP_BY_STEP && GetPlayerID() == 0)
    {
        std::cout << "[OnAnimationEnd] "
            << "Animation = " << name << std::endl;
    }

    m_autoVelocity = 0.0f;
  
    if (name == "Attack1")
    {
        if (GetPlayerInput().attackDown)
        {
            // TODO : decommenter
            //m_animator.PlayAnimation("Attack2");
        }
        else
        {
            SetState(Player::State::IDLE);
            LockAttack(0.1f);
        }
    }
    
    // TODO : Enchainement des Attack2, Attack3
    //else if (name == "Attack2")
    //{

    //}

}

void FireWarrior::OnFrameChanged(Animation *which, const std::string &name, int frameID)
{
    if (m_scene->GetUpdateMode() == Scene::UpdateMode::STEP_BY_STEP && GetPlayerID() == 0)
    {
        std::cout << "[OnFrameChanged] "
            << "Animation = " << name << ", "
            << "FrameID = " << frameID << std::endl;
    }

    m_autoVelocity = 0.f;

    QueryFilter filter(m_config->otherTeamMask | CATEGORY_ITEM);

    const float lockTime = 0.2f;
    float s = m_facingRight ? 1.f : -1.f;

    if (name == "Attack1")
    {
        switch (frameID)
        {
            // TODO : Vitesse crédible
        case 0: m_autoVelocity = s * 12.0f; break;
        case 1: m_autoVelocity = s * 12.0f; break;
        case 2: m_autoVelocity = s * 12.0f; break;
        default: break;
        }

        if (frameID == 1)
        {
            PlaySFXAttack(SFX_WHOOSH);
        }
        if (frameID == 2)
        {
            // TODO : apdapter le centre de l'attaque
            b2Vec2 position = GetPosition();
            position += b2Vec2(s * 1.0f, 0.0f); 

            Damage damage;
            damage.amount = 3.f;
            // TODO : Verrouillage pour la victime

            damage.lockAttackTime = 10.5f * ATTACK_FRAME_TIME;

            // TODO : adapter la zone d'attaque
            bool hit = AttackCircle(damage, filter, position, 2.f) ;

            //PlaySFXHit(hit, SFX_HIT);
        }
    }
    else if (name == "Attack2")
    {
        // TODO : autoVelocité

        // TODO : déclenchement de l'attaque
        if (frameID == 2)
        {
            PlaySFXAttack(SFX_WHOOSH);

            b2Vec2 position = GetPosition();

            Damage damage;
            damage.amount = 3.f;
            damage.lockTime = lockTime;
            // TODO : paramètres supplémentaires ?

            b2Vec2 vertices[5] = { // C'est cadeau
                position + b2Vec2(s * 0.0f, 2.3f),
                position + b2Vec2(s * 0.0f, 0.0f),
                position + b2Vec2(s * 1.9f, 0.0f),
                position + b2Vec2(s * 1.8f, 1.5f),
                position + b2Vec2(s * 0.9f, 2.3f)
            };
            bool hit = AttackPolygon(damage, filter, vertices, 5);
            //PlaySFXHit(hit, SFX_HIT);
        }
    }
    else if (name == "Attack3")
    {
        // TODO : autoVelocité

        // TODO : déclenchement de l'attaque avec smash
        if (frameID == 2)
        {
            PlaySFXAttack(SFX_WHOOSH);

            b2Vec2 position = GetPosition();
            position += b2Vec2(s * 1.7f, 1.0f);

            Damage damage;
            damage.hasEjection = true;
            // TODO : paramètres supplémentaires

            // TODO : angle d'éjection fonction de la position du joueur
            damage.ejection = b2Vec2(10.0f, 10.0f);

            // TODO : Zone de collision adaptée
        }
    }
    // TODO : D'autres évènement sur frames ?
}
