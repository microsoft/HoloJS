#include "panner-node-projection.h"
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

HRESULT PannerNodeProjection::initialize()
{
    JS_PROJECTION_REGISTER(L"webaudioPanner", L"setPosition", setPosition);
    JS_PROJECTION_REGISTER(L"webaudioPanner", L"setOrientation", setOrientation);
    JS_PROJECTION_REGISTER(L"webaudioPanner", L"setVelocity", setVelocity);

    JS_PROJECTION_REGISTER(L"webaudioPanner", L"setRefDistance", setRefDistance);
    JS_PROJECTION_REGISTER(L"webaudioPanner", L"getRefDistance", getRefDistance);

    JS_PROJECTION_REGISTER(L"webaudioPanner", L"getPanningModel", getPanningModel);
    JS_PROJECTION_REGISTER(L"webaudioPanner", L"setPanningModel", setPanningModel);

    JS_PROJECTION_REGISTER(L"webaudioPanner", L"getDistanceModel", getDistanceModel);
    JS_PROJECTION_REGISTER(L"webaudioPanner", L"setDistanceModel", setDistanceModel);

    JS_PROJECTION_REGISTER(L"webaudioPanner", L"positionX", positionX);
    JS_PROJECTION_REGISTER(L"webaudioPanner", L"positionY", positionY);
    JS_PROJECTION_REGISTER(L"webaudioPanner", L"positionZ", positionZ);

    JS_PROJECTION_REGISTER(L"webaudioPanner", L"orientationX", orientationX);
    JS_PROJECTION_REGISTER(L"webaudioPanner", L"orientationY", orientationY);
    JS_PROJECTION_REGISTER(L"webaudioPanner", L"orientationZ", orientationZ);

    JS_PROJECTION_REGISTER(L"webaudioPanner", L"getMaxDistance", getMaxDistance);
    JS_PROJECTION_REGISTER(L"webaudioPanner", L"setMaxDistance", setMaxDistance);

    JS_PROJECTION_REGISTER(L"webaudioPanner", L"getRolloffFactor", getRolloffFactor);
    JS_PROJECTION_REGISTER(L"webaudioPanner", L"setRolloffFactor", setRolloffFactor);

    JS_PROJECTION_REGISTER(L"webaudioPanner", L"getConeInnerAngle", getConeInnerAngle);
    JS_PROJECTION_REGISTER(L"webaudioPanner", L"setConeInnerAngle", setConeInnerAngle);

    JS_PROJECTION_REGISTER(L"webaudioPanner", L"getConeOuterAngle", getConeOuterAngle);
    JS_PROJECTION_REGISTER(L"webaudioPanner", L"setConeOuterAngle", setConeOuterAngle);

    JS_PROJECTION_REGISTER(L"webaudioPanner", L"getConeOuterGain", getConeOuterGain);
    JS_PROJECTION_REGISTER(L"webaudioPanner", L"setConeOuterGain", setConeOuterGain);

    return S_OK;
}

JsValueRef PannerNodeProjection::_getPanningModel(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    auto pannerNode = m_resourceManager->externalToObject<IPannerNode>(arguments[1], ObjectType::IPannerNode);
    RETURN_INVALID_REF_IF_NULL(pannerNode);

    auto panningModel = pannerNode->getPanningModel();

    JsValueRef panningModelRef;
    RETURN_INVALID_REF_IF_JS_ERROR(JsPointerToString(panningModel.c_str(), panningModel.length(), &panningModelRef));
    return panningModelRef;
}

JsValueRef PannerNodeProjection::_setPanningModel(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    auto pannerNode = m_resourceManager->externalToObject<IPannerNode>(arguments[1], ObjectType::IPannerNode);
    RETURN_INVALID_REF_IF_NULL(pannerNode);

    wstring panningModel;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetString(arguments[2], panningModel));

    pannerNode->setPanningModel(panningModel);

    return JS_INVALID_REFERENCE;
}

