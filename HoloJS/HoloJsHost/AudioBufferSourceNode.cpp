#include "pch.h"
#include "AudioBufferSourceNode.h"
#include "AudioParam.h"
#include "ScriptHostUtilities.h"
#include "ScriptResourceTracker.h"
#include "SoundBuffer.h"

using namespace HologramJS::Audio;
using namespace HologramJS::Utilities;

bool AudioBufferSourceNode::InitializeProjections()
{
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"setBuffer", L"audioBufferSourceNode", setBuffer));
    RETURN_IF_FALSE(
        ScriptHostUtilities::ProjectFunction(L"getPlaybackRate", L"audioBufferSourceNode", getPlaybackRate));

    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"start", L"audioBufferSourceNode", start));
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"stop", L"audioBufferSourceNode", stop));

    return true;
}

JsValueRef CHAKRA_CALLBACK AudioBufferSourceNode::getPlaybackRate(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    RETURN_IF_FALSE(argumentCount == 2);

    auto audioBufferSourceNode = ScriptResourceTracker::ExternalToObject<AudioBufferSourceNode>(arguments[1]);
    RETURN_INVALID_REF_IF_NULL(audioBufferSourceNode);

    return ScriptResourceTracker::ObjectToDirectExternal(new AudioParam(
        audioBufferSourceNode->m_context, audioBufferSourceNode->m_audioBufferSourceNode->playbackRate()));
}

JsValueRef CHAKRA_CALLBACK AudioBufferSourceNode::setBuffer(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    RETURN_IF_FALSE(argumentCount == 3);

    auto audioBufferSourceNode = ScriptResourceTracker::ExternalToObject<AudioBufferSourceNode>(arguments[1]);
    RETURN_INVALID_REF_IF_NULL(audioBufferSourceNode);

    auto soundBuffer = ScriptResourceTracker::ExternalToObject<SoundBuffer>(arguments[2]);
    RETURN_INVALID_REF_IF_NULL(soundBuffer);

    auto lockedArea = [&](lab::ContextGraphLock& graphLock, lab::ContextRenderLock& renderLock) {
        audioBufferSourceNode->m_audioBufferSourceNode->setBuffer(renderLock,
                                                                  soundBuffer->getSoundBuffer()->audioBuffer);
    };

    lab::AcquireLocksForContext("Tone and Sample App", audioBufferSourceNode->m_context, lockedArea);

    return JS_INVALID_REFERENCE;
}

JsValueRef CHAKRA_CALLBACK AudioBufferSourceNode::start(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    RETURN_INVALID_REF_IF_TRUE(argumentCount < 2);

    auto audioBufferSourceNode = ScriptResourceTracker::ExternalToObject<AudioBufferSourceNode>(arguments[1]);
    RETURN_INVALID_REF_IF_NULL(audioBufferSourceNode);

    float when = argumentCount > 2 ? ScriptHostUtilities::GLfloatFromJsRef(arguments[2]) : 0;

    audioBufferSourceNode->m_audioBufferSourceNode->start(when);

    return JS_INVALID_REFERENCE;
}

JsValueRef CHAKRA_CALLBACK AudioBufferSourceNode::stop(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    RETURN_INVALID_REF_IF_TRUE(argumentCount < 2);

    auto audioBufferSourceNode = ScriptResourceTracker::ExternalToObject<AudioBufferSourceNode>(arguments[1]);
    RETURN_INVALID_REF_IF_NULL(audioBufferSourceNode);

    float when = argumentCount > 2 ? ScriptHostUtilities::GLfloatFromJsRef(arguments[2]) : 0;

    audioBufferSourceNode->m_audioBufferSourceNode->stop(when);

    return JS_INVALID_REFERENCE;
}
