/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "Utils.h"

#define RAD_TO_DEG 57.2957795130823f

inline float Math::SignedAngleAtan2(b2Vec2 from, b2Vec2 to)
{
    // Le calcule de l'angle est plus précis avec atan2 plutôt qu'avec acos
    // Il n'y a pas non plus besoin de normaliser les vecteurs
    float s = Math::Det(from, to); // |from|.|to|.sin(angle)
    float c = Math::Dot(from, to); // |from|.|to|.cos(angle)
    return atan2f(s, c);
}

float Math::SignedAngleDeg(b2Vec2 from, b2Vec2 to)
{
    return Math::SignedAngleAtan2(from, to) * RAD_TO_DEG;
}

float Math::SignedAngleRad(b2Vec2 from, b2Vec2 to)
{
    return Math::SignedAngleAtan2(from, to);
}

float Math::AngleDeg(b2Vec2 from, b2Vec2 to)
{
    return fabsf(Math::SignedAngleAtan2(from, to) * RAD_TO_DEG);
}

float Math::AngleRad(b2Vec2 from, b2Vec2 to)
{
    return fabsf(Math::SignedAngleAtan2(from, to));
}

float Math::SmoothDamp(
    float current, float target, float &currentVelocity,
    float smoothTime, float maxSpeed, float deltaTime)
{
    if (deltaTime < 0.001f) return current;

    // Implémentation de Unity
    // Voir Game Programming Gems 4 chapitre 1.10 page 99
    smoothTime = fmaxf(0.0001f, smoothTime);
    float omega = 2.f / smoothTime;
    float x = omega * deltaTime;
    float exp = 1.f / (1.f + x + 0.48f * x * x + 0.235f * x * x * x);
    float change = current - target;
    float targetCopy = target;
    float maxChange = maxSpeed * smoothTime;
    change = Math::Clamp(change, -maxChange, maxChange);
    target = current - change;
    float temp = (currentVelocity + omega * change) * deltaTime;
    currentVelocity = (currentVelocity - omega * temp) * exp;
    float res = target + (change + temp) * exp;
    if (targetCopy - current > 0.f == res > targetCopy)
    {
        res = targetCopy;
        currentVelocity = (res - targetCopy) / deltaTime;
    }
    return res;
}

bool Math::PointInAABB(const b2Vec2 &point, const b2AABB &aabb)
{
    b2Vec2 d1, d2;
    d1 = point - aabb.lowerBound;
    d2 = aabb.upperBound - point;

    if (d1.x < 0.0f || d1.y < 0.0f)
        return false;

    if (d2.x < 0.0f || d2.y < 0.0f)
        return false;

    return true;
}

b2Vec2 SDL_FRectDistance(const SDL_FRect &a, const SDL_FRect &b)
{
    float d1x = b.x - (a.x + a.w);
    float d1y = b.y - (a.y + a.h);
    float d2x = a.x - (b.x + b.w);
    float d2y = a.y - (b.y + b.h);
    float dx = fmaxf(d1x, d2x);
    float dy = fmaxf(d1y, d2y);
    return b2Vec2(dx, dy);
}
