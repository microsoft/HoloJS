#include "stdafx.h"
#include "../host-interfaces.h"
#include "holojs/private/script-host-utilities.h"
#include "include/holojs/windows/mixed-reality/spatial-controller.h"
#include <ppltasks.h>

using namespace Windows::UI::Input::Spatial;
using namespace HoloJs::MixedReality::Input;
using namespace HoloJs::Interfaces;

static const std::wstring ButtonPressedPropertyName = L"pressed";
static const std::wstring ButtonValuePropertyName = L"value";
static const std::wstring ButtonTouchedPropertyName = L"touched";
static const std::wstring AxesPropertyName = L"axes";
static const std::wstring HandednessPropertyName = L"hand";
static const std::wstring IndexPropertyName = L"index";

static const std::wstring HasPositionPropertyName = L"hasPosition";
static const std::wstring HasOrientationPropertyName = L"hasOrientation";

#define AXIS_COUNT 2

SpatialController::SpatialController(SpatialInteractionSource ^ source, unsigned int index)
{
    m_controller = source->Controller;
    m_source = source;

    if (m_controller != nullptr) {
        m_productId = source->Controller->ProductId;
    } else {
        m_productId = 0;
    }
    
    if (source->Handedness == SpatialInteractionSourceHandedness::Left) {
        m_handedness = L"left";
    } else if (source->Handedness == SpatialInteractionSourceHandedness::Right) {
        m_handedness = L"right";
    } else {
        m_handedness = L"";
    }

    m_index = index;
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

    for (unsigned int i = 0; i < m_scriptButtons.size(); i++) {
        if (m_scriptButtons[i] != JS_INVALID_REFERENCE) {
            JsRelease(m_scriptButtons[i], nullptr);
            m_scriptButtons[i] = JS_INVALID_REFERENCE;
        }
    }
}

