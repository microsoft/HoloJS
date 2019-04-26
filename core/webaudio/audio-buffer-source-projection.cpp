#include "audio-buffer-source-projection.h"
#include "../host-interfaces.h"
#include "../include/holojs/private/error-handling.h"
#include "../include/holojs/private/platform-interfaces.h"
#include "../include/holojs/private/script-host-utilities.h"
#include "../include/holojs/private/webaudio-interfaces.h"
#include "../resource-management/resource-manager.h"

using namespace HoloJs;
using namespace HoloJs::ResourceManagement;
using namespace HoloJs::WebAudio;
using namespace std;

HRESULT AudioBufferSourceProjection::initialize()
{
    JS_PROJECTION_REGISTER(L"webaudioAudioBuffer", L"setBuffer", setBuffer);
    JS_PROJECTION_REGISTER(L"webaudioAudioBuffer", L"start", start);
    JS_PROJECTION_REGISTER(L"webaudioAudioBuffer", L"stop", stop);
    JS_PROJECTION_REGISTER(L"webaudioAudioBuffer", L"getPlaybackRate", getPlaybackRate);
	JS_PROJECTION_REGISTER(L"webaudioAudioBuffer", L"getDetune", getDetune);

    JS_PROJECTION_REGISTER(L"webaudioAudioBuffer", L"getLoop", getLoop);
    JS_PROJECTION_REGISTER(L"webaudioAudioBuffer", L"setLoop", setLoop);

    JS_PROJECTION_REGISTER(L"webaudioAudioBuffer", L"getLoopStart", getLoopStart);
    JS_PROJECTION_REGISTER(L"webaudioAudioBuffer", L"setLoopStart", setLoopStart);

    JS_PROJECTION_REGISTER(L"webaudioAudioBuffer", L"getLoopEnd", getLoopEnd);
    JS_PROJECTION_REGISTER(L"webaudioAudioBuffer", L"setLoopEnd", setLoopEnd);

    return S_OK;
}

JsValueRef AudioBufferSourceProjection::_getDetune(JsValueRef* arguments, unsigned short argumentCount)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    auto audioBufferSourceNode =
        m_resourceManager->externalToObject<IAudioBufferSource>(arguments[1], ObjectType::IAudioBufferSource);
    RETURN_INVALID_REF_IF_NULL(audioBufferSourceNode);

    return m_resourceManager->objectToDirectExternal(audioBufferSourceNode->getDetune(), ObjectType::IAudioParam);
}

JsValueRef AudioBufferSourceProjection::_getLoop(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    auto audioBufferSourceNode =
        m_resourceManager->externalToObject<IAudioBufferSource>(arguments[1], ObjectType::IAudioBufferSource);
    RETURN_INVALID_REF_IF_NULL(audioBufferSourceNode);

	auto value = audioBufferSourceNode->getLoop();
	JsValueRef valueRef;
	RETURN_INVALID_REF_IF_JS_ERROR(JsBoolToBoolean(value, &valueRef));

	return valueRef;
}

JsValueRef AudioBufferSourceProjection::_setLoop(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    auto audioBufferSourceNode =
        m_resourceManager->externalToObject<IAudioBufferSource>(arguments[1], ObjectType::IAudioBufferSource);
    RETURN_INVALID_REF_IF_NULL(audioBufferSourceNode);

	auto value = ScriptHostUtilities::GLbooleanFromJsRef(arguments[2]);
	audioBufferSourceNode->setLoop(value);
	return JS_INVALID_REFERENCE;
}

JsValueRef AudioBufferSourceProjection::_getLoopStart(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    auto audioBufferSourceNode =
        m_resourceManager->externalToObject<IAudioBufferSource>(arguments[1], ObjectType::IAudioBufferSource);
    RETURN_INVALID_REF_IF_NULL(audioBufferSourceNode);

	auto value = audioBufferSourceNode->getLoopStart();
	JsValueRef valueRef;
	RETURN_INVALID_REF_IF_JS_ERROR(JsDoubleToNumber(value, &valueRef));

	return valueRef;
}

