#include "stdafx.h"
#include "../file-handle-access.h"
#include "../uwp-mixed-reality-context.h"
#include "../uwp-spatial-input.h"
#include "holojs/holojs-script-host.h"
#include "winrt-view.h"
#include <agents.h>
#include <concrt.h>
#include <ppl.h>
#include <ppltasks.h>

using namespace std;
using namespace HoloJs::UWP;

using namespace concurrency;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace HoloJs::AppModel;

long long QueuedAppStartWorkItem::QueuedAppStartWorkItemId = 1000;

enum class VoiceCommandIntent { LoadOtherApp };

map<wstring, VoiceCommandIntent> g_voiceCommands = {{L"scan", VoiceCommandIntent::LoadOtherApp},
                                                    {L"load another app", VoiceCommandIntent::LoadOtherApp},
                                                    {L"scan qr code", VoiceCommandIntent::LoadOtherApp},
                                                    {L"load", VoiceCommandIntent::LoadOtherApp}};

HoloJsUWPApp::~HoloJsUWPApp() {}

HoloJsUWPApp::HoloJsUWPApp() : m_windowClosed(false), m_windowVisible(true) {}

// The first method called when the IFrameworkView is being created.
void HoloJsUWPApp::Initialize(CoreApplicationView ^ applicationView)
{
    // Register event handlers for app lifecycle. This example includes Activated, so that we
    // can make the CoreWindow active and start rendering on the window.
    applicationView->Activated +=
        ref new TypedEventHandler<CoreApplicationView ^, IActivatedEventArgs ^>(this, &HoloJsUWPApp::OnActivated);

    CoreApplication::Suspending += ref new EventHandler<SuspendingEventArgs ^>(this, &HoloJsUWPApp::OnSuspending);

    CoreApplication::Resuming += ref new EventHandler<Platform::Object ^>(this, &HoloJsUWPApp::OnResuming);
}

void HoloJsUWPApp::initializeScript(HoloJs::IHoloJsScriptHostInternal* scriptHost) { m_host = scriptHost; }

HRESULT HoloJsUWPApp::executeApp(std::shared_ptr<HoloJsApp> app)
{
    return queueForegroundWorkItem(new QueuedAppStartWorkItem(app));
}

HRESULT HoloJsUWPApp::executeAppInternal(std::shared_ptr<HoloJsApp> app)
{
    if (m_activeApp) {
        stopApp();
    }

    RETURN_IF_FAILED(m_host->createExecutionContext());
    initializeScriptResources();

    RETURN_IF_FAILED(m_host->executeLoadedApp(app));

    m_activeApp = app;

    return S_OK;
}

long HoloJsUWPApp::executeOnViewThread(HoloJs::IForegroundWorkItem* workItem)
{
    return queueForegroundWorkItem(workItem);
}

long HoloJsUWPApp::executeInBackground(HoloJs::IBackgroundWorkItem* workItem)
{
    RETURN_IF_TRUE(m_closeRequested || m_windowClosed);

    create_task([workItem]() -> long {
        auto result = workItem->execute();
        delete workItem;
        return result;
    });

    return S_OK;
}

long HoloJsUWPApp::queueForegroundWorkItem(HoloJs::IForegroundWorkItem* workItem)
{
    std::lock_guard<std::recursive_mutex> guard(m_foregroundWorkItemQueue);
    shared_ptr<HoloJs::IForegroundWorkItem> trackedWorkItem(workItem);
    RETURN_IF_TRUE(m_closeRequested || m_windowClosed);

    m_foregroundWorkItems.push(trackedWorkItem);

    return S_OK;
}

void HoloJsUWPApp::executeOneForegroundWorkItem()
{
    if (m_foregroundWorkItems.size() > 0) {
        std::lock_guard<std::recursive_mutex> guard(m_foregroundWorkItemQueue);
        auto workItem = m_foregroundWorkItems.front();
        m_foregroundWorkItems.pop();
        if (workItem->getTag() == QueuedAppStartWorkItem::QueuedAppStartWorkItemId) {
            auto app = dynamic_cast<QueuedAppStartWorkItem*>(workItem.get())->getApp();
            executeAppInternal(app);
        } else {
            workItem->execute();
        }
    }
}

HRESULT HoloJsUWPApp::stopApp()
{
    m_windowElement = nullptr;
    m_timers.reset();
    m_spatialInput.reset();

    if (m_mixedRealityContext) {
        m_mixedRealityContext->releaseScriptResources();
    }

    m_activeApp.reset();

    m_openGLContext.reset();

    return S_OK;
}

