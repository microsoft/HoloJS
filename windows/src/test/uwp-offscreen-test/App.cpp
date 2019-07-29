#include "pch.h"
#include "App.h"
#include <windows.graphics.directx.direct3d11.interop.h>

#include <ppltasks.h>

using namespace uwp_offscreen_test;

using namespace concurrency;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Microsoft::WRL;
using namespace Windows::Graphics::DirectX::Direct3D11;

// The main function is only used to initialize our IFrameworkView class.
[Platform::MTAThread] int main(Platform::Array<Platform::String ^> ^) {
    auto direct3DApplicationSource = ref new Direct3DApplicationSource();
    CoreApplication::Run(direct3DApplicationSource);
    return 0;
}

    IFrameworkView
    ^ Direct3DApplicationSource::CreateView()
{
    return ref new App();
}

App::App() : m_windowClosed(false), m_windowVisible(true) {}

// The first method called when the IFrameworkView is being created.
void App::Initialize(CoreApplicationView ^ applicationView)
{
    // Register event handlers for app lifecycle. This example includes Activated, so that we
    // can make the CoreWindow active and start rendering on the window.
    applicationView->Activated +=
        ref new TypedEventHandler<CoreApplicationView ^, IActivatedEventArgs ^>(this, &App::OnActivated);

    CoreApplication::Suspending += ref new EventHandler<SuspendingEventArgs ^>(this, &App::OnSuspending);

    CoreApplication::Resuming += ref new EventHandler<Platform::Object ^>(this, &App::OnResuming);

    // At this point we have access to the device.
    // We can create the device-dependent resources.
    m_deviceResources = std::make_shared<DX::DeviceResources>();
}

// Called when the CoreWindow object is created (or re-created).
void App::SetWindow(CoreWindow ^ window)
{
    if (Windows::Graphics::Holographic::HolographicSpace::IsAvailable) {
        m_holographicSpace = Windows::Graphics::Holographic::HolographicSpace::CreateForCoreWindow(window);
    }
    window->SizeChanged +=
        ref new TypedEventHandler<CoreWindow ^, WindowSizeChangedEventArgs ^>(this, &App::OnWindowSizeChanged);

    window->VisibilityChanged +=
        ref new TypedEventHandler<CoreWindow ^, VisibilityChangedEventArgs ^>(this, &App::OnVisibilityChanged);

    window->Closed += ref new TypedEventHandler<CoreWindow ^, CoreWindowEventArgs ^>(this, &App::OnWindowClosed);

    DisplayInformation ^ currentDisplayInformation = DisplayInformation::GetForCurrentView();

    currentDisplayInformation->DpiChanged +=
        ref new TypedEventHandler<DisplayInformation ^, Object ^>(this, &App::OnDpiChanged);

    currentDisplayInformation->OrientationChanged +=
        ref new TypedEventHandler<DisplayInformation ^, Object ^>(this, &App::OnOrientationChanged);

    DisplayInformation::DisplayContentsInvalidated +=
        ref new TypedEventHandler<DisplayInformation ^, Object ^>(this, &App::OnDisplayContentsInvalidated);

    m_deviceResources->SetWindow(window);
}

// Initializes scene resources, or loads a previously saved app state.
void App::Load(Platform::String ^ entryPoint)
{
    if (m_main == nullptr) {
        m_main = std::unique_ptr<uwp_offscreen_testMain>(new uwp_offscreen_testMain(m_deviceResources));
    }
}

