#include "stdafx.h"
#include "../host-interfaces.h"
#include "holojs/private/script-host-utilities.h"
#include "include/holojs/windows/mixed-reality/spatial-controller.h"
#include <ppltasks.h>

using namespace Windows::UI::Input::Spatial;
using namespace HoloJs::MixedReality::Input;
using namespace HoloJs::Interfaces;

static const std::wstring PressedPropertyName = L"pressed";
static const std::wstring HasPositionPropertyName = L"hasPosition";
static const std::wstring HasOrientationPropertyName = L"hasOrientation";

SpatialController::SpatialController(SpatialInteractionSource ^ source)
{
    m_controller = source->Controller;
    m_source = source;
}

SpatialController::~SpatialController()
{
    if (m_orientationRef != JS_INVALID_REFERENCE) {
        JsRelease(&m_orientationRef, nullptr);
        m_orientationRef = JS_INVALID_REFERENCE;
    }

    if (m_scriptController != JS_INVALID_REFERENCE) {
        JsRelease(m_scriptController, nullptr);
        m_scriptController = JS_INVALID_REFERENCE;
    }

    for (int i = 0; i < ARRAYSIZE(m_scriptButtons); i++) {
        if (m_scriptButtons[i] != JS_INVALID_REFERENCE) {
            JsRelease(m_scriptButtons[i], nullptr);
            m_scriptButtons[i] = JS_INVALID_REFERENCE;
        }
    }
}

HRESULT SpatialController::projectToScript()
{
    static const std::wstring spatialControllerId = L"Spatial Controller";
    static const std::wstring controllerIdPropName = L"id";

    RETURN_IF_JS_ERROR(JsCreateObject(&m_scriptController));
    unsigned int refCount;
    RETURN_IF_FAILED(JsAddRef(m_scriptController, &refCount));

    RETURN_IF_FAILED(createPoseProperty());
    RETURN_IF_FAILED(createButtonsArray());

    JsValueRef axesRef;
    RETURN_IF_JS_ERROR(
        ScriptHostUtilities::SetFloat64ArrayProperty(0, &axesRef, &m_axesStoragePointer, m_scriptController, L"axes"));

    JsValueRef controllerId;

    RETURN_IF_JS_ERROR(JsPointerToString(spatialControllerId.c_str(), spatialControllerId.length(), &controllerId));
    RETURN_IF_FAILED(ScriptHostUtilities::SetJsProperty(m_scriptController, controllerIdPropName, controllerId));

    return S_OK;
}

HRESULT SpatialController::createPoseProperty()
{
    static const std::wstring posePropertyName = L"pose";

    RETURN_IF_JS_ERROR(JsCreateObject(&m_scriptPose));
    RETURN_IF_FAILED(ScriptHostUtilities::SetJsProperty(m_scriptController, posePropertyName, m_scriptPose));

    JsValueRef hasPosition, hasOrientation;
    RETURN_IF_JS_ERROR(JsGetFalseValue(&hasPosition));
    RETURN_IF_JS_ERROR(JsGetFalseValue(&hasOrientation));
    m_hadOrientationPreviously = false;
    m_hadPositionPreviously = false;
    RETURN_IF_FAILED(ScriptHostUtilities::SetJsProperty(m_scriptPose, HasPositionPropertyName, hasPosition));
    RETURN_IF_FAILED(ScriptHostUtilities::SetJsProperty(m_scriptPose, HasOrientationPropertyName, hasOrientation));

    // Controllers don't always have orientation; create an orientation array, but don't attach it to the pose object yet
    RETURN_IF_FAILED(ScriptHostUtilities::CreateTypedArray(
        JsArrayTypeFloat32, &m_orientationRef, 4, reinterpret_cast<unsigned char**>(&m_orientationVectorStoragePointer)));
    RETURN_IF_JS_ERROR(JsAddRef(m_orientationRef, nullptr));

    JsValueRef nullRef;
    RETURN_IF_JS_ERROR(JsGetNullValue(&nullRef));
    RETURN_IF_FAILED(ScriptHostUtilities::SetJsProperty(m_scriptPose, L"orientation", nullRef));

    JsValueRef positionRef;
    RETURN_IF_FAILED(ScriptHostUtilities::SetFloat32ArrayProperty(
        3, &positionRef, &m_positionVectorStoragePointer, m_scriptPose, L"position"));

    JsValueRef angularVelocityRef;
    RETURN_IF_FAILED(ScriptHostUtilities::SetFloat32ArrayProperty(
        3, &angularVelocityRef, &m_angularVelocityStoragePointer, m_scriptPose, L"angularVelocity"));

    JsValueRef linearVelocityRef;
    RETURN_IF_FAILED(ScriptHostUtilities::SetFloat32ArrayProperty(
        3, &linearVelocityRef, &m_linearVelocityStoragePointer, m_scriptPose, L"linearVelocity"));

    return S_OK;
}

