#pragma once

#include "ChakraForHoloJS.h"
#include "ObjectEvents.h"
#include "System.h"
#include "Timers.h"
#include "WebGLProjections.h"
#include "WindowElement.h"
#include <ppltasks.h>

namespace HologramJS {

using float4x4 = Windows::Foundation::Numerics::float4x4;

public
enum class StereoEffectMode { Auto, WebVR };

[Windows::Foundation::Metadata::WebHostHidden] public ref class HologramScriptHost sealed {
   public:
    HologramScriptHost();

    virtual ~HologramScriptHost();

    bool InitializeSystem();

    bool InitializeRendering(Windows::Perception::Spatial::SpatialStationaryFrameOfReference ^ frameOfReference,
                             StereoEffectMode stereoMode,
                             int width,
                             int height);
    void Shutdown();

    Windows::Foundation::IAsyncOperation<bool> ^ RunApp(Platform::String ^ jsonUri);

    void ResizeWindow(int width, int height) { m_window->Resize(width, height); }
    void VSync(float4x4 midViewMatrix,
               float4x4 midProjectionMatrix,
               float4x4 leftViewMatrix,
               float4x4 leftProjectionMatrix,
               float4x4 rightViewMatrix,
               float4x4 rightProjectionMatrix)
    {
        if (!m_webGlContextInitialized) {
            if (!TryInitializeWebGlContext()) {
                return;
            }
        }

        m_window->VSync(midViewMatrix,
                        midProjectionMatrix,
                        leftViewMatrix,
                        leftProjectionMatrix,
                        rightViewMatrix,
                        rightProjectionMatrix);

        m_webglProjections->ScriptRenderComplete();
    }

   private:
    JsRuntimeHandle m_jsRuntime = nullptr;
    JsContextRef m_jsContext = nullptr;

    bool TryInitializeWebGlContext();
    bool m_webGlContextInitialized = false;

    bool EnableHolographicExperimental(Windows::Perception::Spatial::SpatialStationaryFrameOfReference ^
                                           frameOfReference,
                                       WebGL::RenderMode renderMode);

    concurrency::task<bool> RunLocalScriptApp(std::wstring jsonFilePath);
    concurrency::task<bool> RunWebScriptApp(std::wstring jsonFilePath);

    std::shared_ptr<API::WindowElement> m_window;
    std::shared_ptr<HologramJS::WebGL::WebGLProjections> m_webglProjections;
    API::System m_system;
    API::Timers m_timers;
    Utilities::EventsManager m_scriptEventsManager;
};
}  // namespace HologramJS
