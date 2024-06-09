/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "Settings.h"

/// @ingroup Animator
/// @brief Type d'une courbe d'accélération.
/// Ces fonctions sont définies sur [0,1] et vérifient généralement f(0) = 0 et f(1) = 1.
typedef float (*EasingFct)(float);

/// @ingroup Animator
/// @brief Courbe d'accélération linéaire.
/// @param[in] x ratio entre 0.f et 1.f.
/// @return Le ratio de progression entre 0.f et 1.f.
float EasingFct_Linear(float x);

/// @ingroup Animator
/// @brief Courbe d'accélération douce en entrée.
/// @param[in] x ratio entre 0.f et 1.f.
/// @return Le ratio de progression entre 0.f et 1.f.
float EasingFct_In(float x);

/// @ingroup Animator
/// @brief Courbe d'accélération douce en sortie.
/// @param[in] x ratio entre 0.f et 1.f.
/// @return Le ratio de progression entre 0.f et 1.f.
float EasingFct_Out(float x);

/// @ingroup Animator
/// @brief Courbe d'accélération avec douce en entrée et en sortie.
/// @param[in] x ratio entre 0.f et 1.f.
/// @return Le ratio de progression entre 0.f et 1.f.
float EasingFct_InOut(float x);

/// @ingroup Animator
/// @brief Courbe d'accélération définie à partir du cosinus.
/// @param[in] x ratio entre 0.f et 1.f.
/// @return Le ratio de progression entre 0.f et 1.f.
float EasingFct_Cos(float x);


float EasingFct_Shake2(float t);
float EasingFct_Shake3(float t);
float EasingFct_Shake4(float t);
