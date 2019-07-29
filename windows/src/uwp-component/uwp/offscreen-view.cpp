#include "stdafx.h"
#include "../uwp-mixed-reality-context.h"
#include "../uwp-spatial-input.h"
#include "holojs/holojs-script-host.h"
#include "offscreen-view.h"
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

UWPOffscreenView::~UWPOffscreenView() {}

long UWPOffscreenView::executeOnViewThread(HoloJs::IForegroundWorkItem* workItem)
{
    CoreApplication::MainView->Dispatcher->RunAsync(CoreDispatcherPriority::Normal,
                                                    ref new DispatchedHandler([workItem]() {
                                                        workItem->execute();
                                                        delete workItem;
                                                    }));

    return S_OK;
}

long UWPOffscreenView::executeInBackground(HoloJs::IBackgroundWorkItem* workItem)
{
    create_task([workItem]() -> long {
        auto result = workItem->execute();
        delete workItem;
        return result;
    });

    return S_OK;
}

HRESULT UWPOffscreenView::executeApp(std::shared_ptr<HoloJs::AppModel::HoloJsApp> app)
{
    m_app = app;
    return S_OK;
}

HRESULT UWPOffscreenView::initialize(HoloJs::IHoloJsScriptHostInternal* host)
{
    m_host = host;

    if (MixedReality::MixedRealityContext::headsetAvailable()) {
        m_mixedRealityContext = make_shared<HoloJs::UWP::UWPMixedRealityContext>();
    }

    return S_OK;
}

void UWPOffscreenView::run() {}

void UWPOffscreenView::stop() {}

void UWPOffscreenView::onError(HoloJs::ScriptHostErrorType errorType) { OutputDebugString(L"Error"); }

HRESULT UWPOffscreenView::initializeScriptResources()
{
    m_windowElement = m_host->getWindowElement();

    m_coreWindowInputHandler = ref new CoreWindowInput();
    m_coreWindowInputHandler->setWindowElement(m_windowElement);
    m_coreWindowInputHandler->registerEventHandlers();

    RETURN_IF_FAILED(reinterpret_cast<IDXGISurface*>(m_viewConfiguration.offscreenRenderSurface)
                         ->QueryInterface(IID_PPV_ARGS(&m_renderSurface)));

    m_openGLContext = make_unique<Win32::UWPOpenGLContext>();
    m_openGLContext->setOffscreenRendering(m_renderSurface.Get());
    m_openGLContext->initialize();

    m_windowElement->resize(m_openGLContext->getWidth(), m_openGLContext->getHeight());

    m_windowElement->setHeadsetAvailable(MixedReality::MixedRealityContext::headsetAvailable());

    if (m_mixedRealityContext) {
        m_mixedRealityContext->setScriptWindow(m_windowElement);
        m_mixedRealityContext->initializeScriptHost();

        m_spatialInput = make_unique<HoloJs::UWP::UWPSpatialInput>();
        m_spatialInput->setCoreWindow(CoreApplication::MainView->CoreWindow);
        m_spatialInput->setScriptWindow(m_windowElement);
        m_spatialInput->setFrameOfReference(m_mixedRealityContext->getStationaryFrameOfReference());
        RETURN_IF_FAILED(m_spatialInput->initialize());
    }

    m_timers = make_unique<HoloJs::Timers>();
    m_timers->setTimersImplementation(new WinRTTimers());
    m_timers->initialize();

    return S_OK;
}

long UWPOffscreenView::render(void* renderParameters)
{
    if (m_app) {
        RETURN_IF_FAILED(m_host->createExecutionContext());
        RETURN_IF_FAILED(initializeScriptResources());

        RETURN_IF_FAILED(m_host->executeLoadedApp(m_app));

        m_app.reset();
        m_appLoaded = true;
    }

    if (renderParameters != nullptr && m_mixedRealityContext != nullptr) {
        auto framePrediction = safe_cast<Windows::Graphics::Holographic::HolographicFramePrediction ^>(
            reinterpret_cast<Platform::Object ^>(renderParameters));

		m_mixedRealityContext->setHolographicFramePrediction(framePrediction);
    }

    if (m_appLoaded) {
        RETURN_IF_FAILED(m_windowElement->vSync());
        glFlush();
    }

    return S_OK;
}