/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "Damager.h"
#include "StageManager.h"

Damager::Damager(Scene *scene, int layer) :
    GameBody(scene, layer), m_playerID(-1)
{
    SetName("Damager");
}

Damager::~Damager()
{
}

bool Damager::TakeDamage(const Damage &damage, Damager *damager)
{
    return false;
}

bool Damager::AttackCircle(
    const Damage &damage, const QueryFilter &filter,
    b2Vec2 center, float radius)
{
    bool hit = false;
    std::vector<OverlapResult> result;
    m_scene->OverlapCircleGizmos(center, radius, filter, result);
    for (OverlapResult &overlap : result)
    {
        Damager *damagable = dynamic_cast<Damager *>(overlap.gameBody);
        if (damagable && damagable->TakeDamage(damage, this))
        {
            hit = true;
        }
    }
    return hit;
}

bool Damager::AttackPolygon(
    const Damage &damage, const QueryFilter &filter,
    const b2Vec2 *vertices, int vertexCount)
{
    bool hit = false;
    std::vector<OverlapResult> result;
    m_scene->OverlapPolygonGizmos(vertices, vertexCount, filter, result);
    for (OverlapResult &overlap : result)
    {
        Damager *damagable = dynamic_cast<Damager *>(overlap.gameBody);
        if (damagable && damagable->TakeDamage(damage, this))
        {
            hit = true;
        }
    }
    return hit;
}

bool Damager::AttackBox(
    const Damage &damage, const QueryFilter &filter,
    b2Vec2 center, float hx, float hy, float angleDeg)
{
    bool hit = false;
    std::vector<OverlapResult> result;
    m_scene->OverlapBoxGizmos(center, b2Vec2(hx, hy), angleDeg, filter, result);
    for (OverlapResult &overlap : result)
    {
        Damager *damagable = dynamic_cast<Damager *>(overlap.gameBody);
        if (damagable && damagable->TakeDamage(damage, this))
        {
            hit = true;
        }
    }
    return hit;
}

Damage::Damage() :
    amount(0), lockTime(0.f), lockAttackTime(0.f),
    ejection(b2Vec2_zero), hasEjection(false)
{
}

Damage::~Damage()
{
}

const PlayerConfig *Damager::GetPlayerConfig() const
{
    if (m_playerID < 0 || m_playerID >= MAX_PLAYER_COUNT)
    {
        assert(false);
        return nullptr;
    }

    StageManager *stageManager = StageManager::GetFromScene(m_scene);
    if (stageManager == nullptr) return nullptr;

    return stageManager->GetPlayerConfig(m_playerID);
}
