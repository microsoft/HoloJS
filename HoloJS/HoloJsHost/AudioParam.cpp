#include "pch.h"
#include "AudioParam.h"
#include "ScriptHostUtilities.h"
#include "ScriptResourceTracker.h"

using namespace HologramJS::Audio;
using namespace HologramJS::Utilities;

bool AudioParam::InitializeProjections()
{
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"setValueAtTime", L"audioParam", setValueAtTime));

    return true;
}

JsValueRef CHAKRA_CALLBACK AudioParam::setValueAtTime(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    RETURN_IF_FALSE(argumentCount == 4);

    auto audioParam = ScriptResourceTracker::ExternalToObject<AudioParam>(arguments[1]);
    RETURN_INVALID_REF_IF_NULL(audioParam);

    auto value = ScriptHostUtilities::GLfloatFromJsRef(arguments[2]);
    auto startTime = ScriptHostUtilities::GLfloatFromJsRef(arguments[3]);

    audioParam->m_audioParam->setValueAtTime(value, startTime);

    return JS_INVALID_REFERENCE;
}
