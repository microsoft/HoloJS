#pragma once

#include "LabSound/extended/LabSound.h"
#include "holojs/private/chakra.h"
#include "holojs/private/event-target.h"
#include "holojs/private/holojs-script-host-internal.h"
#include "holojs/private/object-lifetime.h"
#include "holojs/private/webaudio-interfaces.h"
#include <memory>

namespace HoloJs {

class HoloJsScriptHost;

namespace Win32 {

namespace WebAudio {

class AudioContext : public HoloJs::IAudioContext {
   public:
    AudioContext(HoloJs::IHoloJsScriptHostInternal* host)
        : m_host(host)
    {
        m_context = lab::MakeRealtimeAudioContext(2);
    }

    virtual ~AudioContext();

    virtual void Release() {  }

    virtual IGainNode* createGain();
    virtual IAudioDestinationNode* getDestination();
    virtual IPannerNode* createPanner();
    virtual IAudioListener* getListener();
    virtual IAudioBufferSource* createAudioBufferSource();

    virtual long decodeAudioData(JsValueRef data, JsValueRef onSuccessCallback, JsValueRef onFailedCallback);

	virtual float getSampleRate() { return m_context->sampleRate(); }
	virtual double getCurrentTime() { return m_context->currentTime(); }

   private:
    HoloJs::IHoloJsScriptHostInternal* m_host;

    std::shared_ptr<lab::AudioContext> m_context;
};
}  // namespace WebAudio
}  // namespace Win32
}  // namespace HoloJs