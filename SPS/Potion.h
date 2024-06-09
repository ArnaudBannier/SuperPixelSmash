/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "GameSettings.h"
#include "GameCommon.h"
#include "Damager.h"

class Potion : public Damager
{
public:
    Potion(Scene *scene);
    virtual ~Potion();

    virtual void Start() override;
    virtual void Render() override;
    virtual void FixedUpdate() override;

    virtual bool TakeDamage(const Damage &damage, Damager *damager);

protected:
    bool m_used;
    Animator m_animator;
};
