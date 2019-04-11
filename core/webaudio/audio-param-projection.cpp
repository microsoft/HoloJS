#include "audio-param-projection.h"
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

HRESULT AudioParamProjection::initialize()
{
    JS_PROJECTION_REGISTER(L"webaudioAudioParam", L"setValueAtTime", setValueAtTime);
    JS_PROJECTION_REGISTER(L"webaudioAudioParam", L"setTargetAtTime", setTargetAtTime);

    JS_PROJECTION_REGISTER(L"webaudioAudioParam", L"getValue", getValue);
    JS_PROJECTION_REGISTER(L"webaudioAudioParam", L"setValue", setValue);

    JS_PROJECTION_REGISTER(L"webaudioAudioParam", L"getDefaultValue", getDefaultValue);
    JS_PROJECTION_REGISTER(L"webaudioAudioParam", L"getMinValue", getMinValue);
    JS_PROJECTION_REGISTER(L"webaudioAudioParam", L"getMaxValue", getMaxValue);

    JS_PROJECTION_REGISTER(L"webaudioAudioParam", L"linearRampToValueAtTime", linearRampToValueAtTime);
    JS_PROJECTION_REGISTER(L"webaudioAudioParam", L"exponentialRampToValueAtTime", exponentialRampToValueAtTime);
    JS_PROJECTION_REGISTER(L"webaudioAudioParam", L"setValueCurveAtTime", setValueCurveAtTime);

    JS_PROJECTION_REGISTER(L"webaudioAudioParam", L"cancelScheduledValues", cancelScheduledValues);

    return S_OK;
}

JsValueRef AudioParamProjection::_getValue(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    auto audioParam = m_resourceManager->externalToObject<IAudioParam>(arguments[1], ObjectType::IAudioParam);
    RETURN_INVALID_REF_IF_NULL(audioParam);

	auto value = audioParam->getValue();
	JsValueRef valueRef;
	RETURN_INVALID_REF_IF_JS_ERROR(JsDoubleToNumber(static_cast<double>(value), &valueRef));

	return valueRef;
}

JsValueRef AudioParamProjection::_setValue(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    auto audioParam = m_resourceManager->externalToObject<IAudioParam>(arguments[1], ObjectType::IAudioParam);
    RETURN_INVALID_REF_IF_NULL(audioParam);

	auto value = ScriptHostUtilities::GLfloatFromJsRef(arguments[2]);
	audioParam->setValue(value);
	return JS_INVALID_REFERENCE;
}

JsValueRef AudioParamProjection::_getDefaultValue(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    auto audioParam = m_resourceManager->externalToObject<IAudioParam>(arguments[1], ObjectType::IAudioParam);
    RETURN_INVALID_REF_IF_NULL(audioParam);

	auto value = audioParam->getDefaultValue();
	JsValueRef valueRef;
	RETURN_INVALID_REF_IF_JS_ERROR(JsDoubleToNumber(static_cast<double>(value), &valueRef));

	return valueRef;
}

JsValueRef AudioParamProjection::_getMinValue(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    auto audioParam = m_resourceManager->externalToObject<IAudioParam>(arguments[1], ObjectType::IAudioParam);
    RETURN_INVALID_REF_IF_NULL(audioParam);

	auto value = audioParam->getMinValue();
	JsValueRef valueRef;
	RETURN_INVALID_REF_IF_JS_ERROR(JsDoubleToNumber(static_cast<double>(value), &valueRef));

	return valueRef;
}

JsValueRef AudioParamProjection::_getMaxValue(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    auto audioParam = m_resourceManager->externalToObject<IAudioParam>(arguments[1], ObjectType::IAudioParam);
    RETURN_INVALID_REF_IF_NULL(audioParam);

	auto value = audioParam->getMaxValue();
	JsValueRef valueRef;
	RETURN_INVALID_REF_IF_JS_ERROR(JsDoubleToNumber(static_cast<double>(value), &valueRef));

	return valueRef;
}