// This method is called after the window becomes active.
void App::Run()
{
    // Create DX shared texture
    D3D11_TEXTURE2D_DESC texDesc = {0};
    texDesc.Width = 200;
    texDesc.Height = 200;
    texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> sharedTexture;
    m_deviceResources->GetD3DDevice()->CreateTexture2D(&texDesc, nullptr, sharedTexture.ReleaseAndGetAddressOf());

    Microsoft::WRL::ComPtr<IDXGISurface> sharedSurface;
    sharedTexture.As(&sharedSurface);

    Windows::Graphics::DirectX::Direct3D11::IDirect3DSurface ^ scriptRenderSurface =
        CreateDirect3DSurface(sharedSurface.Get());

    auto scriptHost = ref new HoloJs::UWP::HoloJsScriptHost();
    auto viewConfiguration = ref new HoloJs::UWP::ViewConfiguration();

    scriptHost->disableLoadingAnimation();
    // scriptHost->enableDebugger();

    viewConfiguration->setViewMode(HoloJs::UWP::ViewMode::Offscreen);
    viewConfiguration->setOffscreenRenderSurface(scriptRenderSurface);
    if (scriptHost->initialize(viewConfiguration)) {
        auto uri = ref new Platform::String(L"https://microsoft.github.io/HoloJS/samples/molecule-viewer.json");
        scriptHost->startUri(uri);
    }

    while (!m_windowClosed) {
        if (m_windowVisible) {
            CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

            m_main->Update();

            if (m_main->Render()) {
                scriptHost->render(nullptr);
                m_deviceResources->Present();
            }
        } else {
            CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(
                CoreProcessEventsOption::ProcessOneAndAllPending);
        }
    }
}

// Required for IFrameworkView.
// Terminate events do not cause Uninitialize to be called. It will be called if your IFrameworkView
// class is torn down while the app is in the foreground.
void App::Uninitialize() {}

// Application lifecycle event handlers.

void App::OnActivated(CoreApplicationView ^ applicationView, IActivatedEventArgs ^ args)
{
    // Run() won't start until the CoreWindow is activated.
    CoreWindow::GetForCurrentThread()->Activate();
}

void App::OnSuspending(Platform::Object ^ sender, SuspendingEventArgs ^ args)
{
    // Save app state asynchronously after requesting a deferral. Holding a deferral
    // indicates that the application is busy performing suspending operations. Be
    // aware that a deferral may not be held indefinitely. After about five seconds,
    // the app will be forced to exit.
    SuspendingDeferral ^ deferral = args->SuspendingOperation->GetDeferral();

    create_task([this, deferral]() {
        m_deviceResources->Trim();

        // Insert your code here.

        deferral->Complete();
    });
}

void App::OnResuming(Platform::Object ^ sender, Platform::Object ^ args)
{
    // Restore any data or state that was unloaded on suspend. By default, data
    // and state are persisted when resuming from suspend. Note that this event
    // does not occur if the app was previously terminated.

    // Insert your code here.
}

// Window event handlers.

void App::OnWindowSizeChanged(CoreWindow ^ sender, WindowSizeChangedEventArgs ^ args)
{
    m_deviceResources->SetLogicalSize(Size(sender->Bounds.Width, sender->Bounds.Height));
    m_main->CreateWindowSizeDependentResources();
}

void App::OnVisibilityChanged(CoreWindow ^ sender, VisibilityChangedEventArgs ^ args)
{
    m_windowVisible = args->Visible;
}

void App::OnWindowClosed(CoreWindow ^ sender, CoreWindowEventArgs ^ args) { m_windowClosed = true; }

// DisplayInformation event handlers.

void App::OnDpiChanged(DisplayInformation ^ sender, Object ^ args)
{
    // Note: The value for LogicalDpi retrieved here may not match the effective DPI of the app
    // if it is being scaled for high resolution devices. Once the DPI is set on DeviceResources,
    // you should always retrieve it using the GetDpi method.
    // See DeviceResources.cpp for more details.
    m_deviceResources->SetDpi(sender->LogicalDpi);
    m_main->CreateWindowSizeDependentResources();
}

void App::OnOrientationChanged(DisplayInformation ^ sender, Object ^ args)
{
    m_deviceResources->SetCurrentOrientation(sender->CurrentOrientation);
    m_main->CreateWindowSizeDependentResources();
}

void App::OnDisplayContentsInvalidated(DisplayInformation ^ sender, Object ^ args)
{
    m_deviceResources->ValidateDevice();
}