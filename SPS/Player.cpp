/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "Player.h"
#include "Platform.h"
#include "PlayerAI.h"
#include "StageManager.h"

const float ATTACK_FRAME_TIME = 0.04f;

Player::Player(Scene *scene, const PlayerConfig *config, PlayerStats *stats) :
    Damager(scene, Layer::LAYER_PLAYER), m_config(config), m_stats(stats),
    m_facingRight(true), m_hDirection(0.f),
    m_state(State::IDLE), m_animator(),
    m_shieldAnimator(), m_ejectionScore(0.f), m_defend(false), m_launchBegins(false),
    m_ejection(b2Vec2_zero), m_hVelocity(0.f),
    m_jumpImpulse(14.f), m_ai(nullptr),
    m_attackType(AttackType::NONE),
    m_delayAttack(-1.f), m_delayEarlyJump(-1.f), 
    m_delayLock(-1.f), m_delayLockAttack(-1.f),
    m_autoVelocity(0.f), m_hasAutoVelocity(false), m_externalVelocity(b2Vec2_zero),
    m_isGrounded(true), m_wasGrounded(true), m_inContact(false),
    m_bodyFixture(nullptr), m_feetFixture(nullptr), m_lastDamager(nullptr),
    m_renderShift(b2Vec2_zero)
{
    SetName("Player");
    SetStartPosition(0.f, 3.f);

    AssetManager *assets = scene->GetAssetManager();
    SpriteSheet *spriteSheet = nullptr;
    SpriteGroup *spriteGroup = nullptr;
    SpriteAnim *anim = nullptr;

    // Bouclier
    spriteSheet = assets->GetSpriteSheet(SHEET_SHIELD);
    AssertNew(spriteSheet);
    spriteGroup = spriteSheet->GetGroup("Shield");
    AssertNew(spriteGroup);

    anim = m_shieldAnimator.CreateAnimation("Shield", spriteGroup);
    anim->SetCycleCount(-1);
    anim->SetFPS(20.f);

    // IA
    if (config->isCPU)
    {
        m_ai = new PlayerAI(this);
    }

    m_animator.AddListener(this);

    // ID
    SetPlayerID(config->playerID);

    // Physique
    m_accAir = 30.f;
    m_accGround = 60.f;
    m_maxSpeed = 50.f; // TODO : adapter

    // Délais 
    // AddFixedUpdateDelay(&m_delayLock);// TODO : decommenter
}

Player::~Player()
{
    if (m_ai) delete m_ai;
}

void Player::Start()
{
    SetState(State::IDLE);
}

void Player::SetState(Player::State state)
{
    State prevState = m_state;
    if (prevState != state)
    {
        m_state = state;
        OnStateChanged(state, prevState);
    }
}

void Player::Update()
{
    const PlayerInput &input = GetPlayerInput();

    SetVisible(true);
    m_hDirection = input.axisX;

    StageManager *stageManager = StageManager::GetFromScene(m_scene);
    if (stageManager == nullptr) return;
    if (stageManager->IsPaused()) return;

    if (input.jumpPressed) // TODO : décommenter pour récupérer l'information de saut
    {
        //m_delayEarlyJump = 0.1f;
    }
    if (input.attackPressed)
    {
        // TODO : membre m_attackType à modifier
    }

    // TODO : membre m_defend à modifier
}

void Player::Render()
{
    Camera *camera = m_scene->GetActiveCamera();
    b2Vec2 position = GetInterpolatedPosition();

    SDL_RendererFlip flip = m_facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    const float s = m_facingRight ? 1.f : -1.f;

    SDL_Texture *texture = m_animator.GetTexture();
    if (texture)
    {
        const SDL_Rect *src = m_animator.GetSourceRect();
        SDL_FRect rect = { 0 };

        b2Vec2 spritePosition = position;
        spritePosition.x += s * m_renderShift.x;
        spritePosition.y += m_renderShift.y;
        camera->WorldToView(spritePosition, src, 22.f, rect);

        RenderCopyExF( // TODO : C'est cadeau, les paramètres sont bons
            g_renderer, texture, src, &rect, 
            Anchor::SOUTH, 0.f, b2Vec2(0.5f, 0.5f), flip
        );

    }

    // TODO : Bouclier
    //texture = m_shieldAnimator.GetTexture();
    
}

