#include "pch.h"
#include "WindowElement.h"
#include "InputInterface.h"
#include "ScriptErrorHandling.h"
#include "ScriptHostUtilities.h"
#include <WindowsNumerics.h>

using namespace HologramJS::API;
using namespace HologramJS::Utilities;
using namespace HologramJS::Input;
using namespace std;

WindowElement::WindowElement() {}

WindowElement::~WindowElement() { Close(); }

void WindowElement::Close()
{
    if (m_callbackFunction != JS_INVALID_REFERENCE) {
        unsigned int refCount;
        JsRelease(m_callbackFunction, &refCount);
        m_callbackFunction = JS_INVALID_REFERENCE;
    }
}

bool WindowElement::Initialize()
{
    RETURN_IF_FALSE(
        ScriptHostUtilities::ProjectFunction(L"getWidth", L"window", getWidthStatic, this, &m_getWidthFunction));
    RETURN_IF_FALSE(
        ScriptHostUtilities::ProjectFunction(L"getHeight", L"window", getHeightStatic, this, &m_getHeightFunction));
    RETURN_IF_FALSE(
        ScriptHostUtilities::ProjectFunction(L"getBaseUrl", L"window", getBaseUrlStatic, this, &m_getBaseUrlFunction));
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(
        L"setCallback", L"window", setCallbackStatic, this, &m_setCallbackFunction));
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(
        L"requestSpatialMappingData", L"window", requestSpatialMappingStatic, this, &m_setCallbackFunction));

    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(
        L"addEventListener", L"input", addEventListenerStatic, this, &m_addEventListener));

    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(
        L"removeEventListener", L"input", removeEventListenerStatic, this, &m_removeEventListener));

    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(
        L"setVoiceCommands", L"input", setVoiceCommandsStatic, this, &m_setVoiceCommands));

    RETURN_IF_FALSE(CreateViewMatrixStorageAndScriptProjection());

    RETURN_IF_JS_ERROR(JsIntToNumber(static_cast<int>(NativeToScriptInputType::Resize), &m_resizeEventType));
    RETURN_IF_JS_ERROR(JsIntToNumber(static_cast<int>(NativeToScriptInputType::VSync), &m_vsyncEventType));

    return true;
}

JsValueRef WindowElement::getBaseUrl(JsValueRef* arguments, unsigned short argumentCount)
{
    JsValueRef baseUrlValue;
    RETURN_INVALID_REF_IF_JS_ERROR(JsPointerToString(m_baseUrl.c_str(), m_baseUrl.length(), &baseUrlValue));
    return baseUrlValue;
}

JsValueRef WindowElement::setCallback(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    JsValueRef callback = arguments[1];

    unsigned int refCount;
    RETURN_INVALID_REF_IF_JS_ERROR(JsAddRef(callback, &refCount));

    m_callbackFunction = callback;

    m_keyboardInput.SetScriptCallback(callback);
    m_mouseInput.SetScriptCallback(callback);
    m_spatialInput.SetScriptCallback(callback);
    m_spatialMapping.SetScriptCallback(callback);
    m_voiceInput.SetScriptCallback(callback);

    return JS_INVALID_REFERENCE;
}

JsValueRef WindowElement::getWidth(JsValueRef* arguments, unsigned short argumentCount)
{
    JsValueRef widthValue;
    RETURN_INVALID_REF_IF_JS_ERROR(JsIntToNumber(m_width, &widthValue));
    return widthValue;
}

JsValueRef WindowElement::getHeight(JsValueRef* arguments, unsigned short argumentCount)
{
    JsValueRef heightValue;
    RETURN_NULL_IF_JS_ERROR(JsIntToNumber(m_height, &heightValue));
    return heightValue;
}

