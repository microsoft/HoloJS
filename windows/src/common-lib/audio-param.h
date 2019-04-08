#pragma once

#include "LabSound/extended/LabSound.h"
#include "audio-node.h"
#include "holojs/private/chakra.h"
#include "holojs/private/error-handling.h"
#include "holojs/private/object-lifetime.h"
#include "holojs/private/webaudio-interfaces.h"

namespace HoloJs {

class HoloJsScriptHost;

namespace Win32 {

namespace WebAudio {

class AudioParam : public HoloJs::IAudioParam {
   public:
    AudioParam(std::shared_ptr<lab::AudioContext> context, std::shared_ptr<lab::AudioParam> param)
        : m_param(param), m_context(context)
    {
    }

    virtual ~AudioParam() { OutputDebugString(L"E"); }

    virtual void Release() {}

    virtual JsValueRef connect(IAudioNode* destination) { RETURN_INVALID_REF_IF_FALSE(false); }

    virtual JsValueRef disconnect() { RETURN_INVALID_REF_IF_FALSE(false); }

    virtual void setValueAtTime(float value, double time) { m_param->setValueAtTime(value, static_cast<float>(time)); }
    virtual void setTargetAtTime(double target, double startTime, double timeConstant)
    {
        m_param->setTargetAtTime(
            static_cast<float>(target), static_cast<float>(startTime), static_cast<float>(timeConstant));
    }

    virtual float getValue()
    {
        lab::ContextRenderLock r(m_context.get(), "getValue");
        return m_param->value(r);
    }
    virtual void setValue(float value) { m_param->setValue(value); }

    virtual float getDefaultValue() { return m_param->defaultValue(); }
    virtual float getMinValue() { return m_param->minValue(); }
    virtual float getMaxValue() { return m_param->maxValue(); }

    virtual void linearRampToValueAtTime(float value, float time) { m_param->linearRampToValueAtTime(value, time); }
    virtual void exponentialRampToValueAtTime(float value, float time)
    {
        m_param->exponentialRampToValueAtTime(value, time);
    }

    virtual void setValueCurveAtTime(const std::vector<float>& values, float time, float duration)
    {
        m_param-> setValueCurveAtTime(values, time, duration);
    }

	virtual void cancelScheduledValues(float startTime) { m_param->cancelScheduledValues(startTime); }

   private:
    std::shared_ptr<lab::AudioContext> m_context;
    std::shared_ptr<lab::AudioParam> m_param;
};
}  // namespace WebAudio
}  // namespace Win32
}  // namespace HoloJs