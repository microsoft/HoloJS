#pragma once

#include "stdafx.h"
#include "../uwp-opengl-context.h"
#include "holojs/private/holojs-script-host-internal.h"
#include "holojs/private/holojs-view.h"
#include "holojs/private/script-host-errors.h"
#include "holojs/private/script-window.h"
#include "holojs/private/timers.h"
#include "holojs/windows/voice-input.h"
#include "holojs/windows/qr-scanner.h"
#include "holojs/windows/mixed-reality/mixed-reality-context.h"
#include "winrt-timers-implementation.h"
#include <exception>

namespace HoloJs {
namespace UWP {

// Main entry point for our app. Connects the app with the Windows shell and handles application lifecycle events.
ref class HoloJsUWPApp sealed : public Windows::ApplicationModel::Core::IFrameworkView {
   public:
    HoloJsUWPApp();
    virtual ~HoloJsUWPApp();

    // IFrameworkView Methods.
    virtual void Initialize(Windows::ApplicationModel::Core::CoreApplicationView ^ applicationView);
    virtual void SetWindow(Windows::UI::Core::CoreWindow ^ window);
    virtual void Load(Platform::String ^ entryPoint);
    virtual void Run();
    virtual void Uninitialize();
    void Close() { m_closeRequested = true; }

    internal :

        void
        initializeScript(HoloJs::IHoloJsScriptHostInternal* scriptHost);

    HRESULT executeApp(std::shared_ptr<HoloJs::AppModel::HoloJsApp> app);

    virtual void executeOnViewThread(HoloJs::ScriptContextWorkItem* workItem);
    virtual void executeInBackground(HoloJs::BackgroundWorkItem* workItem);

    bool isWindingOrderReversed() { return m_mixedRealityContext && m_mixedRealityContext->isWindingOrderReversed(); }

    HRESULT initializeRenderingResources();
    HRESULT initializeScriptResources();
    HRESULT stopApp();

	void setViewConfiguration(HoloJs::ViewConfiguration viewConfiguration) { m_viewConfiguration = viewConfiguration; }

   protected:
    // Application lifecycle event handlers.
    void OnActivated(Windows::ApplicationModel::Core::CoreApplicationView ^ applicationView,
                     Windows::ApplicationModel::Activation::IActivatedEventArgs ^ args);
    void OnSuspending(Platform::Object ^ sender, Windows::ApplicationModel::SuspendingEventArgs ^ args);
    void OnResuming(Platform::Object ^ sender, Platform::Object ^ args);

    // Window event handlers.
    void OnWindowSizeChanged(Windows::UI::Core::CoreWindow ^ sender,
                             Windows::UI::Core::WindowSizeChangedEventArgs ^ args);
    void OnVisibilityChanged(Windows::UI::Core::CoreWindow ^ sender,
                             Windows::UI::Core::VisibilityChangedEventArgs ^ args);
    void OnWindowClosed(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::CoreWindowEventArgs ^ args);

    // DisplayInformation event handlers.
    void OnDpiChanged(Windows::Graphics::Display::DisplayInformation ^ sender, Platform::Object ^ args);
    void OnOrientationChanged(Windows::Graphics::Display::DisplayInformation ^ sender, Platform::Object ^ args);
    void OnDisplayContentsInvalidated(Windows::Graphics::Display::DisplayInformation ^ sender, Platform::Object ^ args);

   private:
    std::shared_ptr<MixedReality::MixedRealityContext> m_mixedRealityContext;
    std::unique_ptr<MixedReality::Input::SpatialInput> m_spatialInput;

    std::unique_ptr<HoloJs::Platforms::Win32::VoiceInput> m_voiceInput;
    void onSpeechRecognized(std::wstring command, double confidence);

    std::unique_ptr<HoloJs::Platforms::Win32::QrScanner> m_qrScanner;
    void onQrRecognized(std::wstring text);

    HoloJs::IWindow* m_windowElement;
    HoloJs::IHoloJsScriptHostInternal* m_host;

    std::unique_ptr<HoloJs::Timers> m_timers;

    bool Render();

    Platform::Agile<Windows::UI::Core::CoreWindow> m_window;
    Windows::UI::Core::CoreDispatcher ^ m_dispatcher;

    bool m_windowClosed;
    bool m_windowVisible;
    bool m_viewReady;
    bool m_closeRequested;

    float m_width;
    float m_height;
    bool m_sizeAvailable = false;

    std::unique_ptr<OpenGL::OpenGLContext> m_openGLContext;

    int getWidth() { return static_cast<int>(m_width); }
    int getHeight() { return static_cast<int>(m_height); }

    std::shared_ptr<HoloJs::AppModel::HoloJsApp> m_queuedApp;
    std::shared_ptr<HoloJs::AppModel::HoloJsApp> m_activeApp;
    HRESULT executeLoadedScripts();

    void registerEventHandlers();
    void OnKeyUp(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::KeyEventArgs ^ args);
    void OnKeyDown(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::KeyEventArgs ^ args);
    void OnPointerWheelChanged(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::PointerEventArgs ^ args);
    void OnPointerPressed(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::PointerEventArgs ^ args);
    void OnPointerReleased(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::PointerEventArgs ^ args);
    void OnPointerMoved(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::PointerEventArgs ^ args);

	HoloJs::ViewConfiguration m_viewConfiguration;
};

ref class HoloJsViewSource sealed : Windows::ApplicationModel::Core::IFrameworkViewSource {
   public:
    virtual Windows::ApplicationModel::Core::IFrameworkView ^ CreateView() { return m_app; }

        void SetHoloJsApp(HoloJsUWPApp ^ app)
    {
        m_app = app;
    }

   private:
    HoloJsUWPApp ^ m_app;
};

class WinRTHoloJsView : public IHoloJsView {
   public:
    WinRTHoloJsView(HoloJs::ViewConfiguration viewConfiguration)
    {
        m_viewSource = ref new HoloJsViewSource();
        m_app = ref new HoloJsUWPApp();
		m_app->setViewConfiguration(viewConfiguration);
        m_viewSource->SetHoloJsApp(m_app);
		m_viewConfiguration = viewConfiguration;
    }

    virtual ~WinRTHoloJsView();

    virtual HRESULT initialize(HoloJs::IHoloJsScriptHostInternal* host);
    virtual void setViewWindow(void* window) { throw new std::exception("Not implemented"); }

    virtual void run();

    virtual void stop();

    virtual HRESULT executeApp(std::shared_ptr<HoloJs::AppModel::HoloJsApp> app) { return m_app->executeApp(app); }

    virtual HRESULT executeScript(const wchar_t* script) { return E_FAIL; }

    virtual void executeOnViewThread(HoloJs::ScriptContextWorkItem* workItem) { m_app->executeOnViewThread(workItem); }
    virtual void executeInBackground(HoloJs::BackgroundWorkItem* workItem) { m_app->executeInBackground(workItem); }

    virtual void onError(HoloJs::ScriptHostErrorType errorType);

    virtual bool isWindingOrderReversed() { return m_app->isWindingOrderReversed(); }

    virtual void setIcon(void* platformIcon)
    { /* not available in UWP */
    }
    virtual void setTitle(const std::wstring& title)
    { /* not available in UWP view */
    }

   private:
    HoloJsViewSource ^ m_viewSource;

    HoloJsUWPApp ^ m_app;

	HoloJs::ViewConfiguration m_viewConfiguration;
};
}  // namespace UWP
}  // namespace HoloJs
