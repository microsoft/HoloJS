#include "pch.h"
#include "AudioContext.h"

using namespace HologramJS::Audio;
using namespace std;

AudioContext::AudioContext() {}

AudioContext::~AudioContext()
{
    if (m_audioContentRef != JS_INVALID_REFERENCE) {
        JsRelease(m_audioContentRef, nullptr);
        m_audioContentRef = JS_INVALID_REFERENCE;
    }
}

bool AudioContext::DecodeAudioData(JsValueRef data, JsValueRef onSuccess, JsValueRef onError)
{
    JsValueType dataType;
    RETURN_IF_JS_ERROR(JsGetValueType(data, &dataType));

    if (dataType != JsArrayBuffer) {
        return false;
    }

    RETURN_IF_JS_ERROR(JsAddRef(data, nullptr));
    m_audioContentRef = data;

    byte* buffer;
    unsigned int bufferLength;
    RETURN_IF_JS_ERROR(JsGetArrayBufferStorage(data, &buffer, &bufferLength));
}
