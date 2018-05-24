#pragma once

#include "IRelease.h"
#include "LabSound/extended/LabSound.h"

namespace HologramJS {
namespace Audio {
class SoundBuffer : public HologramJS::Utilities::IRelease {
   public:
    SoundBuffer(std::shared_ptr<lab::SoundBuffer> soundBuffer) : m_soundBuffer(soundBuffer) {}
    virtual ~SoundBuffer() {}

    virtual void Release() {}

    std::shared_ptr<lab::SoundBuffer> getSoundBuffer() { return m_soundBuffer; }

   private:
    std::shared_ptr<lab::SoundBuffer> m_soundBuffer;
};

}  // namespace Audio
}  // namespace HologramJS
