#pragma once

#include "LabSound/extended/LabSound.h"
#include "audio-node.h"
#include "audio-param.h"
#include "holojs/private/chakra.h"
#include "holojs/private/object-lifetime.h"
#include "holojs/private/webaudio-interfaces.h"

namespace HoloJs {

class HoloJsScriptHost;

namespace Win32 {

namespace WebAudio {

class AudioListener : public HoloJs::IAudioListener {
   public:
    AudioListener(std::shared_ptr<lab::AudioContext> context) : m_context(context) {}

    virtual ~AudioListener() { }

    virtual void Release() {}

    virtual JsValueRef connect(IAudioNode* destination) { return JS_INVALID_REFERENCE; }
    virtual JsValueRef disconnect() { return JS_INVALID_REFERENCE; }

    virtual void setPosition(float x, float y, float z) { m_context->listener().setPosition(x, y, z); }
    virtual void setOrientation(float x, float y, float z, float upX, float upY, float upZ)
    {
        m_context->listener().setOrientation(x, y, z, upX, upY, upZ);
    }

	virtual IAudioParam* positionX() { return new AudioParam(m_context, m_context->listener().positionX()); }
	virtual IAudioParam* positionY() { return new AudioParam(m_context, m_context->listener().positionY()); }
	virtual IAudioParam* positionZ() { return new AudioParam(m_context, m_context->listener().positionZ()); }
	
	virtual IAudioParam* forwardX() { return new AudioParam(m_context, m_context->listener().forwardX()); }
	virtual IAudioParam* forwardY() { return new AudioParam(m_context, m_context->listener().forwardY()); }
	virtual IAudioParam* forwardZ() { return new AudioParam(m_context, m_context->listener().forwardZ()); }

	virtual IAudioParam* upX() { return new AudioParam(m_context, m_context->listener().upX()); }
	virtual IAudioParam* upY() { return new AudioParam(m_context, m_context->listener().upY()); }
	virtual IAudioParam* upZ() { return new AudioParam(m_context, m_context->listener().upZ()); }

   private:
    std::shared_ptr<lab::AudioContext> m_context;
};
}  // namespace WebAudio
}  // namespace Win32
}  // namespace HoloJs