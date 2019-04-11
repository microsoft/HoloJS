#include "audio-listener-projection.h"
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

HRESULT AudioListenerProjection::initialize()
{
    JS_PROJECTION_REGISTER(L"webaudio", L"listenerSetPosition", setPosition);
    JS_PROJECTION_REGISTER(L"webaudio", L"listenerSetOrientation", setOrientation);

    JS_PROJECTION_REGISTER(L"webaudio", L"listenerGetPositionX", positionX);
    JS_PROJECTION_REGISTER(L"webaudio", L"listenerGetPositionY", positionY);
    JS_PROJECTION_REGISTER(L"webaudio", L"listenerGetPositionZ", positionZ);

    JS_PROJECTION_REGISTER(L"webaudio", L"listenerGetForwardX", forwardX);
    JS_PROJECTION_REGISTER(L"webaudio", L"listenerGetForwardY", forwardY);
    JS_PROJECTION_REGISTER(L"webaudio", L"listenerGetForwardZ", forwardZ);

    JS_PROJECTION_REGISTER(L"webaudio", L"listenerGetUpX", upX);
    JS_PROJECTION_REGISTER(L"webaudio", L"listenerGetUpY", upY);
    JS_PROJECTION_REGISTER(L"webaudio", L"listenerGetUpZ", upZ);

    return S_OK;
}

JsValueRef AudioListenerProjection::_setPosition(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

    auto listenerNode = m_resourceManager->externalToObject<IAudioListener>(arguments[1], ObjectType::IAudioListener);
    RETURN_INVALID_REF_IF_NULL(listenerNode);

    const auto x = ScriptHostUtilities::GLfloatFromJsRef(arguments[2]);
    const auto y = ScriptHostUtilities::GLfloatFromJsRef(arguments[3]);
    const auto z = ScriptHostUtilities::GLfloatFromJsRef(arguments[4]);

    listenerNode->setPosition(x, y, z);

    return JS_INVALID_REFERENCE;
}
JsValueRef AudioListenerProjection::_setOrientation(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 8);

    auto listenerNode = m_resourceManager->externalToObject<IAudioListener>(arguments[1], ObjectType::IAudioListener);
    RETURN_INVALID_REF_IF_NULL(listenerNode);

    const auto x = ScriptHostUtilities::GLfloatFromJsRef(arguments[2]);
    const auto y = ScriptHostUtilities::GLfloatFromJsRef(arguments[3]);
    const auto z = ScriptHostUtilities::GLfloatFromJsRef(arguments[4]);
    const auto upX = ScriptHostUtilities::GLfloatFromJsRef(arguments[5]);
    const auto upY = ScriptHostUtilities::GLfloatFromJsRef(arguments[6]);
    const auto upZ = ScriptHostUtilities::GLfloatFromJsRef(arguments[7]);

    listenerNode->setOrientation(x, y, z, upX, upY, upZ);

    return JS_INVALID_REFERENCE;
}

JsValueRef AudioListenerProjection::getParamRefByType(JsValueRef listenerRef, AudioParamType type)
{
    auto listener = m_resourceManager->externalToObject<IAudioListener>(listenerRef, ObjectType::IAudioListener);
    RETURN_INVALID_REF_IF_NULL(listener);

    IAudioParam* param;

    switch (type) {
        case AudioParamType::ForwardX:
            param = listener->forwardX();
            break;

        case AudioParamType::ForwardY:
            param = listener->forwardY();
            break;

        case AudioParamType::ForwardZ:
            param = listener->forwardZ();
            break;

        case AudioParamType::PositionX:
            param = listener->positionX();
            break;

        case AudioParamType::PositionY:
            param = listener->positionY();
            break;

        case AudioParamType::PositionZ:
            param = listener->positionZ();
            break;

        case AudioParamType::UpX:
            param = listener->upX();
            break;

        case AudioParamType::UpY:
            param = listener->upY();
            break;

        case AudioParamType::UpZ:
            param = listener->upZ();
            break;

        default:
            param = nullptr;
            break;
    }

    RETURN_INVALID_REF_IF_NULL(param);

    return m_resourceManager->objectToDirectExternal<IAudioParam>(
        param, ObjectType::IAudioParam, ObjectType::IAudioNode);
}

JsValueRef AudioListenerProjection::_positionX(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);
    return getParamRefByType(arguments[1], AudioParamType::PositionX);
}

JsValueRef AudioListenerProjection::_positionY(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);
    return getParamRefByType(arguments[1], AudioParamType::PositionY);
}
JsValueRef AudioListenerProjection::_positionZ(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);
    return getParamRefByType(arguments[1], AudioParamType::PositionZ);
}

JsValueRef AudioListenerProjection::_forwardX(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);
    return getParamRefByType(arguments[1], AudioParamType::ForwardX);
}

JsValueRef AudioListenerProjection::_forwardY(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);
    return getParamRefByType(arguments[1], AudioParamType::ForwardY);
}

JsValueRef AudioListenerProjection::_forwardZ(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);
    return getParamRefByType(arguments[1], AudioParamType::ForwardZ);
}

JsValueRef AudioListenerProjection::_upX(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);
    return getParamRefByType(arguments[1], AudioParamType::UpX);
}

JsValueRef AudioListenerProjection::_upY(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);
    return getParamRefByType(arguments[1], AudioParamType::UpY);
}

JsValueRef AudioListenerProjection::_upZ(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);
    return getParamRefByType(arguments[1], AudioParamType::UpZ);
}