JsValueRef AudioParamProjection::_linearRampToValueAtTime(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    auto audioParam = m_resourceManager->externalToObject<IAudioParam>(arguments[1], ObjectType::IAudioParam);
    RETURN_INVALID_REF_IF_NULL(audioParam);

	auto value = ScriptHostUtilities::GLfloatFromJsRef(arguments[2]);
	auto time = ScriptHostUtilities::GLfloatFromJsRef(arguments[3]);
	audioParam->linearRampToValueAtTime(value, time);
	return JS_INVALID_REFERENCE;
}

JsValueRef AudioParamProjection::_exponentialRampToValueAtTime(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    auto audioParam = m_resourceManager->externalToObject<IAudioParam>(arguments[1], ObjectType::IAudioParam);
    RETURN_INVALID_REF_IF_NULL(audioParam);

	auto value = ScriptHostUtilities::GLfloatFromJsRef(arguments[2]);
	auto time = ScriptHostUtilities::GLfloatFromJsRef(arguments[3]);
	audioParam->exponentialRampToValueAtTime(value, time);
	return JS_INVALID_REFERENCE;
}

JsValueRef AudioParamProjection::_setValueCurveAtTime(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

    auto audioParam = m_resourceManager->externalToObject<IAudioParam>(arguments[1], ObjectType::IAudioParam);
    RETURN_INVALID_REF_IF_NULL(audioParam);

	JsValueRef valuesRef = arguments[2];
	auto time = ScriptHostUtilities::GLfloatFromJsRef(arguments[2]);
	auto duration = ScriptHostUtilities::GLfloatFromJsRef(arguments[3]);

	// Get the length of the values array
    JsValueRef lengthRef;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetJsProperty(valuesRef, L"length", &lengthRef));
    int length = ScriptHostUtilities::GLintFromJsRef(lengthRef);

	vector<float> values(length);

	for (int i = 0; i < length; i++) {
		JsValueRef indexRef;
		RETURN_INVALID_REF_IF_JS_ERROR(JsIntToNumber(i, &indexRef));

		JsValueRef valueRef;
		RETURN_INVALID_REF_IF_JS_ERROR(JsGetIndexedProperty(valuesRef, indexRef, &valueRef));

		JsValueType valueType;
		RETURN_INVALID_REF_IF_JS_ERROR(JsGetValueType(valueRef, &valueType));

		RETURN_INVALID_REF_IF_FALSE(valueType == JsNumber);

		values.push_back(ScriptHostUtilities::GLfloatFromJsRef(valueRef));
	}

	audioParam->setValueCurveAtTime(values, time, duration);
	return JS_INVALID_REFERENCE;
}

JsValueRef AudioParamProjection::_cancelScheduledValues(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    auto audioParam = m_resourceManager->externalToObject<IAudioParam>(arguments[1], ObjectType::IAudioParam);
    RETURN_INVALID_REF_IF_NULL(audioParam);

	auto time = ScriptHostUtilities::GLfloatFromJsRef(arguments[2]);
	audioParam->cancelScheduledValues(time);
	return JS_INVALID_REFERENCE;
}

JsValueRef AudioParamProjection::_setValueAtTime(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    auto audioParam = m_resourceManager->externalToObject<IAudioParam>(arguments[1], ObjectType::IAudioParam);
    RETURN_INVALID_REF_IF_NULL(audioParam);

    auto value = ScriptHostUtilities::GLfloatFromJsRef(arguments[2]);
    auto startTime = ScriptHostUtilities::doubleFromJsRef(arguments[3]);

    audioParam->setValueAtTime(value, startTime);

    return JS_INVALID_REFERENCE;
}

JsValueRef AudioParamProjection::_setTargetAtTime(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

    auto audioParam = m_resourceManager->externalToObject<IAudioParam>(arguments[1], ObjectType::IAudioParam);
    RETURN_INVALID_REF_IF_NULL(audioParam);

    auto target = ScriptHostUtilities::doubleFromJsRef(arguments[2]);
    auto startTime = ScriptHostUtilities::doubleFromJsRef(arguments[3]);
    auto timeConstant = ScriptHostUtilities::doubleFromJsRef(arguments[4]);

    audioParam->setTargetAtTime(target, startTime, timeConstant);

    return JS_INVALID_REFERENCE;
}