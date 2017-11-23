#pragma once
#include "ChakraForHoloJS.h"
#include "InputInterface.h"

namespace HologramJS {
namespace Input {
class SpatialInput {
   public:
    SpatialInput();
    ~SpatialInput();

    void SetScriptCallback(JsValueRef scriptCallback) { m_scriptCallback = scriptCallback; }

    void SetStationaryFrameOfReference(Windows::Perception::Spatial::SpatialStationaryFrameOfReference ^
                                       frameOfReference)
    {
        m_frameOfReference = frameOfReference;
    }

    // Called on the UI thread to drain all events that were queued on worker threads
    // Remove this once RS1 is no longer supported; in RS2+ spatial input events are
    // guaranteed to come on the UI thread
    void DrainQueuedSpatialInputEvents();

    bool AddEventListener(const std::wstring& type);

    bool RemoveEventListener(const std::wstring& type);

   private:
    JsValueRef m_scriptCallback = JS_INVALID_REFERENCE;

    Windows::Foundation::EventRegistrationToken m_sourcePressedToken;
    Windows::Foundation::EventRegistrationToken m_sourceReleasedToken;
    Windows::Foundation::EventRegistrationToken m_sourceLostToken;
    Windows::Foundation::EventRegistrationToken m_sourceDetectedToken;
    Windows::Foundation::EventRegistrationToken m_sourceUpdateToken;

    bool m_spatialInputAvailable = false;

    // Stores decoded information about a spatial event
    struct SpatialEventData {
        Windows::UI::Input::Spatial::SpatialInteractionSourceKind kind;
        SpatialInputEventType type;
        bool isPressed;
        Windows::Foundation::Numerics::float3 position;

        SpatialEventData(Windows::UI::Input::Spatial::SpatialInteractionSourceKind kind,
                         SpatialInputEventType type,
                         bool isPressed,
                         Windows::Foundation::Numerics::float3 position)
        {
            this->kind = kind;
            this->type = type;
            this->isPressed = isPressed;
            this->position = position;
        }
    };

    void CallbackScriptForSpatialInput(const SpatialEventData& eventData);
    void QueueEventOrFireCallback(SpatialInputEventType type,
                                  Windows::UI::Input::Spatial::SpatialInteractionSourceEventArgs ^ args);

    JsValueRef m_spatialInputEventName;
    JsValueRef m_spatialInputSourcePressedName;
    JsValueRef m_spatialInputSourceReleasedName;

    Windows::Perception::Spatial::SpatialStationaryFrameOfReference ^ m_frameOfReference;

    // Lock to synchronize access to the queued events vector
    std::mutex m_eventsQueue;

    // List of events that were queued because they came on a non-UI thread;
    // This list is drained on v-sync
    std::vector<SpatialEventData> m_queuedEvents;

    unsigned int m_inputRefCount = 0;
};
}  // namespace Input
}  // namespace HologramJS
