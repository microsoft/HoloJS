#include "pch.h"
#include "SpatialInput.h"
#include "ScriptErrorHandling.h"

PCWSTR g_supportedSpatialInputEvents[] = {
    L"sourcepress", L"sourcerelease", L"sourcelost", L"sourcedetected", L"sourceupdate"};

using namespace HologramJS::Input;
using namespace Windows::UI::Input::Spatial;
using namespace Windows::Foundation;
using namespace std;
using namespace Windows::Perception::Spatial;

SpatialInput::SpatialInput() {}

SpatialInput::~SpatialInput()
{
    if (m_inputRefCount > 0) {
        SpatialInteractionManager::GetForCurrentView()->SourceReleased -= m_sourceReleasedToken;
        SpatialInteractionManager::GetForCurrentView()->SourcePressed -= m_sourcePressedToken;
        SpatialInteractionManager::GetForCurrentView()->SourceDetected -= m_sourceDetectedToken;
        SpatialInteractionManager::GetForCurrentView()->SourceLost -= m_sourceLostToken;
        SpatialInteractionManager::GetForCurrentView()->SourceUpdated -= m_sourceUpdateToken;

        m_inputRefCount = 0;
    }
}

bool SpatialInput::AddEventListener(const wstring& type)
{
    for (int i = 0; i < ARRAYSIZE(g_supportedSpatialInputEvents); i++) {
        if (type == g_supportedSpatialInputEvents[i]) {
            m_inputRefCount++;

            if (m_inputRefCount == 1) {
                if (!SpatialInteractionManager::GetForCurrentView()) {
                    return true;
                }

                m_sourcePressedToken = SpatialInteractionManager::GetForCurrentView()->SourcePressed +=
                    ref new TypedEventHandler<SpatialInteractionManager ^, SpatialInteractionSourceEventArgs ^>(
                        [this](SpatialInteractionManager ^ sender, SpatialInteractionSourceEventArgs ^ args) {
                            QueueEventOrFireCallback(SpatialInputEventType::Pressed, args);
                        });

                m_sourceReleasedToken = SpatialInteractionManager::GetForCurrentView()->SourceReleased +=
                    ref new TypedEventHandler<SpatialInteractionManager ^, SpatialInteractionSourceEventArgs ^>(
                        [this](SpatialInteractionManager ^ sender, SpatialInteractionSourceEventArgs ^ args) {
                            QueueEventOrFireCallback(SpatialInputEventType::Released, args);
                        });

                m_sourceDetectedToken = SpatialInteractionManager::GetForCurrentView()->SourceDetected +=
                    ref new TypedEventHandler<SpatialInteractionManager ^, SpatialInteractionSourceEventArgs ^>(
                        [this](SpatialInteractionManager ^ sender, SpatialInteractionSourceEventArgs ^ args) {
                            QueueEventOrFireCallback(SpatialInputEventType::Detected, args);
                        });

                m_sourceLostToken = SpatialInteractionManager::GetForCurrentView()->SourceLost +=
                    ref new TypedEventHandler<SpatialInteractionManager ^, SpatialInteractionSourceEventArgs ^>(
                        [this](SpatialInteractionManager ^ sender, SpatialInteractionSourceEventArgs ^ args) {
                            QueueEventOrFireCallback(SpatialInputEventType::Lost, args);
                        });

                m_sourceUpdateToken = SpatialInteractionManager::GetForCurrentView()->SourceUpdated +=
                    ref new TypedEventHandler<SpatialInteractionManager ^, SpatialInteractionSourceEventArgs ^>(
                        [this](SpatialInteractionManager ^ sender, SpatialInteractionSourceEventArgs ^ args) {
                            QueueEventOrFireCallback(SpatialInputEventType::Update, args);
                        });

                m_spatialInputAvailable = true;
            }

            return true;
        }
    }

    return false;
}

bool SpatialInput::RemoveEventListener(const wstring& type)
{
    for (int i = 0; i < ARRAYSIZE(g_supportedSpatialInputEvents); i++) {
        if (type == g_supportedSpatialInputEvents[i]) {
            m_inputRefCount--;

            if (m_inputRefCount == 0 && m_spatialInputAvailable) {
                SpatialInteractionManager::GetForCurrentView()->SourceReleased -= m_sourceReleasedToken;
                SpatialInteractionManager::GetForCurrentView()->SourcePressed -= m_sourcePressedToken;
                SpatialInteractionManager::GetForCurrentView()->SourceDetected -= m_sourceDetectedToken;
                SpatialInteractionManager::GetForCurrentView()->SourceLost -= m_sourceLostToken;
                SpatialInteractionManager::GetForCurrentView()->SourceUpdated -= m_sourceUpdateToken;
            }

            return true;
        }
    }

    return false;
}