void Player::FixedUpdate()
{
    Damager::FixedUpdate();

    if (m_ai)
    {
        m_ai->FixedUpdate();
        Update();
    }

    b2Body *body = GetBody();
    b2Vec2 position = body->GetPosition();
    b2Vec2 velocity = GetVelocity();

    m_animator.Update(m_scene->GetTime().GetDelta());
    m_shieldAnimator.Update(m_scene->GetTime().GetDelta());

    if (position.y < -5.f || fabsf(position.x) > 40.f || position.y > 25.f )
    {
        OnPlayerKO();
        return;
    }

    // Détection du sol
    FixedUpdateIsGrounded();

    if (m_wasGrounded == false && m_isGrounded)
    {
        m_scene->GetAssetManager()->PlaySoundFX(SFX_LAND);
        EmitDustImpact();
    }

    // Met à jour l'état du joueur
    FixedUpdateState();

    FixedUpdateAutoVelocity();

    // Calcule l'orientation
    if ((IsAttacking() == false) && m_delayLock < 0.f)
    {
        if ((m_hDirection > 0.f && !m_facingRight) ||
            (m_hDirection < 0.f && m_facingRight))
        {
            m_facingRight = !m_facingRight;
        }
    }

    // Met à jour la physique
    FixedUpdatePhysics();

    m_inContact = false;
    m_launchBegins = false;
    m_externalVelocity.SetZero();
    m_lastDamager = nullptr;
}


void Player::FixedUpdateIsGrounded()
{
    b2Body *body = GetBody();
    b2Vec2 velocity = body->GetLinearVelocity() - m_externalVelocity;
    b2Vec2 position = body->GetPosition();

    m_wasGrounded = m_isGrounded;
    m_isGrounded = false;

    if (velocity.y < 8.f)
    {
        for (int i = -1; i <= 1; i += 2)
        {
            b2Vec2 point1 = position + b2Vec2(i * 0.3f, 0.3f);
            b2Vec2 point2 = position + b2Vec2(i * 0.3f, -0.1f);
            QueryFilter filter(CATEGORY_TERRAIN);
            RayHit rayHit = m_scene->RayCastFirstGizmos(point1, point2, filter);
            m_isGrounded = (rayHit.fixture != nullptr);
            if (m_isGrounded) break;
        }
    }
    if (m_wasGrounded == false && m_inContact == false)
    {
        m_isGrounded = false;
    }
}

void Player::FixedUpdateState()
{
    const PlayerInput &input = GetPlayerInput();
    b2Body *body = GetBody();
    b2Vec2 velocity = body->GetLinearVelocity() - m_externalVelocity;

    // Conditions de sortie
    if (m_launchBegins)
    {
        SetState(State::LAUNCHED);
        return;
    }
    if (m_state == State::LAUNCHED)
    {
        if (velocity.y > -4.f && velocity.Length() > 1.f)
            return;
    }
    // TODO : quitter si delaylock
 

    // TODO : état DEFEND
 

    // Etat au sol
    if (m_isGrounded)
    {
        // TODO : modifier
        if (IsAttacking() == false)
        {
            if (CanAttack())
            {
                SetState(State::ATTACK);
            }
            else
            {
                SetState(State::IDLE);
            }
        }
    }
    else // Etat en l'air 
    {
        if (IsAttacking() == false)
        {
            if (m_state != State::JUMP && m_state != State::FALL)
            {
                SetState(State::JUMP);
            }

            if (m_state == State::JUMP && velocity.y <= 2.f)
            {
                SetState(State::FALL);
            }
            else if (m_state == State::FALL && velocity.y > 6.f)
            {
                SetState(State::JUMP);
            }
        }
    }
}

void Player::FixedUpdateAutoVelocity()
{
    switch (m_state)
    {
    case State::ATTACK: 
        // TODO forcer hasAutoVelocity en cas d'attaque et autres
        break;
    default:
        m_hasAutoVelocity = false;
        m_autoVelocity = 0.f;
        break;
    }
}

void Player::FixedUpdatePhysics()
{
    const PlayerInput &input = GetPlayerInput();

    b2Body *body = GetBody();
    b2Vec2 velocity = body->GetLinearVelocity();
    b2Vec2 position = body->GetPosition();

    body->SetGravityScale(1.f);

    // Conditions de sorties
    if (m_launchBegins) // TODO
    {
        body->SetLinearVelocity(m_ejection);
        return;
    }

    // TODO : jouer sur la gravité 
    //if (m_state == State::LAUNCHED) 
    //{
    //
    //    return;
    //}

    // TODO : jouer sur la gravité (0) et annuler la vitesse
    //if (m_state == State::TAKE_DAMAGE)
    //{
    //
    //    return;
    //}

    // TODO : quitter si delayLock
    

    // Saut // TODO
    if (m_delayEarlyJump > 0.f && m_isGrounded)
    {
        // TODO : vitesse verticale et reinit m_delayEarlyJump

        // TODO : décommenter
        // m_scene->GetAssetManager()->PlaySoundFX(SFX_JUMP_GROUND); 
        
    }

    // TODO : décommenter pour le saut long (on joue directement sur la gravité
    //if ((m_isGrounded == false)) 
    //{
    //    float scale = input.jumpDown ? 0.5f : 1.0f;
    //    body->SetGravityScale(scale);
    //}

    // TODO : Mise a jour de la vitesse
    m_hVelocity = 0.f;
    if (m_state != State::DEFEND)
    {
        //m_hVelocity = m_maxSpeed * m_hDirection;
    }
    velocity.x = m_hVelocity;

    //if (m_hasAutoVelocity) // TODO : décommenter pour gérer la vitesse automatique
    //{
    //    velocity.x = m_autoVelocity;
    //}
    //else
    //{
    //    velocity.x = m_hVelocity;
    //}

    velocity += m_externalVelocity; // Vitese de la plateforme a ajouter au joueur
    //body->SetLinearVelocity(velocity); // TODO : decommenter pour appliquer la vitesse au corps
    m_externalVelocity.SetZero();
}

