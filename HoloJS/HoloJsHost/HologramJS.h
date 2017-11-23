#pragma once

#include "ChakraForHoloJS.h"
#include "ObjectEvents.h"
#include "System.h"
#include "Timers.h"
#include "WindowElement.h"
#include <ppltasks.h>

namespace HologramJS {

using float4x4 = Windows::Foundation::Numerics::float4x4;

[Windows::Foundation::Metadata::WebHostHidden] public ref class HologramScriptHost sealed {
   public:
    HologramScriptHost();

    virtual ~HologramScriptHost();

    bool Initialize();
    void Shutdown();

    bool EnableHolographicExperimental(Windows::Perception::Spatial::SpatialStationaryFrameOfReference ^
                                           frameOfReference,
                                       bool autoStereoEnabled);

    Windows::Foundation::IAsyncOperation<bool> ^ RunApp(Platform::String ^ jsonUri);

    void ResizeWindow(int width, int height) { m_window.Resize(width, height); }
    void VSync(float4x4 midViewMatrix,
               float4x4 midProjectionMatrix,
               float4x4 leftViewMatrix,
               float4x4 leftProjectionMatrix,
               float4x4 rightViewMatrix,
               float4x4 rightProjectionMatrix)
    {
        m_window.VSync(midViewMatrix,
                       midProjectionMatrix,
                       leftViewMatrix,
                       leftProjectionMatrix,
                       rightViewMatrix,
                       rightProjectionMatrix);
    }

   private:
    JsRuntimeHandle m_jsRuntime = nullptr;
    JsContextRef m_jsContext = nullptr;

    concurrency::task<bool> RunLocalScriptApp(std::wstring jsonFilePath);
    concurrency::task<bool> RunWebScriptApp(std::wstring jsonFilePath);

    API::WindowElement m_window;
    API::System m_system;
    API::Timers m_timers;
    Utilities::EventsManager m_scriptEventsManager;
};
}  // namespace HologramJS
