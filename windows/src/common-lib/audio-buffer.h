#pragma once
#include "holojs/private/webaudio-interfaces.h"
#include "LabSound/extended/LabSound.h"

namespace HoloJs {


namespace Win32 {

namespace WebAudio {

class AudioBuffer : public HoloJs::IAudioBuffer {
   public:
    AudioBuffer(std::shared_ptr<lab::AudioBus> bus) : m_bus(bus) {}

    virtual ~AudioBuffer() {}

    virtual void Release() {}

    std::shared_ptr<lab::AudioBus> getBus() { return m_bus; }

   private:
    std::shared_ptr<lab::AudioBus> m_bus;
};
}  // namespace WebAudio
}  // namespace Win32
}  // namespace HoloJs