HRESULT SpatialController::projectToScript()
{
    static const std::wstring spatialControllerId = L"Spatial Controller (Spatial Interaction Source)";
    static const std::wstring handsControllerId = L"Microsoft HoloLens Hands Input";
    static const std::wstring controllerIdPropName = L"id";
    static const std::wstring controllerProductIdPropName = L"productId";

    RETURN_IF_JS_ERROR(JsCreateObject(&m_scriptController));
    unsigned int refCount;
    RETURN_IF_FAILED(JsAddRef(m_scriptController, &refCount));

    RETURN_IF_FAILED(createPoseProperty());
    RETURN_IF_FAILED(createButtonsArray());

    JsValueRef axesRef;
    RETURN_IF_JS_ERROR(
        ScriptHostUtilities::SetFloat64ArrayProperty(0, &axesRef, &m_axesStoragePointer, m_scriptController, L"axes"));

    RETURN_IF_FAILED(ScriptHostUtilities::SetJsProperty(m_scriptController, controllerIdPropName, m_controller == nullptr ? handsControllerId : spatialControllerId));

    RETURN_IF_FAILED(ScriptHostUtilities::SetJsProperty(
        m_scriptController, controllerProductIdPropName, static_cast<int>(m_productId)));

    if (m_controller != nullptr) {
        JsValueRef axisArrayRef;
        RETURN_IF_JS_ERROR(ScriptHostUtilities::SetFloat64ArrayProperty(
            AXIS_COUNT * 2, &axisArrayRef, &m_axisBuffer, m_scriptController, AxesPropertyName.c_str()));
    }

    RETURN_IF_FAILED(ScriptHostUtilities::SetJsProperty(m_scriptController, IndexPropertyName, static_cast<int>(m_index)));

    RETURN_IF_JS_ERROR(ScriptHostUtilities::SetJsProperty(m_scriptController, HandednessPropertyName, m_handedness));

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

    // Controllers don't always have orientation; create an orientation array, but don't attach it to the pose object
    // yet
    RETURN_IF_FAILED(
        ScriptHostUtilities::CreateTypedArray(JsArrayTypeFloat32,
                                              &m_orientationRef,
                                              4,
                                              reinterpret_cast<unsigned char**>(&m_orientationVectorStoragePointer)));
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

    m_scriptButtons.resize(m_controller != nullptr ? 5 : 1);

    JsValueRef buttonsArray;
    RETURN_IF_JS_ERROR(JsCreateArray(static_cast<unsigned int>(m_scriptButtons.size()), &buttonsArray));

    JsValueRef falseRef;
    RETURN_IF_JS_ERROR(JsGetFalseValue(&falseRef));

    for (unsigned int i = 0; i < m_scriptButtons.size(); i++) {
        RETURN_IF_JS_ERROR(JsCreateObject(&m_scriptButtons[i]));
        RETURN_IF_JS_ERROR(ScriptHostUtilities::SetJsProperty(m_scriptButtons[i], ButtonPressedPropertyName, falseRef));
        RETURN_IF_JS_ERROR(ScriptHostUtilities::SetJsProperty(m_scriptButtons[i], ButtonTouchedPropertyName, falseRef));
        RETURN_IF_JS_ERROR(ScriptHostUtilities::SetJsProperty(m_scriptButtons[i], ButtonValuePropertyName, 0));

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

    struct ButtonData {
        bool pressed;
        bool touched;
        double value;
    };

    const auto& controller = state->ControllerProperties;

    if (controller != nullptr) {
        ButtonData buttonData[] = {
            {controller->IsThumbstickPressed,
             controller->IsThumbstickPressed,
             controller->IsThumbstickPressed ? 1.0 : 0.0 ? 1.0 : 0.0},
            {state->IsSelectPressed, state->IsSelectPressed, state->SelectPressedValue},
            {state->IsGrasped, state->IsGrasped, state->IsGrasped ? 1.0 : 0.0},
            {state->IsMenuPressed, state->IsMenuPressed, state->IsMenuPressed ? 1.0 : 0.0},
            {controller->IsTouchpadPressed, controller->IsTouchpadTouched, controller->IsTouchpadPressed ? 1.0 : 0.0}};

        int index = 0;
        for (const auto& button : buttonData) {
            JsValueRef pressedRef;
            RETURN_IF_JS_ERROR(JsBoolToBoolean(button.pressed, &pressedRef));
            RETURN_IF_JS_ERROR(
                ScriptHostUtilities::SetJsProperty(m_scriptButtons[index], ButtonPressedPropertyName, pressedRef));

            JsValueRef touchedRef;
            RETURN_IF_JS_ERROR(JsBoolToBoolean(button.touched, &touchedRef));
            RETURN_IF_JS_ERROR(
                ScriptHostUtilities::SetJsProperty(m_scriptButtons[index], ButtonTouchedPropertyName, touchedRef));

            RETURN_IF_JS_ERROR(
                ScriptHostUtilities::SetJsProperty(m_scriptButtons[index], ButtonValuePropertyName, button.value));

            index++;
        }

        m_axisBuffer[0] = controller->ThumbstickX;
        m_axisBuffer[1] = controller->ThumbstickY;

        m_axisBuffer[2] = controller->TouchpadX;
        m_axisBuffer[3] = controller->TouchpadY;
    } else {
        ButtonData buttonData[] = {
            {state->IsPressed, state->IsPressed, state->SelectPressedValue}};

        JsValueRef pressedRef;
        RETURN_IF_JS_ERROR(JsBoolToBoolean(state->IsPressed, &pressedRef));
        RETURN_IF_JS_ERROR(
            ScriptHostUtilities::SetJsProperty(m_scriptButtons[0], ButtonPressedPropertyName, pressedRef));

        RETURN_IF_JS_ERROR(
            ScriptHostUtilities::SetJsProperty(m_scriptButtons[0], ButtonTouchedPropertyName, pressedRef));

        RETURN_IF_JS_ERROR(
            ScriptHostUtilities::SetJsProperty(m_scriptButtons[0], ButtonValuePropertyName, state->IsPressed ? 1.0 : 0.0));
    }

    return S_OK;
}