void Player::OnCollisionStay(GameCollision &collision)
{
    if (collision.fixture == m_bodyFixture)
    {
        collision.SetEnabled(false);
        return;
    }

    float sep = b2Min(collision.manifold.separations[0], collision.manifold.separations[1]);
    if (sep < -0.1f)
    {
        collision.SetEnabled(false);
    }

    Terrain *terrain = dynamic_cast<Terrain *>(collision.gameBody);
    if (terrain && terrain->IsOneWay())
    {
        if (Math::AngleDeg(collision.manifold.normal, b2Vec2(0, 1)) < 110.f)
        {
            collision.SetEnabled(false);
        }

        if (collision.manifold.normal.y > -0.1f)
            collision.SetEnabled(false);
    }

    if (collision.IsEnabled())
    {
        Platform *platform = dynamic_cast<Platform *>(collision.gameBody);
        {
            if (platform) platform->AddGameBody(this);
        }

        m_inContact = true;
    }
}

void Player::EmitEjectionParticles()
{
    AssetManager *assets = m_scene->GetAssetManager();
    SpriteSheet *spriteSheet = assets->GetSpriteSheet(SHEET_VFX_PARTICLES);
    AssertNew(spriteSheet);
    SpriteGroup *spriteGroup = spriteSheet->GetGroup("Square");
    AssertNew(spriteGroup);

    b2Vec2 position = GetPosition();
    float angle = Math::AngleDeg(b2Vec2(1.f, 0.f), m_ejection);
    for (int i = 0; i < 30; i++)
    {
        Particle *particle = m_scene->GetParticleSystem(LAYER_PARTICLES)
            ->EmitParticle(spriteGroup, position, 20.f);

        SpriteAnim *anim = particle->GetSpriteAnimation();
        anim->SetCycleCount(0);

        b2Vec2 velocity = Math::UnitVectorDeg(angle + Random::RangeF(-15.f, 15.f));
        velocity *= Random::RangeF(5.f, 15.0f);

        particle->SetLifetime(Random::RangeF(0.5f, 1.2f));
        particle->SetVelocity(velocity);
        particle->SetDamping(1.f);
        particle->SetGravity(b2Vec2(0.f, -10.f));

        LerpAnim<float> *alphaAnim = particle->CreateAlphaAnimation(1.f, 0.f);
        alphaAnim->SetEasing(EasingFct_In);

        LerpAnim<float> *scaleAnim = particle->CreateScaleAnimation(1.0f, 0.25f);
    }
}

void Player::EmitSmallDust(float xShift)
{
    AssetManager *assets = m_scene->GetAssetManager();
    SpriteSheet *spriteSheet = assets->GetSpriteSheet(SHEET_VFX_DUST);
    AssertNew(spriteSheet);
    SpriteGroup *spriteGroup = spriteSheet->GetGroup("Effect");
    AssertNew(spriteGroup);

    b2Vec2 position = GetPosition() + b2Vec2(xShift, -0.1f);
    Particle *particle = m_scene->GetParticleSystem(LAYER_PARTICLES)
        ->EmitParticle(spriteGroup, position, 20.f);

    SpriteAnim *anim = particle->GetSpriteAnimation();
    anim->SetFPS(15.f);
    anim->SetCycleCount(1);

    particle->SetLifetimeFromAnim();
    particle->SetOpacity(0.8f);
    particle->SetAnchor(Anchor::SOUTH);
    if (m_facingRight == false) particle->SetFlip(SDL_FLIP_HORIZONTAL);

    LerpAnim<float> *alphaAnim = particle->CreateAlphaAnimation(0.7f, 0.f);
}