HRESULT HoloJsUWPApp::initializeRenderingResources()
{
    if (m_mixedRealityContext) {
        m_width = m_mixedRealityContext->getWidth();
        m_height = m_mixedRealityContext->getHeight();
    }

    if (m_viewConfiguration.enableQrCodeNavigation) {
        m_voiceInput = make_unique<HoloJs::Platforms::Win32::SpeechRecognizer>(m_host);
        m_voiceInput->setOnResultsCallback([this](const wstring& command, double confidence) -> bool {
            onSpeechRecognized(command, confidence);
            return true;
        });

        vector<wstring> commands;
        for (const auto& command : g_voiceCommands) {
            if (command.second == VoiceCommandIntent::LoadOtherApp) {
                commands.push_back(command.first);
            }
        }

        if (commands.size() > 0) {
            m_voiceInput->setKeywords(commands);
            m_voiceInput->start();
        }

        m_qrScanner = make_unique<HoloJs::Platforms::Win32::QrScanner>(m_host);
        m_qrScanner->setOnResultsCallback([this](bool success, wstring result) { onQrRecognized(success, result); });
    }

    return S_OK;
}

HRESULT HoloJsUWPApp::initializeScriptResources()
{
    m_windowElement = m_host->getWindowElement();
    m_windowElement->resize(getWidth(), getHeight());

    m_openGLContext = make_unique<Win32::UWPOpenGLContext>();

    m_windowElement->setHeadsetAvailable(MixedReality::MixedRealityContext::headsetAvailable());

    if (m_mixedRealityContext) {
        m_mixedRealityContext->initializeScriptHost();

        m_spatialInput = make_unique<HoloJs::UWP::UWPSpatialInput>();
        m_spatialInput->setCoreWindow(m_window.Get());
        m_spatialInput->setScriptWindow(m_windowElement);
        m_spatialInput->setFrameOfReference(m_mixedRealityContext->getStationaryFrameOfReference());
        RETURN_IF_FAILED(m_spatialInput->initialize());

        m_openGLContext->setMixedRealityContext(m_mixedRealityContext);
        m_openGLContext->initialize();
    } else {
        m_openGLContext->setCoreWindow(m_window);
        m_openGLContext->initialize();
        m_width = static_cast<float>(m_openGLContext->getWidth());
        m_height = static_cast<float>(m_openGLContext->getHeight());
    }

    m_timers = make_unique<HoloJs::Timers>();
    m_timers->setTimersImplementation(new WinRTTimers());
    m_timers->initialize();

    return S_OK;
}

void HoloJsUWPApp::registerEventHandlers()
{
    m_window->KeyUp += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^,
                                                                      Windows::UI::Core::KeyEventArgs ^>(
        this, &HoloJs::UWP::HoloJsUWPApp::OnKeyUp);

    m_window->KeyDown += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^,
                                                                        Windows::UI::Core::KeyEventArgs ^>(
        this, &HoloJs::UWP::HoloJsUWPApp::OnKeyDown);

    m_window->PointerWheelChanged +=
        ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^,
                                                       Windows::UI::Core::PointerEventArgs ^>(
            this, &HoloJs::UWP::HoloJsUWPApp::OnPointerWheelChanged);

    m_window->PointerPressed += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^,
                                                                               Windows::UI::Core::PointerEventArgs ^>(
        this, &HoloJs::UWP::HoloJsUWPApp::OnPointerPressed);

    m_window->PointerReleased += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^,
                                                                                Windows::UI::Core::PointerEventArgs ^>(
        this, &HoloJs::UWP::HoloJsUWPApp::OnPointerReleased);

    m_window->PointerMoved += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^,
                                                                             Windows::UI::Core::PointerEventArgs ^>(
        this, &HoloJs::UWP::HoloJsUWPApp::OnPointerMoved);
}