void WindowElement::Resize(int width, int height)
{
    const bool shouldFireResize = (m_width != width || m_height != height);

    if (shouldFireResize) {
        m_width = width;
        m_height = height;

        if (m_callbackFunction != JS_INVALID_REFERENCE) {
            JsValueRef parameters[2];
            parameters[0] = m_callbackFunction;
            parameters[1] = m_resizeEventType;

            JsValueRef result;
            HANDLE_EXCEPTION_IF_JS_ERROR(
                JsCallFunction(m_callbackFunction, parameters, ARRAYSIZE(parameters), &result));
        }
    }
}

void WindowElement::VSync(float4x4 midViewMatrix,
                          float4x4 midProjectionMatrix,
                          float4x4 leftViewMatrix,
                          float4x4 leftProjectionMatrix,
                          float4x4 rightViewMatrix,
                          float4x4 rightProjectionMatrix)
{
    // Workaround for RS1: spatial input events can be delivered on non-UI threads; in this case m_spatialInput
    // queued events internally and since now we're on the UI thread we drain them
    m_spatialInput.DrainQueuedSpatialInputEvents();

    m_spatialMapping.ProcessOneSpatialMappingDataUpdate();

    if (m_callbackFunction != JS_INVALID_REFERENCE) {
        if (m_viewMatrixMidStoragePointer != nullptr) {
            // Update the view matrix inside the JSVM
            CopyMemory(m_viewMatrixMidStoragePointer, &midViewMatrix.m11, 16 * sizeof(float));
        }
        if (m_viewMatrixLeftStoragePointer != nullptr) {
            // Update the view matrix inside the JSVM
            CopyMemory(m_viewMatrixLeftStoragePointer, &leftViewMatrix.m11, 16 * sizeof(float));
        }
        if (m_viewMatrixRightStoragePointer != nullptr) {
            // Update the view matrix inside the JSVM
            CopyMemory(m_viewMatrixRightStoragePointer, &rightViewMatrix.m11, 16 * sizeof(float));
        }

        if (m_projectionMatrixMidStoragePointer != nullptr) {
            // Update the projection matrix inside the JSVM
            CopyMemory(m_projectionMatrixMidStoragePointer, &midProjectionMatrix.m11, 16 * sizeof(float));
        }
        if (m_projectionMatrixLeftStoragePointer != nullptr) {
            // Update the projection matrix inside the JSVM
            CopyMemory(m_projectionMatrixLeftStoragePointer, &leftProjectionMatrix.m11, 16 * sizeof(float));
        }
        if (m_projectionMatrixRightStoragePointer != nullptr) {
            // Update the projection matrix inside the JSVM
            CopyMemory(m_projectionMatrixRightStoragePointer, &rightProjectionMatrix.m11, 16 * sizeof(float));
        }

        if (m_cameraPositionMidStoragePointer != nullptr) {
            // Inverse the view matrix in order to find back the translation equivalent to the position
            // in the last column.
            bool success = invert(midViewMatrix, &m_inverseMidViewMatrix);
            if (success) {
                // Update the camera Position inside the JSVM
                CopyMemory(m_cameraPositionMidStoragePointer, &m_inverseMidViewMatrix.m41, 4 * sizeof(float));
            }
        }
        if (m_cameraPositionLeftStoragePointer != nullptr) {
            // Inverse the view matrix in order to find back the translation equivalent to the position
            // in the last column.
            bool success = invert(leftViewMatrix, &m_inverseLeftViewMatrix);
            if (success) {
                // Update the camera Position inside the JSVM
                CopyMemory(m_cameraPositionLeftStoragePointer, &m_inverseLeftViewMatrix.m41, 4 * sizeof(float));
            }
        }
        if (m_cameraPositionRightStoragePointer != nullptr) {
            // Inverse the view matrix in order to find back the translation equivalent to the position
            // in the last column.
            bool success = invert(rightViewMatrix, &m_inverseRightViewMatrix);
            if (success) {
                // Update the camera Position inside the JSVM
                CopyMemory(m_cameraPositionRightStoragePointer, &m_inverseRightViewMatrix.m41, 4 * sizeof(float));
            }
        }

        JsValueRef parameters[2];
        parameters[0] = m_callbackFunction;
        parameters[1] = m_vsyncEventType;
        JsValueRef result;
        HANDLE_EXCEPTION_IF_JS_ERROR(JsCallFunction(m_callbackFunction, parameters, ARRAYSIZE(parameters), &result));
    }
}

