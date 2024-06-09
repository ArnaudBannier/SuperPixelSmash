/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "Settings.h"
#include "Animation.h"
#include "SpriteSheet.h"

class SpriteAnim : public Animation
{
public:
    SpriteAnim(const std::string &name, SpriteGroup *spriteGroup);

    virtual void Play() override;
    virtual void Update(float dt) override;
    virtual void SetCycleTime(float cycleTime) override;

    void SetFPS(float fps);
    int GetSpriteCount() const;

    SpriteGroup *GetSpriteGroup();
    SDL_Texture *GetTexture() const;
    const SDL_Rect *GetSourceRect() const;

    int GetFrameID() const;

protected:
    SpriteGroup *m_spriteGroup;
    std::vector<float> m_frameTimes;
    int m_frameID;

    void ComputeFrameTimes();
};

inline void SpriteAnim::SetCycleTime(float cycleTime)
{
    Animation::SetCycleTime(cycleTime);
    ComputeFrameTimes();
}

inline int SpriteAnim::GetSpriteCount() const
{
    return m_spriteGroup->GetSpriteCount();
}

inline SpriteGroup *SpriteAnim::GetSpriteGroup()
{
    return m_spriteGroup;
}

inline SDL_Texture *SpriteAnim::GetTexture() const
{
    return m_spriteGroup->GetTexture();
}

inline const SDL_Rect *SpriteAnim::GetSourceRect() const
{
    return m_spriteGroup->GetSourceRect(m_frameID);
}

inline int SpriteAnim::GetFrameID() const
{
    return m_frameID;
}

inline void SpriteAnim::SetFPS(float fps)
{
    SetCycleTime(GetSpriteCount() / fps);
}