JsValueRef PannerNodeProjection::_getDistanceModel(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    auto pannerNode = m_resourceManager->externalToObject<IPannerNode>(arguments[1], ObjectType::IPannerNode);
    RETURN_INVALID_REF_IF_NULL(pannerNode);

    auto distanceModel = pannerNode->getDistanceModel();

    JsValueRef distanceModelRef;
    RETURN_INVALID_REF_IF_JS_ERROR(JsPointerToString(distanceModel.c_str(), distanceModel.length(), &distanceModelRef));
    return distanceModelRef;
}

JsValueRef PannerNodeProjection::_setDistanceModel(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    auto pannerNode = m_resourceManager->externalToObject<IPannerNode>(arguments[1], ObjectType::IPannerNode);
    RETURN_INVALID_REF_IF_NULL(pannerNode);

    wstring distanceModel;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetString(arguments[2], distanceModel));

    pannerNode->setDistanceModel(distanceModel);

    return JS_INVALID_REFERENCE;
}

JsValueRef PannerNodeProjection::_positionX(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    auto pannerNode = m_resourceManager->externalToObject<IPannerNode>(arguments[1], ObjectType::IPannerNode);
    RETURN_INVALID_REF_IF_NULL(pannerNode);

    auto positionX = pannerNode->positionX();
    RETURN_INVALID_REF_IF_NULL(positionX);

    return m_resourceManager->objectToDirectExternal(positionX, ObjectType::IAudioParam, ObjectType::IAudioNode);
}

JsValueRef PannerNodeProjection::_positionY(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    auto pannerNode = m_resourceManager->externalToObject<IPannerNode>(arguments[1], ObjectType::IPannerNode);
    RETURN_INVALID_REF_IF_NULL(pannerNode);

    auto positionY = pannerNode->positionY();
    RETURN_INVALID_REF_IF_NULL(positionY);

    return m_resourceManager->objectToDirectExternal(positionY, ObjectType::IAudioParam, ObjectType::IAudioNode);
}

JsValueRef PannerNodeProjection::_positionZ(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    auto pannerNode = m_resourceManager->externalToObject<IPannerNode>(arguments[1], ObjectType::IPannerNode);
    RETURN_INVALID_REF_IF_NULL(pannerNode);

    auto positionZ = pannerNode->positionZ();
    RETURN_INVALID_REF_IF_NULL(positionZ);

    return m_resourceManager->objectToDirectExternal(positionZ, ObjectType::IAudioParam, ObjectType::IAudioNode);
}

JsValueRef PannerNodeProjection::_orientationX(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    auto pannerNode = m_resourceManager->externalToObject<IPannerNode>(arguments[1], ObjectType::IPannerNode);
    RETURN_INVALID_REF_IF_NULL(pannerNode);

    auto orientationX = pannerNode->orientationX();
    RETURN_INVALID_REF_IF_NULL(orientationX);

    return m_resourceManager->objectToDirectExternal(orientationX, ObjectType::IAudioParam, ObjectType::IAudioNode);
}

JsValueRef PannerNodeProjection::_orientationY(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    auto pannerNode = m_resourceManager->externalToObject<IPannerNode>(arguments[1], ObjectType::IPannerNode);
    RETURN_INVALID_REF_IF_NULL(pannerNode);

    auto orientationY = pannerNode->orientationY();
    RETURN_INVALID_REF_IF_NULL(orientationY);

    return m_resourceManager->objectToDirectExternal(orientationY, ObjectType::IAudioParam, ObjectType::IAudioNode);
}

JsValueRef PannerNodeProjection::_orientationZ(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    auto pannerNode = m_resourceManager->externalToObject<IPannerNode>(arguments[1], ObjectType::IPannerNode);
    RETURN_INVALID_REF_IF_NULL(pannerNode);

    auto orientationZ = pannerNode->orientationZ();
    RETURN_INVALID_REF_IF_NULL(orientationZ);

    return m_resourceManager->objectToDirectExternal(orientationZ, ObjectType::IAudioParam, ObjectType::IAudioNode);
}