bool WindowElement::CreateViewMatrixStorageAndScriptProjection()
{
    if (m_viewMatrixMidScriptProjection == JS_INVALID_REFERENCE) {
        RETURN_IF_JS_ERROR(
            JsCreateTypedArray(JsTypedArrayType::JsArrayTypeFloat32, nullptr, 0, 16, &m_viewMatrixMidScriptProjection));
        RETURN_IF_JS_ERROR(JsCreateTypedArray(
            JsTypedArrayType::JsArrayTypeFloat32, nullptr, 0, 4, &m_cameraPositionMidScriptProjection));
        RETURN_IF_JS_ERROR(JsCreateTypedArray(
            JsTypedArrayType::JsArrayTypeFloat32, nullptr, 0, 16, &m_projectionMatrixMidScriptProjection));

        RETURN_IF_JS_ERROR(JsCreateTypedArray(
            JsTypedArrayType::JsArrayTypeFloat32, nullptr, 0, 16, &m_viewMatrixLeftScriptProjection));
        RETURN_IF_JS_ERROR(JsCreateTypedArray(
            JsTypedArrayType::JsArrayTypeFloat32, nullptr, 0, 4, &m_cameraPositionLeftScriptProjection));
        RETURN_IF_JS_ERROR(JsCreateTypedArray(
            JsTypedArrayType::JsArrayTypeFloat32, nullptr, 0, 16, &m_projectionMatrixLeftScriptProjection));

        RETURN_IF_JS_ERROR(JsCreateTypedArray(
            JsTypedArrayType::JsArrayTypeFloat32, nullptr, 0, 16, &m_viewMatrixRightScriptProjection));
        RETURN_IF_JS_ERROR(JsCreateTypedArray(
            JsTypedArrayType::JsArrayTypeFloat32, nullptr, 0, 4, &m_cameraPositionRightScriptProjection));
        RETURN_IF_JS_ERROR(JsCreateTypedArray(
            JsTypedArrayType::JsArrayTypeFloat32, nullptr, 0, 16, &m_projectionMatrixRightScriptProjection));

        JsValueRef globalObject;
        RETURN_IF_JS_ERROR(JsGetGlobalObject(&globalObject));

        JsValueRef holographicObject;
        RETURN_IF_FALSE(ScriptHostUtilities::GetJsProperty(globalObject, L"holographic", &holographicObject));

        JsValueRef nativeInterface;
        RETURN_IF_FALSE(ScriptHostUtilities::GetJsProperty(holographicObject, L"nativeInterface", &nativeInterface));

        JsValueRef windowObject;
        RETURN_IF_FALSE(ScriptHostUtilities::GetJsProperty(nativeInterface, L"window", &windowObject));

        JsPropertyIdRef viewMatrixId;
        RETURN_IF_JS_ERROR(JsGetPropertyIdFromName(L"holographicViewMatrixMid", &viewMatrixId));
        RETURN_IF_JS_ERROR(JsSetProperty(windowObject, viewMatrixId, m_viewMatrixMidScriptProjection, true));
        RETURN_IF_JS_ERROR(JsGetPropertyIdFromName(L"holographicViewMatrixLeft", &viewMatrixId));
        RETURN_IF_JS_ERROR(JsSetProperty(windowObject, viewMatrixId, m_viewMatrixLeftScriptProjection, true));
        RETURN_IF_JS_ERROR(JsGetPropertyIdFromName(L"holographicViewMatrixRight", &viewMatrixId));
        RETURN_IF_JS_ERROR(JsSetProperty(windowObject, viewMatrixId, m_viewMatrixRightScriptProjection, true));

        JsPropertyIdRef cameraPositionId;
        RETURN_IF_JS_ERROR(JsGetPropertyIdFromName(L"holographicCameraPositionMid", &cameraPositionId));
        RETURN_IF_JS_ERROR(JsSetProperty(windowObject, cameraPositionId, m_cameraPositionMidScriptProjection, true));
        RETURN_IF_JS_ERROR(JsGetPropertyIdFromName(L"holographicCameraPositionLeft", &cameraPositionId));
        RETURN_IF_JS_ERROR(JsSetProperty(windowObject, cameraPositionId, m_cameraPositionLeftScriptProjection, true));
        RETURN_IF_JS_ERROR(JsGetPropertyIdFromName(L"holographicCameraPositionRight", &cameraPositionId));
        RETURN_IF_JS_ERROR(JsSetProperty(windowObject, cameraPositionId, m_cameraPositionRightScriptProjection, true));

        JsPropertyIdRef projectionMatrixId;
        RETURN_IF_JS_ERROR(JsGetPropertyIdFromName(L"holographicProjectionMatrixMid", &projectionMatrixId));
        RETURN_IF_JS_ERROR(
            JsSetProperty(windowObject, projectionMatrixId, m_projectionMatrixMidScriptProjection, true));
        RETURN_IF_JS_ERROR(JsGetPropertyIdFromName(L"holographicProjectionMatrixLeft", &projectionMatrixId));
        RETURN_IF_JS_ERROR(
            JsSetProperty(windowObject, projectionMatrixId, m_projectionMatrixLeftScriptProjection, true));
        RETURN_IF_JS_ERROR(JsGetPropertyIdFromName(L"holographicProjectionMatrixRight", &projectionMatrixId));
        RETURN_IF_JS_ERROR(
            JsSetProperty(windowObject, projectionMatrixId, m_projectionMatrixRightScriptProjection, true));

        unsigned int bufferLength;
        JsTypedArrayType type;
        int elementSize;
        RETURN_IF_JS_ERROR(JsGetTypedArrayStorage(m_viewMatrixMidScriptProjection,
                                                  (ChakraBytePtr*)&m_viewMatrixMidStoragePointer,
                                                  &bufferLength,
                                                  &type,
                                                  &elementSize));
        RETURN_IF_JS_ERROR(JsGetTypedArrayStorage(m_viewMatrixLeftScriptProjection,
                                                  (ChakraBytePtr*)&m_viewMatrixLeftStoragePointer,
                                                  &bufferLength,
                                                  &type,
                                                  &elementSize));
        RETURN_IF_JS_ERROR(JsGetTypedArrayStorage(m_viewMatrixRightScriptProjection,
                                                  (ChakraBytePtr*)&m_viewMatrixRightStoragePointer,
                                                  &bufferLength,
                                                  &type,
                                                  &elementSize));
        RETURN_IF_JS_ERROR(JsGetTypedArrayStorage(m_cameraPositionMidScriptProjection,
                                                  (ChakraBytePtr*)&m_cameraPositionMidStoragePointer,
                                                  &bufferLength,
                                                  &type,
                                                  &elementSize));
        RETURN_IF_JS_ERROR(JsGetTypedArrayStorage(m_cameraPositionLeftScriptProjection,
                                                  (ChakraBytePtr*)&m_cameraPositionLeftStoragePointer,
                                                  &bufferLength,
                                                  &type,
                                                  &elementSize));
        RETURN_IF_JS_ERROR(JsGetTypedArrayStorage(m_cameraPositionRightScriptProjection,
                                                  (ChakraBytePtr*)&m_cameraPositionRightStoragePointer,
                                                  &bufferLength,
                                                  &type,
                                                  &elementSize));
        RETURN_IF_JS_ERROR(JsGetTypedArrayStorage(m_projectionMatrixMidScriptProjection,
                                                  (ChakraBytePtr*)&m_projectionMatrixMidStoragePointer,
                                                  &bufferLength,
                                                  &type,
                                                  &elementSize));
        RETURN_IF_JS_ERROR(JsGetTypedArrayStorage(m_projectionMatrixLeftScriptProjection,
                                                  (ChakraBytePtr*)&m_projectionMatrixLeftStoragePointer,
                                                  &bufferLength,
                                                  &type,
                                                  &elementSize));
        RETURN_IF_JS_ERROR(JsGetTypedArrayStorage(m_projectionMatrixRightScriptProjection,
                                                  (ChakraBytePtr*)&m_projectionMatrixRightStoragePointer,
                                                  &bufferLength,
                                                  &type,
                                                  &elementSize));
    }

    return true;
}

