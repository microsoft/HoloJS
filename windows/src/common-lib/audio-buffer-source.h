#pragma once

#include "LabSound/extended/LabSound.h"
#include "audio-buffer.h"
#include "audio-node.h"
#include "audio-param.h"
#include "holojs/private/chakra.h"
#include "holojs/private/object-lifetime.h"
#include "holojs/private/webaudio-interfaces.h"

namespace HoloJs {

class HoloJsScriptHost;

namespace Win32 {

namespace WebAudio {

class AudioBufferSource : public HoloJs::IAudioBufferSource, public AudioNode {
   public:
    AudioBufferSource(std::shared_ptr<lab::AudioContext> context,
                      std::shared_ptr<lab::SampledAudioNode> audioBufferSource)
        : m_audioBufferSource(audioBufferSource), m_context(context), AudioNode(context, audioBufferSource)
    {
		m_isValid = std::make_shared<bool>(true);
    }

    virtual ~AudioBufferSource()
    {
		*m_isValid = false;
    }

    virtual void Release() {}

    virtual JsValueRef connect(IAudioNode* destination) { return AudioNode::audioNodeConnect(destination); }

    virtual JsValueRef disconnect() { return AudioNode::audioNodeDisconnect(); }

    virtual IAudioParam* getPlaybackRate() { return new AudioParam(m_context, m_audioBufferSource->playbackRate()); }
    virtual IAudioParam* getDetune() { return new AudioParam(m_context, m_audioBufferSource->detune()); }

    virtual void start(float when) { m_audioBufferSource->start(when); }

    virtual void stop(float when) { m_audioBufferSource->stop(when); }

    virtual void setBuffer(IAudioBuffer* buffer)
    {
        auto bufferImpl = dynamic_cast<AudioBuffer*>(buffer);
        EXIT_IF_TRUE(bufferImpl == nullptr);

        lab::ContextRenderLock r(m_context.get(), "setBuffer");
        m_audioBufferSource->setBus(r, bufferImpl->getBus());
    }

    virtual bool getLoop() { return m_audioBufferSource->loop(); }
    virtual void setLoop(bool value) { m_audioBufferSource->setLoop(value); }

    virtual double getLoopStart() { return m_audioBufferSource->loopStart(); }
    virtual void setLoopStart(double value) { m_audioBufferSource->setLoopStart(value); }

    virtual double getLoopEnd() { return m_audioBufferSource->loopEnd(); }
    virtual void setLoopEnd(double value) { m_audioBufferSource->setLoopEnd(value); }

	std::shared_ptr<bool> m_isValid;

   private:
    std::shared_ptr<lab::AudioContext> m_context;
    std::shared_ptr<lab::SampledAudioNode> m_audioBufferSource;
};
}  // namespace WebAudio
}  // namespace Win32
}  // namespace HoloJs