JsValueRef PannerNodeProjection::_getMaxDistance(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    auto pannerNode = m_resourceManager->externalToObject<IPannerNode>(arguments[1], ObjectType::IPannerNode);
    RETURN_INVALID_REF_IF_NULL(pannerNode);

    auto value = pannerNode->getMaxDistance();
    JsValueRef valueRef;
    RETURN_INVALID_REF_IF_JS_ERROR(JsDoubleToNumber(static_cast<double>(value), &valueRef));

    return valueRef;
}

JsValueRef PannerNodeProjection::_setMaxDistance(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    auto pannerNode = m_resourceManager->externalToObject<IPannerNode>(arguments[1], ObjectType::IPannerNode);
    RETURN_INVALID_REF_IF_NULL(pannerNode);

    auto value = ScriptHostUtilities::GLfloatFromJsRef(arguments[2]);
    pannerNode->setMaxDistance(value);

    return JS_INVALID_REFERENCE;
}

JsValueRef PannerNodeProjection::_getRolloffFactor(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    auto pannerNode = m_resourceManager->externalToObject<IPannerNode>(arguments[1], ObjectType::IPannerNode);
    RETURN_INVALID_REF_IF_NULL(pannerNode);

    auto value = pannerNode->getRolloffFactor();
    JsValueRef valueRef;
    RETURN_INVALID_REF_IF_JS_ERROR(JsDoubleToNumber(static_cast<double>(value), &valueRef));

    return valueRef;
}

JsValueRef PannerNodeProjection::_setRolloffFactor(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    auto pannerNode = m_resourceManager->externalToObject<IPannerNode>(arguments[1], ObjectType::IPannerNode);
    RETURN_INVALID_REF_IF_NULL(pannerNode);

    auto value = ScriptHostUtilities::GLfloatFromJsRef(arguments[2]);
    pannerNode->setRolloffFactor(value);

    return JS_INVALID_REFERENCE;
}

JsValueRef PannerNodeProjection::_getConeInnerAngle(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    auto pannerNode = m_resourceManager->externalToObject<IPannerNode>(arguments[1], ObjectType::IPannerNode);
    RETURN_INVALID_REF_IF_NULL(pannerNode);

    auto value = pannerNode->getConeInnerAngle();
    JsValueRef valueRef;
    RETURN_INVALID_REF_IF_JS_ERROR(JsDoubleToNumber(static_cast<double>(value), &valueRef));

    return valueRef;
}

JsValueRef PannerNodeProjection::_setConeInnerAngle(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    auto pannerNode = m_resourceManager->externalToObject<IPannerNode>(arguments[1], ObjectType::IPannerNode);
    RETURN_INVALID_REF_IF_NULL(pannerNode);

    auto value = ScriptHostUtilities::GLfloatFromJsRef(arguments[2]);
    pannerNode->setConeInnerAngle(value);

    return JS_INVALID_REFERENCE;
}

JsValueRef PannerNodeProjection::_getConeOuterAngle(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    auto pannerNode = m_resourceManager->externalToObject<IPannerNode>(arguments[1], ObjectType::IPannerNode);
    RETURN_INVALID_REF_IF_NULL(pannerNode);

    auto value = pannerNode->getConeOuterAngle();
    JsValueRef valueRef;
    RETURN_INVALID_REF_IF_JS_ERROR(JsDoubleToNumber(static_cast<double>(value), &valueRef));

    return valueRef;
}

JsValueRef PannerNodeProjection::_setConeOuterAngle(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    auto pannerNode = m_resourceManager->externalToObject<IPannerNode>(arguments[1], ObjectType::IPannerNode);
    RETURN_INVALID_REF_IF_NULL(pannerNode);

    auto value = ScriptHostUtilities::GLfloatFromJsRef(arguments[2]);
    pannerNode->setConeOuterAngle(value);

    return JS_INVALID_REFERENCE;
}