JsValueRef AudioBufferSourceProjection::_setLoopStart(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    auto audioBufferSourceNode =
        m_resourceManager->externalToObject<IAudioBufferSource>(arguments[1], ObjectType::IAudioBufferSource);
    RETURN_INVALID_REF_IF_NULL(audioBufferSourceNode);

	auto value = ScriptHostUtilities::doubleFromJsRef(arguments[2]);
	audioBufferSourceNode->setLoopStart(value);
	return JS_INVALID_REFERENCE;
}

JsValueRef AudioBufferSourceProjection::_getLoopEnd(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    auto audioBufferSourceNode =
        m_resourceManager->externalToObject<IAudioBufferSource>(arguments[1], ObjectType::IAudioBufferSource);
    RETURN_INVALID_REF_IF_NULL(audioBufferSourceNode);

	auto value = audioBufferSourceNode->getLoopEnd();
	JsValueRef valueRef;
	RETURN_INVALID_REF_IF_JS_ERROR(JsDoubleToNumber(value, &valueRef));

	return valueRef;
}

JsValueRef AudioBufferSourceProjection::_setLoopEnd(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    auto audioBufferSourceNode =
        m_resourceManager->externalToObject<IAudioBufferSource>(arguments[1], ObjectType::IAudioBufferSource);
    RETURN_INVALID_REF_IF_NULL(audioBufferSourceNode);

	auto value = ScriptHostUtilities::doubleFromJsRef(arguments[2]);
	audioBufferSourceNode->setLoopEnd(value);

	return JS_INVALID_REFERENCE;
}

JsValueRef AudioBufferSourceProjection::_setBuffer(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    auto audioBufferSourceNode =
        m_resourceManager->externalToObject<IAudioBufferSource>(arguments[1], ObjectType::IAudioBufferSource);
    RETURN_INVALID_REF_IF_NULL(audioBufferSourceNode);

    auto audioBuffer = m_resourceManager->externalToObject<IAudioBuffer>(arguments[2], ObjectType::IAudioBuffer);
    RETURN_INVALID_REF_IF_NULL(audioBuffer);

    audioBufferSourceNode->setBuffer(audioBuffer);

    return JS_INVALID_REFERENCE;
}

JsValueRef AudioBufferSourceProjection::_start(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount >= 2);

    auto audioBufferSourceNode =
        m_resourceManager->externalToObject<IAudioBufferSource>(arguments[1], ObjectType::IAudioBufferSource);
    RETURN_INVALID_REF_IF_NULL(audioBufferSourceNode);

    float when = argumentCount > 2 ? ScriptHostUtilities::GLfloatFromJsRef(arguments[2]) : 0;
    audioBufferSourceNode->start(when);

    return JS_INVALID_REFERENCE;
}

JsValueRef AudioBufferSourceProjection::_stop(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount >= 2);

    auto audioBufferSourceNode =
        m_resourceManager->externalToObject<IAudioBufferSource>(arguments[1], ObjectType::IAudioBufferSource);
    RETURN_INVALID_REF_IF_NULL(audioBufferSourceNode);

    float when = argumentCount > 2 ? ScriptHostUtilities::GLfloatFromJsRef(arguments[2]) : 0;
    audioBufferSourceNode->stop(when);

    return JS_INVALID_REFERENCE;
}

JsValueRef AudioBufferSourceProjection::_getPlaybackRate(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    auto audioBufferSourceNode =
        m_resourceManager->externalToObject<IAudioBufferSource>(arguments[1], ObjectType::IAudioBufferSource);
    RETURN_INVALID_REF_IF_NULL(audioBufferSourceNode);

    return m_resourceManager->objectToDirectExternal(audioBufferSourceNode->getPlaybackRate(), ObjectType::IAudioParam);
}