#pragma once

#include "HologramJS.h"

namespace HologramJS {

    using float4x4 = Windows::Foundation::Numerics::float4x4;

    public delegate void HoloJsCallback();

    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class HoloJsAppView sealed : public Windows::ApplicationModel::Core::IFrameworkView
    {
    public:
        HoloJsAppView(Platform::String^ appUri);

        // IFrameworkView Methods.
        virtual void Initialize(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView);
        virtual void SetWindow(Windows::UI::Core::CoreWindow^ window);
        virtual void Load(Platform::String^ entryPoint);
        virtual void Run();
        virtual void Uninitialize();

        // Enable auto stereo rendering
        property bool AutoStereoEnabled;

        // Enable depth based image stabilization
        property bool ImageStabilizationEnabled;

        // Callback to allow developers to run their own extensions.
        property HoloJsCallback^ OnBeforeRun;

        // Handle web-ar protocol activations; will execute scripts navigated to on the web-ar protocol
        property bool EnableWebArProtocolHandler;

        // The location of the world coordinate system to use; the default location is 2 meters infront of the camera
        property Windows::Foundation::Numerics::float3 WorldOriginRelativePosition;

    private:
        void RecreateRenderer();

        // Application lifecycle event handlers.
        void OnActivated(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView, Windows::ApplicationModel::Activation::IActivatedEventArgs^ args);

        // Window event handlers.
        void OnVisibilityChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ args);
        void OnWindowClosed(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::CoreWindowEventArgs^ args);

        void InitializeEGL(Windows::Graphics::Holographic::HolographicSpace^ holographicSpace);
        void InitializeEGL(Windows::UI::Core::CoreWindow^ window);
        void InitializeEGLInner(Platform::Object^ windowBasis);
        void CleanupEGL();

        bool m_WindowClosed;
        bool m_WindowVisible;

        EGLDisplay m_EglDisplay;
        EGLContext m_EglContext;
        EGLSurface m_EglSurface;

        // The holographic space the app will use for rendering.
        Windows::Graphics::Holographic::HolographicSpace^ mHolographicSpace = nullptr;

        
        Windows::Perception::Spatial::SpatialStationaryFrameOfReference^ mStationaryReferenceFrame = nullptr;

        EGLint m_PanelWidth = 0;
        EGLint m_PanelHeight = 0;

        Platform::String^ m_appUri;

        HologramScriptHost^ m_holoScriptHost;
        void LoadAndExecuteScript();
    };

    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class HoloJsAppSource sealed : Windows::ApplicationModel::Core::IFrameworkViewSource
    {
    public:
        HoloJsAppSource(Platform::String^ appUri) :
            m_appUri(appUri),
            m_appView(nullptr) {}

        // The default overload is meaningless - only useful for JavaScript but the whole class is WebHostHidden.
        // It makes the compiler happy though
        [Windows::Foundation::Metadata::DefaultOverloadAttribute]
        HoloJsAppSource(Windows::ApplicationModel::Core::IFrameworkView^ appView) :
            m_appView(appView),
            m_appUri(nullptr) {}

        virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView() {
            return (m_appView ? m_appView : ref new HologramJS::HoloJsAppView(m_appUri));
        }
    private:
        Platform::String^ m_appUri;
        Windows::ApplicationModel::Core::IFrameworkView^ m_appView;
    };
}