JsValueRef WindowElement::requestSpatialMapping(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

    const auto extentX = ScriptHostUtilities::GLfloatFromJsRef(arguments[1]);
    const auto extentY = ScriptHostUtilities::GLfloatFromJsRef(arguments[2]);
    const auto extentZ = ScriptHostUtilities::GLfloatFromJsRef(arguments[3]);
    const auto trianglesPerCubicMeter = ScriptHostUtilities::GLintFromJsRef(arguments[4]);

    return m_spatialMapping.GetSpatialData(extentX, extentY, extentZ, trianglesPerCubicMeter);
}

JsValueRef WindowElement::addEventListener(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    wstring type;
    RETURN_INVALID_REF_IF_FALSE(ScriptHostUtilities::GetString(arguments[1], type));

    if (!m_mouseInput.AddEventListener(type)) {
        if (!m_keyboardInput.AddEventListener(type)) {
            if (!m_spatialInput.AddEventListener(type)) {
                m_voiceInput.AddEventListener(type);
            }
        }
    }

    return JS_INVALID_REFERENCE;
}

JsValueRef WindowElement::removeEventListener(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    wstring type;
    RETURN_INVALID_REF_IF_FALSE(ScriptHostUtilities::GetString(arguments[1], type));

    if (!m_mouseInput.RemoveEventListener(type)) {
        if (!m_keyboardInput.RemoveEventListener(type)) {
            if (!m_spatialInput.RemoveEventListener(type)) {
                m_voiceInput.RemoveEventListener(type);
            }
        }
    }

    return JS_INVALID_REFERENCE;
}

