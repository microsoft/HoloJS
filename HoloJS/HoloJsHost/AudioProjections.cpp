#include "pch.h"
#include "AudioProjections.h"
#include "AudioContext.h"
#include "AudioNode.h"
#include "AudioBufferSourceNode.h"
#include "AudioParam.h"
#include "AudioScheduledSourceNode.h"
#include "ScriptHostUtilities.h"
#include "ScriptResourceTracker.h"

using namespace HologramJS::Utilities;
using namespace HologramJS::Audio;

bool AudioProjections::Initialize()
{
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"createContext", L"audio", createContext));
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"decodeAudioData", L"audio", decodeAudioData));

    RETURN_IF_FALSE(AudioNode::InitializeProjections());
    RETURN_IF_FALSE(AudioContext::InitializeProjections());
    RETURN_IF_FALSE(AudioBufferSourceNode::InitializeProjections());
    RETURN_IF_FALSE(AudioParam::InitializeProjections());
    RETURN_IF_FALSE(AudioScheduledSourceNode::InitializeProjections());

    return true;
}

JsValueRef CHAKRA_CALLBACK AudioProjections::createContext(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    return ScriptResourceTracker::ObjectToDirectExternal(new AudioContext());
}

JsValueRef CHAKRA_CALLBACK AudioProjections::decodeAudioData(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    RETURN_INVALID_REF_IF_TRUE(argumentCount < 3);

    AudioContext* context = ScriptResourceTracker::ExternalToObject<AudioContext>(arguments[1]);
    RETURN_INVALID_REF_IF_NULL(context);

    JsValueRef data = arguments[2];

    JsValueRef onSuccessCallback = argumentCount > 3 ? arguments[3] : JS_INVALID_REFERENCE;
    JsValueRef onFailedCallback = argumentCount > 4 ? arguments[4] : JS_INVALID_REFERENCE;

    context->DecodeAudioData(data, onSuccessCallback, onFailedCallback);

    return JS_INVALID_REFERENCE;
}
