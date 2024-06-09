/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "Settings.h"
#include "Animation.h"

template <class T>
class LerpAnim : public Animation
{
public:
    LerpAnim(const std::string &name, T value0, T value1);

    void SetValues(T value0, T value1);
    T GetValue() const;

protected:
    T m_values[2];
};

template<class T>
inline LerpAnim<T>::LerpAnim(const std::string &name, T value0, T value1) :
    Animation(name), m_values{ value0, value1 }
{
}

template<class T>
inline void LerpAnim<T>::SetValues(T value0, T value1)
{
    m_values[0] = value0;
    m_values[1] = value1;
}

template<class T>
inline T LerpAnim<T>::GetValue() const
{
    float p = GetProgression();
    p = fmaxf(0.f, fminf(p, 1.f));
    return (1.f - p) * m_values[0] + p * m_values[1];
}