// Called when the CoreWindow object is created (or re-created).
void HoloJsUWPApp::SetWindow(CoreWindow ^ window)
{
    m_window = window;
    window->SizeChanged +=
        ref new TypedEventHandler<CoreWindow ^, WindowSizeChangedEventArgs ^>(this, &HoloJsUWPApp::OnWindowSizeChanged);

    window->VisibilityChanged +=
        ref new TypedEventHandler<CoreWindow ^, VisibilityChangedEventArgs ^>(this, &HoloJsUWPApp::OnVisibilityChanged);

    window->Closed +=
        ref new TypedEventHandler<CoreWindow ^, CoreWindowEventArgs ^>(this, &HoloJsUWPApp::OnWindowClosed);

    registerEventHandlers();

    DisplayInformation ^ currentDisplayInformation = DisplayInformation::GetForCurrentView();

    currentDisplayInformation->DpiChanged +=
        ref new TypedEventHandler<DisplayInformation ^, Object ^>(this, &HoloJsUWPApp::OnDpiChanged);

    currentDisplayInformation->OrientationChanged +=
        ref new TypedEventHandler<DisplayInformation ^, Object ^>(this, &HoloJsUWPApp::OnOrientationChanged);

    DisplayInformation::DisplayContentsInvalidated +=
        ref new TypedEventHandler<DisplayInformation ^, Object ^>(this, &HoloJsUWPApp::OnDisplayContentsInvalidated);

    if (MixedReality::MixedRealityContext::headsetAvailable()) {
        m_mixedRealityContext = make_shared<HoloJs::UWP::UWPMixedRealityContext>();
        m_mixedRealityContext->setCoreWindow(window);
        m_mixedRealityContext->setScriptWindow(m_windowElement);
        m_mixedRealityContext->initializeHolographicSpace();
    }

    m_dispatcher = window->Dispatcher;
}

// Initializes scene resources, or loads a previously saved app state.
void HoloJsUWPApp::Load(Platform::String ^ entryPoint) {}

bool HoloJsUWPApp::Render()
{
    Windows::Graphics::Holographic::HolographicFrame ^ frame;
    if (m_mixedRealityContext) {
        // Back buffers can change from frame to frame. Validate each buffer, and recreate
        // resource views and depth buffers as needed.
        frame = m_mixedRealityContext->createHolographicFrame();

        if (frame) {
            m_windowElement->vSync();
            glFlush();
            m_mixedRealityContext->render(frame, m_openGLContext->getWebGLTexture());
            m_mixedRealityContext->present(frame);
        }

    } else {
        m_windowElement->vSync();
        glFlush();

        if (FAILED(m_openGLContext->swap())) {
            m_windowElement->deviceLost();
            m_openGLContext->setCoreWindow(m_window);
            m_openGLContext->initialize();
            m_width = static_cast<float>(m_openGLContext->getWidth());
            m_height = static_cast<float>(m_openGLContext->getHeight());
            m_windowElement->resize(getWidth(), getHeight());
            m_windowElement->deviceRestored();
        }
    }

    return true;
}

// This method is called after the window becomes active.
void HoloJsUWPApp::Run()
{
    initializeRenderingResources();

    while (!m_windowClosed && !m_closeRequested) {
        const auto viewReady =
            !m_mixedRealityContext || (m_mixedRealityContext && m_mixedRealityContext->isCameraAvailable());

        if (viewReady) {
            executeOneForegroundWorkItem();
        }

        if (m_windowVisible) {
            CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

            if (m_activeApp) {
                if (!m_mixedRealityContext) {
                    m_width = static_cast<float>(m_openGLContext->getWidth());
                    m_height = static_cast<float>(m_openGLContext->getHeight());
                    m_windowElement->resize(getWidth(), getHeight());
                }
                Render();
            }
        } else {
            CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(
                CoreProcessEventsOption::ProcessOneAndAllPending);
        }
    }

    // Drain foreground workitems queue
    {
        std::lock_guard<std::recursive_mutex> guard(m_foregroundWorkItemQueue);
        while (m_foregroundWorkItems.size() > 0) {
            executeOneForegroundWorkItem();
        }
    }

    if (m_activeApp) {
        stopApp();
    }
}

void HoloJsUWPApp::onSpeechRecognized(std::wstring command, double confidence)
{
    auto match = g_voiceCommands.find(command);
    if (match != g_voiceCommands.end()) {
        if (match->second == VoiceCommandIntent::LoadOtherApp) {
            m_host->showHostRenderedElement(HoloJs::HostRenderedElements::QrScanGuide);
            m_qrScanner->tryDecode();
        }
    }
}

void HoloJsUWPApp::onQrRecognized(bool success, std::wstring text)
{
    if (success) {
        m_host->executeUri(text.c_str());
    } else {
        m_host->showHostRenderedElement(HoloJs::HostRenderedElements::NothingLoaded);
    }
}

// Required for IFrameworkView.
// Terminate events do not cause Uninitialize to be called. It will be called if your IFrameworkView
// class is torn down while the app is in the foreground.
void HoloJsUWPApp::Uninitialize() {}

// Application lifecycle event handlers.