void Player::EmitDustImpact()
{
    AssetManager *assets = m_scene->GetAssetManager();
    SpriteSheet *spriteSheet = assets->GetSpriteSheet(SHEET_VFX_IMPACT_DUST);
    AssertNew(spriteSheet);
    SpriteGroup *spriteGroup = spriteSheet->GetGroup("Effect");
    AssertNew(spriteGroup);

    b2Vec2 position = GetPosition() + b2Vec2(0.f, 0.0f); // TODO : modifier
    Particle *particle = m_scene->GetParticleSystem(LAYER_PARTICLES)
        ->EmitParticle(spriteGroup, position, 12.f); // TODO : modifier

    SpriteAnim *anim = particle->GetSpriteAnimation();
    anim->SetFPS(60.f); // TODO : modifier
    anim->SetCycleCount(1);
    anim->SetDelay(0.1f);

    particle->SetLifetimeFromAnim();
    particle->SetAnchor(Anchor::SOUTH);
    particle->CreateAlphaAnimation(0.8f, 0.4f);
}

void Player::EmitHitParticle()
{
    AssetManager *assets = m_scene->GetAssetManager();
    SpriteSheet *spriteSheet = assets->GetSpriteSheet(SHEET_VFX_HIT);
    AssertNew(spriteSheet);
    SpriteGroup *spriteGroup = spriteSheet->GetGroup("Effect");
    AssertNew(spriteGroup);

    b2Vec2 position = GetPosition();
    position.x += Random::RangeF(-0.8f, 0.8f);
    position.y += Random::RangeF(0.5f, 1.5f);

    Particle *particle = m_scene->GetParticleSystem(LAYER_PARTICLES)
        ->EmitParticle(spriteGroup, position, 20.f);

    SpriteAnim *anim = particle->GetSpriteAnimation();
    anim->SetFPS(30.f);
    anim->SetCycleCount(1);

    particle->SetLifetimeFromAnim();
    particle->SetOpacity(0.8f);
}

void Player::OnPlayerKO()
{
    StageManager *stageManager = StageManager::GetFromScene(m_scene);
    if (stageManager == nullptr) return;

    m_hVelocity = 0.f;
    m_ejectionScore = 0.f;

    // TODO : Décommenter une fois les délais utilisés
    //m_delayAttack = -1.f;
    //m_delayEarlyJump = -1.f;
    //m_delayLock = -1.f;
    //m_delayLockAttack = -1.f;

    m_stats->fallCount++;

    Respawn();
}

void Player::Respawn()
{
    b2Body *body = GetBody();
    body->SetTransform(b2Vec2(0.f, 8.f), 0.f);
    body->SetLinearVelocity(b2Vec2_zero);
    UpdateInterpolation();
}

void Player::LockAttack(float lockTime)
{
    m_delayLockAttack = b2Max(m_delayLockAttack, lockTime);
}

void Player::OnStateChanged(Player::State state, Player::State prevState)
{
    AssetManager *assets = m_scene->GetAssetManager();

    m_autoVelocity = 0.f;

    switch (state)
    {
    case State::ATTACK:
        m_delayAttack = -1.f;
        break;

    // TODO : en DEFEND, animation

    default:
        break;
    }

    if (m_ai) m_ai->OnStateChanged(state, prevState);
}

//void Player::Heal(float amount) // TODO : compléter pour soigner
//{
//
//}

bool Player::TakeDamage(const Damage &damage, Damager *damager)
{
    // TODO : Pendre en compte l'état DEFEND
    if (m_lastDamager == damager)
    {
        return false;
    }
    m_lastDamager = damager;

    // TODO : emission de particules hit

    // TODO : Changement d'état

    m_delayLockAttack = b2Max(m_delayLockAttack, damage.lockAttackTime);

    // TODO : Mettre à jour le delayLock

    // TODO : Faire des dégats

    // TODO : Dans le cas d'une attaque avec éjection, MAJ m_ejection et m_launchBegins

    // Stats
    m_stats->damageTaken += damage.amount;

    return true;
}

const PlayerInput &Player::GetPlayerInput() const
{
    if (m_ai)
    {
        return m_ai->GetInput();
    }
    ControlsInput *controlsInput = ControlsInput::GetFromManager(m_scene->GetInputManager());
    return controlsInput->players[m_config->playerID];
}

void Player::PlaySFXAttack(int soundID)
{
    AssetManager *assets = m_scene->GetAssetManager();
    float volume = Random::RangeF(0.5f, 0.8f);
    if (m_config->isCPU) volume *= 0.5f;

    assets->SetSoundVolume(soundID, volume);
    assets->PlaySoundFX(soundID);
}

void Player::PlaySFXHit(bool hit, int soundID)
{
    if (hit == false) return;

    AssetManager *assets = m_scene->GetAssetManager();
    float volume = Random::RangeF(0.5f, 0.8f);
    if (m_config->isCPU) volume *= 0.5f;

    assets->SetSoundVolume(soundID, volume);
    assets->PlaySoundFX(soundID);
}