JsValueRef PannerNodeProjection::_getConeOuterGain(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    auto pannerNode = m_resourceManager->externalToObject<IPannerNode>(arguments[1], ObjectType::IPannerNode);
    RETURN_INVALID_REF_IF_NULL(pannerNode);

    auto value = pannerNode->getConeOuterGain();
    JsValueRef valueRef;
    RETURN_INVALID_REF_IF_JS_ERROR(JsDoubleToNumber(static_cast<double>(value), &valueRef));

    return valueRef;
}

JsValueRef PannerNodeProjection::_setConeOuterGain(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    auto pannerNode = m_resourceManager->externalToObject<IPannerNode>(arguments[1], ObjectType::IPannerNode);
    RETURN_INVALID_REF_IF_NULL(pannerNode);

    auto value = ScriptHostUtilities::GLfloatFromJsRef(arguments[2]);
    pannerNode->setConeOuterGain(value);

    return JS_INVALID_REFERENCE;
}

JsValueRef PannerNodeProjection::_setPosition(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

    auto pannerNode = m_resourceManager->externalToObject<IPannerNode>(arguments[1], ObjectType::IPannerNode);
    RETURN_INVALID_REF_IF_NULL(pannerNode);

    const auto x = ScriptHostUtilities::GLfloatFromJsRef(arguments[2]);
    const auto y = ScriptHostUtilities::GLfloatFromJsRef(arguments[3]);
    const auto z = ScriptHostUtilities::GLfloatFromJsRef(arguments[4]);

    pannerNode->setPosition(x, y, z);

    return JS_INVALID_REFERENCE;
}
JsValueRef PannerNodeProjection::_setOrientation(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

    auto pannerNode = m_resourceManager->externalToObject<IPannerNode>(arguments[1], ObjectType::IPannerNode);
    RETURN_INVALID_REF_IF_NULL(pannerNode);

    const auto x = ScriptHostUtilities::GLfloatFromJsRef(arguments[2]);
    const auto y = ScriptHostUtilities::GLfloatFromJsRef(arguments[3]);
    const auto z = ScriptHostUtilities::GLfloatFromJsRef(arguments[4]);

    pannerNode->setOrientation(x, y, z);

    return JS_INVALID_REFERENCE;
}

JsValueRef PannerNodeProjection::_setVelocity(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

    auto pannerNode = m_resourceManager->externalToObject<IPannerNode>(arguments[1], ObjectType::IPannerNode);
    RETURN_INVALID_REF_IF_NULL(pannerNode);

    const auto x = ScriptHostUtilities::GLfloatFromJsRef(arguments[2]);
    const auto y = ScriptHostUtilities::GLfloatFromJsRef(arguments[3]);
    const auto z = ScriptHostUtilities::GLfloatFromJsRef(arguments[4]);

    pannerNode->setVelocity(x, y, z);

    return JS_INVALID_REFERENCE;
}

JsValueRef PannerNodeProjection::_getRefDistance(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    auto pannerNode = m_resourceManager->externalToObject<IPannerNode>(arguments[1], ObjectType::IPannerNode);
    RETURN_INVALID_REF_IF_NULL(pannerNode);

    auto refDistance = pannerNode->getRefDistance();

    JsValueRef refDistanceRef;
    RETURN_INVALID_REF_IF_JS_ERROR(JsDoubleToNumber(static_cast<double>(refDistance), &refDistanceRef));

    return refDistanceRef;
}

JsValueRef PannerNodeProjection::_setRefDistance(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    auto pannerNode = m_resourceManager->externalToObject<IPannerNode>(arguments[1], ObjectType::IPannerNode);
    RETURN_INVALID_REF_IF_NULL(pannerNode);

    const auto refDistance = ScriptHostUtilities::GLfloatFromJsRef(arguments[2]);

    pannerNode->setRefDistance(refDistance);
    return JS_INVALID_REFERENCE;
}