void HoloJsUWPApp::OnActivated(CoreApplicationView ^ applicationView, IActivatedEventArgs ^ args)
{
    static PCWSTR holojsProtocol = L"xrs:";
    // On protocol activation, use the URI from the activation as the app URI
    if (args->Kind == ActivationKind::File) {
        auto eventArgs = dynamic_cast<FileActivatedEventArgs ^>(args);

        if (eventArgs->Files->Size > 0) {
            auto item = eventArgs->Files->GetAt(0);
            if (item->IsOfType(Windows::Storage::StorageItemTypes::File)) {
                auto file = safe_cast<Windows::Storage::StorageFile ^>(item);
                HANDLE fileHandle;
                if (SUCCEEDED(getHandleFromStorageFile(file, &fileHandle))) {
                    m_host->executePackageFromHandle(fileHandle);
                }
            }
        }
    } else if (args->Kind == ActivationKind::Protocol) {
        auto eventArgs = dynamic_cast<ProtocolActivatedEventArgs ^>(args);
        wstring url = eventArgs->Uri->AbsoluteUri->Data();

        if (_wcsnicmp(url.c_str(), holojsProtocol, wcslen(holojsProtocol)) == 0) {
            auto underlyingUri = url.substr(wcslen(holojsProtocol));
            m_host->executeUri(underlyingUri.c_str());
        }
    }

    // Run() won't start until the CoreWindow is activated.
    CoreWindow::GetForCurrentThread()->Activate();
}

void HoloJsUWPApp::OnSuspending(Platform::Object ^ sender, SuspendingEventArgs ^ args)
{
    // Save app state asynchronously after requesting a deferral. Holding a deferral
    // indicates that the application is busy performing suspending operations. Be
    // aware that a deferral may not be held indefinitely. After about five seconds,
    // the app will be forced to exit.
    SuspendingDeferral ^ deferral = args->SuspendingOperation->GetDeferral();

    create_task([this, deferral]() {
        if (m_mixedRealityContext) {
        } else {
            // TODO: TRIM
        }

        // Insert your code here.

        deferral->Complete();
    });
}

void HoloJsUWPApp::OnResuming(Platform::Object ^ sender, Platform::Object ^ args)
{
    // Restore any data or state that was unloaded on suspend. By default, data
    // and state are persisted when resuming from suspend. Note that this event
    // does not occur if the app was previously terminated.

    // Insert your code here.
    m_voiceInput->start();
}

// Window event handlers.

void HoloJsUWPApp::OnWindowSizeChanged(CoreWindow ^ sender, WindowSizeChangedEventArgs ^ args)
{
    if (!m_mixedRealityContext) {
        m_sizeAvailable = true;

        if (m_windowElement != nullptr) {
            m_windowElement->resize(getWidth(), getHeight());
        }
    }
}

void HoloJsUWPApp::OnVisibilityChanged(CoreWindow ^ sender, VisibilityChangedEventArgs ^ args)
{
    m_windowVisible = args->Visible;
}

void HoloJsUWPApp::OnWindowClosed(CoreWindow ^ sender, CoreWindowEventArgs ^ args) { m_windowClosed = true; }

// DisplayInformation event handlers.

void HoloJsUWPApp::OnDpiChanged(DisplayInformation ^ sender, Object ^ args) {}

void HoloJsUWPApp::OnOrientationChanged(DisplayInformation ^ sender, Object ^ args) {}

void HoloJsUWPApp::OnDisplayContentsInvalidated(DisplayInformation ^ sender, Object ^ args) {}

WinRTHoloJsView ::~WinRTHoloJsView() {}

void WinRTHoloJsView::run() { CoreApplication::Run(m_viewSource); }

void WinRTHoloJsView::stop() { m_app->Close(); }

HRESULT WinRTHoloJsView::initialize(HoloJs::IHoloJsScriptHostInternal* host)
{
    m_app->initializeScript(host);
    return S_OK;
}

void WinRTHoloJsView::onError(HoloJs::ScriptHostErrorType errorType) { OutputDebugString(L"Error"); }

void getKeyAndCodeFromVirtualKey(Windows::System::VirtualKey vKeyCode, wstring& key, wstring& code)
{
    if ((vKeyCode >= Windows::System::VirtualKey::A && vKeyCode <= Windows::System::VirtualKey::Z) ||
        (vKeyCode >= Windows::System::VirtualKey::Number0 && vKeyCode <= Windows::System::VirtualKey::Number9)) {
        key = static_cast<char>(vKeyCode);
        code = key;
    }
}

