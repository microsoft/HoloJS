#pragma once

#include "stdafx.h"
#include "../uwp-opengl-context.h"
#include "holojs/private/holojs-script-host-internal.h"
#include "holojs/private/script-host-errors.h"
#include "holojs/private/script-window.h"
#include "holojs/private/timers.h"
#include "holojs/windows/mixed-reality/mixed-reality-context.h"
#include "holojs/windows/speech-recognizer.h"
#include "winrt-timers-implementation.h"
#include "../corewindow-input.h"
#include <exception>
#include <memory>
#include <queue>

namespace HoloJs {
namespace UWP {

class UWPOffscreenView : public IHoloJsView {
   public:
    UWPOffscreenView(HoloJs::ViewConfiguration viewConfiguration) { m_viewConfiguration = viewConfiguration; }

    virtual ~UWPOffscreenView();

    virtual HRESULT initialize(HoloJs::IHoloJsScriptHostInternal* host);

    virtual void setViewWindow(void* window) { throw new std::exception("Not implemented"); }

    virtual void run();

    virtual void stop();

    virtual HRESULT executeApp(std::shared_ptr<HoloJs::AppModel::HoloJsApp> app);

    virtual HRESULT executeScript(const wchar_t* script) { return E_FAIL; }

    virtual long executeOnViewThread(HoloJs::IForegroundWorkItem* workItem);

    virtual long executeInBackground(HoloJs::IBackgroundWorkItem* workItem);

    virtual void onError(HoloJs::ScriptHostErrorType errorType);

    virtual bool isWindingOrderReversed()
    {
        return m_mixedRealityContext && m_mixedRealityContext->isWindingOrderReversed();
    }

    virtual void setIcon(void* platformIcon)
    { /* not available in offscreen view */
    }
    virtual void setTitle(const std::wstring& title)
    { /* not available in offscreen view */
    }

    virtual long getStationaryCoordinateSystem(void** coordinateSystem)
    {
        if (m_mixedRealityContext) {
            *coordinateSystem =
                reinterpret_cast<void*>(m_mixedRealityContext->getStationaryFrameOfReference()->CoordinateSystem);
            return S_OK;
        } else {
            return E_FAIL;
        }
    }

    virtual long draw();

   private:
    std::shared_ptr<MixedReality::MixedRealityContext> m_mixedRealityContext;
    std::unique_ptr<MixedReality::Input::SpatialInput> m_spatialInput;

    std::unique_ptr<HoloJs::Platforms::Win32::SpeechRecognizer> m_voiceInput;
    void onSpeechRecognized(std::wstring command, double confidence);

	HoloJs::UWP::CoreWindowInput ^ m_coreWindowInputHandler;

    HoloJs::ViewConfiguration m_viewConfiguration;

    std::unique_ptr<OpenGL::OpenGLContext> m_openGLContext;

    HoloJs::IWindow* m_windowElement;
    HoloJs::IHoloJsScriptHostInternal* m_host;

    std::unique_ptr<HoloJs::Timers> m_timers;

    HRESULT initializeScriptResources();

    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_renderSurface;

    std::shared_ptr<HoloJs::AppModel::HoloJsApp> m_app;
    bool m_appLoaded = false;

	Windows::UI::Core::CoreDispatcher ^ m_dispatcher;
};
}  // namespace UWP
}  // namespace HoloJs
