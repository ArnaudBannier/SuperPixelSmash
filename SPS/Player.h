/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "GameSettings.h"
#include "GameCommon.h"
#include "Damager.h"

extern const float ATTACK_FRAME_TIME;

class PlayerAI;
class Player : public Damager, public AnimationListener
{
public:
    Player(Scene *scene, const PlayerConfig *config, PlayerStats *stats);
    virtual ~Player();

    virtual void Start() override;
    virtual void Update() override;
    virtual void Render() override;
    virtual void FixedUpdate() override;

    enum class State
    {
        IDLE, RUN, JUMP, FALL,
        ATTACK, DEFEND, TAKE_DAMAGE, LAUNCHED
        // ATTACK_AIR, SMASH_START, SMASH_HOLD, SMASH_RELEASE, ROLLING
    };

    virtual void OnCollisionStay(GameCollision &collision) override;
    virtual bool TakeDamage(const Damage &damage, Damager *damager) override;

    virtual void OnStateChanged(Player::State state, Player::State prevState);
    //void Heal(float amount); // TODO : decommenter

    const PlayerConfig *GetConfig() const;
    Player::State GetState() const;
    float GetEjectionScore() const;
    const PlayerStats *GetStats() const;

    void AddExternalVelocity(b2Vec2 externalVelocity);

    void PlaySFXAttack(int soundID);
    void PlaySFXHit(bool hit, int soundID);

protected:
    const PlayerConfig *m_config;
    PlayerStats *m_stats;

    friend class PlayerAI;
    PlayerAI *m_ai;
    
    b2Vec2 m_externalVelocity;
    b2Vec2 m_renderShift;


    Animator m_animator;

    Animator m_shieldAnimator;
    Damager *m_lastDamager;

    float m_delayEarlyJump;
    float m_delayAttack;
    float m_delayLock;
    float m_delayLockAttack;

    float m_ejectionScore;
    float m_hDirection;

    enum class AttackType
    {
        COMBO, SPECIAL, SMASH, NONE
    };
    AttackType m_attackType;

    bool m_isGrounded;
    bool m_wasGrounded;

    bool m_facingRight;
    bool m_defend;
    bool m_launchBegins;
    bool m_inContact;
    bool m_hasAutoVelocity;
    
    float m_accGround;
    float m_accAir;
    float m_maxSpeed;
    float m_hVelocity;
    float m_autoVelocity;

    float m_jumpImpulse;

    b2Fixture *m_feetFixture;
    b2Fixture *m_bodyFixture;
    b2Vec2 m_ejection;

    void SetState(Player::State state);

    void FixedUpdateIsGrounded();
    void FixedUpdateState();
    void FixedUpdateAutoVelocity();
    void FixedUpdatePhysics();

    const PlayerInput &GetPlayerInput() const;

    void OnPlayerKO();
    void Respawn();
    void LockAttack(float lockTime);
    bool IsAttacking() const;
    bool CanAttack() const;

    void EmitEjectionParticles();
    void EmitSmallDust(float xShift);
    void EmitDustImpact();
    void EmitHitParticle();

private:
    State m_state;
};

inline bool Player::IsAttacking() const
{
    return (m_state == State::ATTACK);
}

inline bool Player::CanAttack() const
{
    return m_delayLockAttack < 0.f;
}

inline const PlayerConfig *Player::GetConfig() const
{
    return m_config;
}

inline Player::State Player::GetState() const
{
    return m_state;
}

inline float Player::GetEjectionScore() const
{
    return m_ejectionScore;
}

inline const PlayerStats *Player::GetStats() const
{
    return m_stats;
}

inline void Player::AddExternalVelocity(b2Vec2 externalVelocity)
{
    if (m_isGrounded)
        m_externalVelocity += externalVelocity;
}