HRESULT SpatialController::createButtonsArray()
{
    static const std::wstring buttonsPropertyName = L"buttons";

    JsValueRef buttonsArray;
    RETURN_IF_JS_ERROR(JsCreateArray(2, &buttonsArray));

    JsValueRef falseRef;
    RETURN_IF_JS_ERROR(JsGetFalseValue(&falseRef));

    for (int i = 0; i < ARRAYSIZE(m_scriptButtons); i++) {
        RETURN_IF_JS_ERROR(JsCreateObject(&m_scriptButtons[i]));
        RETURN_IF_JS_ERROR(ScriptHostUtilities::SetJsProperty(m_scriptButtons[i], PressedPropertyName, falseRef));

        JsValueRef indexRef;
        RETURN_IF_JS_ERROR(JsIntToNumber(i, &indexRef));
        RETURN_IF_JS_ERROR(JsSetIndexedProperty(buttonsArray, indexRef, m_scriptButtons[i]));
        RETURN_IF_JS_ERROR(JsAddRef(m_scriptButtons[i], nullptr));
    }

    RETURN_IF_FAILED(ScriptHostUtilities::SetJsProperty(m_scriptController, buttonsPropertyName, buttonsArray));

    return S_OK;
}

HRESULT SpatialController::update(SpatialInteractionSourceState ^ state, SpatialInteractionSourceLocation ^ location)
{
    const bool hasPosition = location->Position != nullptr;
    const bool hasOrientation = location->Orientation != nullptr;
    if (hasPosition) {
        auto positionFloat3 = location->Position->Value;
        CopyMemory(m_positionVectorStoragePointer, &positionFloat3.x, 3 * sizeof(float));
    }

    if (hasOrientation) {
        auto orientationFloat3 = location->Orientation->Value;
        CopyMemory(m_orientationVectorStoragePointer, &orientationFloat3.x, 4 * sizeof(float));

        if (!m_hadOrientationPreviously) {
            RETURN_IF_FAILED(ScriptHostUtilities::SetJsProperty(m_scriptPose, L"orientation", m_orientationRef));
        }
    }

    if (hasPosition != m_hadPositionPreviously) {
        JsValueRef hasPositionRef;
        RETURN_IF_JS_ERROR(JsBoolToBoolean(hasPosition, &hasPositionRef));
        RETURN_IF_FAILED(ScriptHostUtilities::SetJsProperty(m_scriptPose, HasPositionPropertyName, hasPositionRef));
        m_hadPositionPreviously = hasPosition;
    }

    if (hasOrientation != m_hadOrientationPreviously) {
        if (m_hadOrientationPreviously) {
            JsValueRef nullRef;
            RETURN_IF_JS_ERROR(JsGetNullValue(&nullRef));
            RETURN_IF_FAILED(ScriptHostUtilities::SetJsProperty(m_scriptPose, L"orientation", nullRef));
        }

        JsValueRef hasOrientationRef;
        RETURN_IF_JS_ERROR(JsBoolToBoolean(hasOrientation, &hasOrientationRef));
        RETURN_IF_FAILED(
            ScriptHostUtilities::SetJsProperty(m_scriptPose, HasOrientationPropertyName, hasOrientationRef));
        m_hadOrientationPreviously = hasOrientation;
    }

    if (location->AngularVelocity != nullptr) {
        auto angularVelocityFloat3 = location->AngularVelocity->Value;
        CopyMemory(m_angularVelocityStoragePointer, &angularVelocityFloat3.x, 3 * sizeof(float));
    }

    if (location->Velocity != nullptr) {
        auto linearVelocityFloat3 = location->Velocity->Value;
        CopyMemory(m_linearVelocityStoragePointer, &linearVelocityFloat3.x, 3 * sizeof(float));
    }

    JsValueRef buttonPressedRef;
    RETURN_IF_JS_ERROR(JsBoolToBoolean(state->IsPressed, &buttonPressedRef));

    JsValueRef buttonSelectPressedRef;
    RETURN_IF_JS_ERROR(JsBoolToBoolean(state->IsSelectPressed, &buttonSelectPressedRef));

    RETURN_IF_JS_ERROR(ScriptHostUtilities::SetJsProperty(m_scriptButtons[0], PressedPropertyName, buttonPressedRef));
    RETURN_IF_JS_ERROR(
        ScriptHostUtilities::SetJsProperty(m_scriptButtons[1], PressedPropertyName, buttonSelectPressedRef));

    return S_OK;
}
