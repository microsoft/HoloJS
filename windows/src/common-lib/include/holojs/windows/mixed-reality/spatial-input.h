#pragma once

#include "holojs/private/script-window.h"
#include "spatial-controller.h"
#include <map>
#include <memory>

namespace HoloJs {
namespace MixedReality {
namespace Input {

class SpatialInput {
   public:
    SpatialInput() {}
    ~SpatialInput();

    HRESULT initialize();

    void setFrameOfReference(Windows::Perception::Spatial::SpatialStationaryFrameOfReference ^ frameOfReference)
    {
        m_stationaryFrameOfReference = frameOfReference;
    }

    void setWindow(HWND window) { m_window = window; }
    void setCoreWindow(Windows::UI::Core::CoreWindow ^ window) { m_coreWindow = window; }
    void setScriptWindow(HoloJs::IWindow* window) { m_scriptWindow = window; }

   protected:
    HWND m_window;
    Platform::Agile<Windows::UI::Core::CoreWindow> m_coreWindow;
    Windows::UI::Input::Spatial::ISpatialInteractionManager ^ m_spatialInteractionManager;
    virtual HRESULT createSpatialInteractionManager() = 0;

   private:
    HoloJs::IWindow* m_scriptWindow;

    void OnSourceDetected(Windows::UI::Input::Spatial::SpatialInteractionManager ^ sender,
                          Windows::UI::Input::Spatial::SpatialInteractionSourceEventArgs ^ args);

    void OnSourceLost(Windows::UI::Input::Spatial::SpatialInteractionManager ^ sender,
                      Windows::UI::Input::Spatial::SpatialInteractionSourceEventArgs ^ args);

    void OnSourceUpdated(Windows::UI::Input::Spatial::SpatialInteractionManager ^ sender,
                         Windows::UI::Input::Spatial::SpatialInteractionSourceEventArgs ^ args);

    Windows::Foundation::EventRegistrationToken m_sourcePressed;
    Windows::Foundation::EventRegistrationToken m_sourceReleased;
    Windows::Foundation::EventRegistrationToken m_sourceDetectedToken;
    Windows::Foundation::EventRegistrationToken m_sourceLostToken;
    Windows::Foundation::EventRegistrationToken m_sourceUpdatedToken;

    void setupEventHandlers();

    std::map<unsigned int, std::shared_ptr<SpatialController>> m_controllersMap;

    Windows::Perception::Spatial::SpatialStationaryFrameOfReference ^ m_stationaryFrameOfReference;
};

}  // namespace Input
}  // namespace MixedReality
}  // namespace HoloJs
