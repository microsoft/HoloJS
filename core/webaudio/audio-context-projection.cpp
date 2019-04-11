#include "audio-context-projection.h"
#include "../holojs-script-host.h"
#include "../host-interfaces.h"
#include "../include/holojs/private/platform-interfaces.h"
#include "../include/holojs/private/script-host-utilities.h"
#include "../include/holojs/private/holojs-script-host-internal.h"
#include "../include/holojs/private/webaudio-interfaces.h"
#include "../resource-management/resource-manager.h"

using namespace HoloJs;
using namespace HoloJs::ResourceManagement;
using namespace HoloJs::WebAudio;
using namespace std;

HRESULT AudioContextProjection::initialize()
{
    // WebGL context projections
    JS_PROJECTION_REGISTER(L"webaudio", L"createContext", createContext);

    JS_PROJECTION_REGISTER(L"webaudioContext", L"createGain", createGain);
    JS_PROJECTION_REGISTER(L"webaudioContext", L"getDestination", getDestination);
    JS_PROJECTION_REGISTER(L"webaudioContext", L"decodeAudioData", decodeAudioData);
    JS_PROJECTION_REGISTER(L"webaudioContext", L"createPanner", createPanner);
    JS_PROJECTION_REGISTER(L"webaudioContext", L"getListener", getListener);
    JS_PROJECTION_REGISTER(L"webaudioContext", L"createAudioBufferSource", createBufferSource);

    JS_PROJECTION_REGISTER(L"webaudioContext", L"getSampleRate", getSampleRate);
    JS_PROJECTION_REGISTER(L"webaudioContext", L"getCurrentTime", getCurrentTime);

    RETURN_IF_FAILED(m_audioNodeProjection->initialize());
    RETURN_IF_FAILED(m_pannerNodeProjection->initialize());
    RETURN_IF_FAILED(m_audioListenerProjection->initialize());
    RETURN_IF_FAILED(m_audioBufferSourceProjection->initialize());
    RETURN_IF_FAILED(m_audioParamProjection->initialize());

    return S_OK;
}

JsValueRef AudioContextProjection::_createContext(JsValueRef* arguments, unsigned short argumentCount)
{
    return m_resourceManager->objectToDirectExternal(
        getPlatform()->createAudioContext(m_host), ObjectType::IAudioContext);
}

JsValueRef AudioContextProjection::_createGain(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_TRUE(argumentCount != 2);

    auto context = m_resourceManager->externalToObject<IAudioContext>(arguments[1], ObjectType::IAudioContext);
    RETURN_INVALID_REF_IF_NULL(context);

    auto gain = context->createGain();

    return m_resourceManager->objectToDirectExternal<IGainNode>(gain, ObjectType::IGainNode, ObjectType::IAudioNode);
}

JsValueRef AudioContextProjection::_getDestination(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_TRUE(argumentCount != 2);

    auto context = m_resourceManager->externalToObject<IAudioContext>(arguments[1], ObjectType::IAudioContext);
    RETURN_INVALID_REF_IF_NULL(context);

    auto destination = context->getDestination();

    return m_resourceManager->objectToDirectExternal<IAudioDestinationNode>(
        destination, ObjectType::IAudioDestinationNode, ObjectType::IAudioNode);
}

JsValueRef AudioContextProjection::_decodeAudioData(JsValueRef* arguments, unsigned short argumentCount)
{
    // decodeAudioData(callee, context, data, onSuccess, [onError]);
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4 || argumentCount == 5);

    auto context = m_resourceManager->externalToObject<IAudioContext>(arguments[1], ObjectType::IAudioContext);
    RETURN_INVALID_REF_IF_NULL(context);

    JsValueRef data = arguments[2];
    JsValueRef onSuccessCallback = arguments[3];

    JsValueRef onFailedCallback = JS_INVALID_REFERENCE;
    if (argumentCount == 5) {
        JsValueType onErrorType;
        RETURN_INVALID_REF_IF_JS_ERROR(JsGetValueType(arguments[4], &onErrorType));
        if (onErrorType != JsUndefined) {
            onFailedCallback = arguments[4];
        }
    }

    context->decodeAudioData(data, onSuccessCallback, onFailedCallback);

    return JS_INVALID_REFERENCE;
}

JsValueRef AudioContextProjection::_createPanner(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_TRUE(argumentCount != 2);

    auto context = m_resourceManager->externalToObject<IAudioContext>(arguments[1], ObjectType::IAudioContext);
    RETURN_INVALID_REF_IF_NULL(context);

    auto panner = context->createPanner();

    return m_resourceManager->objectToDirectExternal<IPannerNode>(
        panner, ObjectType::IPannerNode, ObjectType::IAudioNode);
}

JsValueRef AudioContextProjection::_getListener(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_TRUE(argumentCount != 2);

    auto context = m_resourceManager->externalToObject<IAudioContext>(arguments[1], ObjectType::IAudioContext);
    RETURN_INVALID_REF_IF_NULL(context);

    auto listener = context->getListener();

    return m_resourceManager->objectToDirectExternal<IAudioListener>(
        listener, ObjectType::IAudioListener, ObjectType::IAudioNode);
}

JsValueRef AudioContextProjection::_createBufferSource(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_TRUE(argumentCount != 2);

    auto context = m_resourceManager->externalToObject<IAudioContext>(arguments[1], ObjectType::IAudioContext);
    RETURN_INVALID_REF_IF_NULL(context);

    auto audioBufferSource = context->createAudioBufferSource();
    return m_resourceManager->objectToDirectExternal<IAudioBufferSource>(
        audioBufferSource, ObjectType::IAudioBufferSource, ObjectType::IAudioNode);
}

JsValueRef AudioContextProjection::_getSampleRate(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_TRUE(argumentCount != 2);

    auto context = m_resourceManager->externalToObject<IAudioContext>(arguments[1], ObjectType::IAudioContext);
    RETURN_INVALID_REF_IF_NULL(context);

	auto value = context->getSampleRate();
	JsValueRef valueRef;
	RETURN_INVALID_REF_IF_JS_ERROR(JsDoubleToNumber(static_cast<double>(value), &valueRef));

	return valueRef;
}

JsValueRef AudioContextProjection::_getCurrentTime(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_TRUE(argumentCount != 2);

    auto context = m_resourceManager->externalToObject<IAudioContext>(arguments[1], ObjectType::IAudioContext);
    RETURN_INVALID_REF_IF_NULL(context);

	auto value = context->getCurrentTime();
	JsValueRef valueRef;
	RETURN_INVALID_REF_IF_JS_ERROR(JsDoubleToNumber(value, &valueRef));

	return valueRef;
}