void HoloJs::UWP::HoloJsUWPApp::OnKeyUp(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::KeyEventArgs ^ args)
{
    if (m_windowElement != nullptr) {
        wstring key;
        wstring code;
        getKeyAndCodeFromVirtualKey(args->VirtualKey, key, code);
        m_windowElement->keyEvent(HoloJs::KeyEventType::KeyUp, key, code, 0, static_cast<int>(args->VirtualKey));
    }
}

void HoloJs::UWP::HoloJsUWPApp::OnKeyDown(Windows::UI::Core::CoreWindow ^ sender,
                                          Windows::UI::Core::KeyEventArgs ^ args)
{
    if (m_windowElement != nullptr) {
        wstring key;
        wstring code;
        getKeyAndCodeFromVirtualKey(args->VirtualKey, key, code);
        m_windowElement->keyEvent(HoloJs::KeyEventType::KeyDown, key, code, 0, static_cast<int>(args->VirtualKey));
    }
}

unsigned int getMouseButtons(Windows::UI::Core::PointerEventArgs ^ args)
{
    unsigned int buttonsMask = 0;
    if (args->CurrentPoint->Properties->IsLeftButtonPressed) {
        buttonsMask |= static_cast<int>(HoloJs::MouseButton::Main);
    } else if (args->CurrentPoint->Properties->IsMiddleButtonPressed) {
        buttonsMask |= static_cast<int>(HoloJs::MouseButton::Auxiliary);
    } else if (args->CurrentPoint->Properties->IsRightButtonPressed) {
        buttonsMask |= static_cast<int>(HoloJs::MouseButton::Second);
    }

    return buttonsMask;
}

unsigned int getFirstMouseButtonPressed(Windows::UI::Core::PointerEventArgs ^ args)
{
    if (args->CurrentPoint->Properties->IsLeftButtonPressed) {
        return static_cast<int>(HoloJs::MouseButton::Main);
    } else if (args->CurrentPoint->Properties->IsMiddleButtonPressed) {
        return static_cast<int>(HoloJs::MouseButton::Auxiliary);
    } else if (args->CurrentPoint->Properties->IsRightButtonPressed) {
        return static_cast<int>(HoloJs::MouseButton::Second);
    }

    return 0;
}

void HoloJs::UWP::HoloJsUWPApp::OnPointerWheelChanged(Windows::UI::Core::CoreWindow ^ sender,
                                                      Windows::UI::Core::PointerEventArgs ^ args)
{
    if (m_windowElement != nullptr) {
        m_windowElement->mouseEvent(HoloJs::MouseButtonEventType::Wheel,
                                    static_cast<int>(args->CurrentPoint->Position.X),
                                    static_cast<int>(args->CurrentPoint->Position.Y),
                                    args->CurrentPoint->Properties->MouseWheelDelta,
                                    getMouseButtons(args));
    }
}

void HoloJs::UWP::HoloJsUWPApp::OnPointerPressed(Windows::UI::Core::CoreWindow ^ sender,
                                                 Windows::UI::Core::PointerEventArgs ^ args)
{
    if (m_windowElement != nullptr) {
        m_windowElement->mouseEvent(HoloJs::MouseButtonEventType::Down,
                                    static_cast<int>(args->CurrentPoint->Position.X),
                                    static_cast<int>(args->CurrentPoint->Position.Y),
                                    getFirstMouseButtonPressed(args),
                                    getMouseButtons(args));
    }
}

void HoloJs::UWP::HoloJsUWPApp::OnPointerReleased(Windows::UI::Core::CoreWindow ^ sender,
                                                  Windows::UI::Core::PointerEventArgs ^ args)
{
    if (m_windowElement != nullptr) {
        m_windowElement->mouseEvent(HoloJs::MouseButtonEventType::Up,
                                    static_cast<int>(args->CurrentPoint->Position.X),
                                    static_cast<int>(args->CurrentPoint->Position.Y),
                                    getFirstMouseButtonPressed(args),
                                    getMouseButtons(args));
    }
}

void HoloJs::UWP::HoloJsUWPApp::OnPointerMoved(Windows::UI::Core::CoreWindow ^ sender,
                                               Windows::UI::Core::PointerEventArgs ^ args)
{
    if (m_windowElement != nullptr) {
        m_windowElement->mouseEvent(HoloJs::MouseButtonEventType::Move,
                                    static_cast<int>(args->CurrentPoint->Position.X),
                                    static_cast<int>(args->CurrentPoint->Position.Y),
                                    getFirstMouseButtonPressed(args),
                                    getMouseButtons(args));
    }
}