void SpatialInput::DrainQueuedSpatialInputEvents()
{
    // Quick check if there's something in the queue
    // Technically there could be a race condition in checking the size outside of the lock
    // but worst case scenario event1 will be delivered on the next vsync
    if (m_queuedEvents.size() == 0) {
        return;
    }

    std::lock_guard<std::mutex> guard(m_eventsQueue);

    for (const auto& eventData : m_queuedEvents) {
        CallbackScriptForSpatialInput(eventData);
    }

    m_queuedEvents.clear();
}

void SpatialInput::CallbackScriptForSpatialInput(const SpatialEventData& eventData)
{
    EXIT_IF_TRUE(m_scriptCallback == JS_INVALID_REFERENCE);
    EXIT_IF_FALSE(m_spatialInputAvailable);

    JsValueRef parameters[8];
    parameters[0] = m_scriptCallback;
    JsValueRef* eventTypeParam = &parameters[1];
    JsValueRef* xParam = &parameters[2];
    JsValueRef* yParam = &parameters[3];
    JsValueRef* zParam = &parameters[4];
    JsValueRef* isPressedParam = &parameters[5];
    JsValueRef* sourceKindParam = &parameters[6];
    JsValueRef* spatialInputTypeParam = &parameters[7];

    EXIT_IF_JS_ERROR(JsIntToNumber(static_cast<int>(NativeToScriptInputType::SpatialInput), eventTypeParam));
    EXIT_IF_JS_ERROR(JsDoubleToNumber(eventData.position.x, xParam));
    EXIT_IF_JS_ERROR(JsDoubleToNumber(eventData.position.y, yParam));
    EXIT_IF_JS_ERROR(JsDoubleToNumber(eventData.position.z, zParam));
    EXIT_IF_JS_ERROR(JsBoolToBoolean(eventData.isPressed, isPressedParam));
    EXIT_IF_JS_ERROR(JsIntToNumber(static_cast<int>(eventData.kind), sourceKindParam));
    EXIT_IF_JS_ERROR(JsIntToNumber(static_cast<int>(eventData.type), spatialInputTypeParam));

    JsValueRef result;
    HANDLE_EXCEPTION_IF_JS_ERROR(JsCallFunction(m_scriptCallback, parameters, ARRAYSIZE(parameters), &result));
}

void SpatialInput::QueueEventOrFireCallback(SpatialInputEventType type,
                                            Windows::UI::Input::Spatial::SpatialInteractionSourceEventArgs ^ args)
{
    EXIT_IF_TRUE(m_scriptCallback == JS_INVALID_REFERENCE);
    EXIT_IF_FALSE(m_spatialInputAvailable);

    JsValueRef parameters[8];
    parameters[0] = m_scriptCallback;
    JsValueRef* eventTypeParam = &parameters[1];
    JsValueRef* xParam = &parameters[2];
    JsValueRef* yParam = &parameters[3];
    JsValueRef* zParam = &parameters[4];
    JsValueRef* isPressedParam = &parameters[5];
    JsValueRef* sourceKindParam = &parameters[6];
    JsValueRef* spatialInputTypeParam = &parameters[7];

    const auto location = args->State->Properties->TryGetLocation(m_frameOfReference->CoordinateSystem);
    const float x = location ? location->Position->Value.x : 0;
    const float y = location ? location->Position->Value.y : 0;
    const float z = location ? location->Position->Value.z : 0;

    JsContextRef currentContext = nullptr;
    EXIT_IF_JS_ERROR(JsGetCurrentContext(&currentContext));

    if (currentContext != nullptr) {
        // We are on the UI thread; prepare the parameters and fire the callback to the script guest
        EXIT_IF_JS_ERROR(JsIntToNumber(static_cast<int>(NativeToScriptInputType::SpatialInput), eventTypeParam));
        EXIT_IF_JS_ERROR(JsDoubleToNumber(x, xParam));
        EXIT_IF_JS_ERROR(JsDoubleToNumber(y, yParam));
        EXIT_IF_JS_ERROR(JsDoubleToNumber(z, zParam));
        EXIT_IF_JS_ERROR(JsBoolToBoolean(args->State->IsPressed, isPressedParam));
        EXIT_IF_JS_ERROR(JsIntToNumber(static_cast<int>(args->State->Source->Kind), sourceKindParam));
        EXIT_IF_JS_ERROR(JsIntToNumber(static_cast<int>(type), spatialInputTypeParam));

        JsValueRef result;
        HANDLE_EXCEPTION_IF_JS_ERROR(JsCallFunction(m_scriptCallback, parameters, ARRAYSIZE(parameters), &result));
    } else {
        // We are not on the JS thread; queue the event and it will be picked up later
        // by the message pump on the UI/JS thread.
        std::lock_guard<std::mutex> guard(m_eventsQueue);
        m_queuedEvents.emplace_back(
            SpatialEventData(args->State->Source->Kind,
                             type,
                             args->State->IsPressed,
                             (location ? location->Position->Value : Windows::Foundation::Numerics::float3())));
    }
}
