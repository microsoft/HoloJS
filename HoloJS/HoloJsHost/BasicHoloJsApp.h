#pragma once

#include "HologramJS.h"

namespace HologramJS {
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class BasicHoloJsApp sealed : public Windows::ApplicationModel::Core::IFrameworkView
    {
    public:
        BasicHoloJsApp(Platform::String^ appUri);

        // IFrameworkView Methods.
        virtual void Initialize(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView);
        virtual void SetWindow(Windows::UI::Core::CoreWindow^ window);
        virtual void Load(Platform::String^ entryPoint);
        virtual void Run();
        virtual void Uninitialize();

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

        // The world coordinate system. In this example, a reference frame placed in the environment.
        Windows::Perception::Spatial::SpatialStationaryFrameOfReference^ mStationaryReferenceFrame = nullptr;

        EGLint m_PanelWidth = 0;
        EGLint m_PanelHeight = 0;

        Platform::String^ m_appUri;

        HologramScriptHost^ m_holoScriptHost;
        void LoadAndExecuteScript();
    };
}
