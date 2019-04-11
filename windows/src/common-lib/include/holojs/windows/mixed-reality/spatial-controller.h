#pragma once

#include "holojs/private/chakra.h"
#include <map>
#include <memory>

namespace HoloJs {
namespace MixedReality {
namespace Input {

class SpatialController {
   public:
    SpatialController(Windows::UI::Input::Spatial::SpatialInteractionSource ^ source);
    ~SpatialController();

    HRESULT projectToScript();

    HRESULT update(Windows::UI::Input::Spatial::SpatialInteractionSourceState ^ state,
                   Windows::UI::Input::Spatial::SpatialInteractionSourceLocation ^ location);

    JsValueRef getScriptControllerObject() { return m_scriptController; }

   private:
    Windows::UI::Input::Spatial::SpatialInteractionController ^ m_controller;
    Windows::UI::Input::Spatial::SpatialInteractionSource ^ m_source;

    JsValueRef m_scriptController = JS_INVALID_REFERENCE;
    JsValueRef m_scriptPose = JS_INVALID_REFERENCE;
    JsValueRef m_scriptButtons[2];

    HRESULT createPoseProperty();

    HRESULT createButtonsArray();

    float* m_orientationVectorStoragePointer = nullptr;
    float* m_positionVectorStoragePointer = nullptr;

    JsValueRef m_orientationRef = JS_INVALID_REFERENCE;

    float* m_angularVelocityStoragePointer = nullptr;
    float* m_linearVelocityStoragePointer = nullptr;
    double* m_axesStoragePointer = nullptr;

    bool m_hadOrientationPreviously = false;
    bool m_hadPositionPreviously = false;
};

}  // namespace Input
}  // namespace MixedReality
}  // namespace HoloJs