JsValueRef WindowElement::setVoiceCommands(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    JsValueRef commandsArray = arguments[1];

    JsValueRef lengthProperty;
    JsPropertyIdRef lengthPropertyName;
    RETURN_INVALID_REF_IF_JS_ERROR(JsGetPropertyIdFromName(L"length", &lengthPropertyName));
    RETURN_INVALID_REF_IF_JS_ERROR(JsGetProperty(commandsArray, lengthPropertyName, &lengthProperty));

    JsValueType type;
    JsGetValueType(lengthProperty, &type);

    int length;
    RETURN_INVALID_REF_IF_JS_ERROR(JsNumberToInt(lengthProperty, &length));

    vector<wstring> commands = vector<wstring>();

    for (int i = 0; i < length; i++) {
        JsValueRef commandValue;
        JsValueRef index;
        RETURN_INVALID_REF_IF_JS_ERROR(JsIntToNumber(i, &index));
        RETURN_INVALID_REF_IF_JS_ERROR(JsGetIndexedProperty(commandsArray, index, &commandValue));

        wstring command;
        RETURN_INVALID_REF_IF_FALSE(ScriptHostUtilities::GetString(commandValue, command));
        commands.emplace_back(move(command));
    }

    m_voiceInput.SetVoiceCommands(std::move(commands));

    return JS_INVALID_REFERENCE;
}
