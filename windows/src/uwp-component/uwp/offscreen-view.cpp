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

long UWPOffscreenView::executeOnViewThread(HoloJs::IForegroundWorkItem* workItem) { return E_FAIL; }

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
    return S_OK;
}

void UWPOffscreenView::run() {}

void UWPOffscreenView::stop() {}

void UWPOffscreenView::onError(HoloJs::ScriptHostErrorType errorType) { OutputDebugString(L"Error"); }

HRESULT UWPOffscreenView::initializeScriptResources()
{
    m_windowElement = m_host->getWindowElement();

    // TODO fix
    

    RETURN_IF_FAILED(reinterpret_cast<IDXGISurface*>(m_viewConfiguration.offscreenRenderSurface)
                         ->QueryInterface(IID_PPV_ARGS(&m_renderSurface)));

    m_openGLContext = make_unique<Win32::UWPOpenGLContext>();
    m_openGLContext->setOffscreenRendering(m_renderSurface.Get());
    m_openGLContext->initialize();

	m_windowElement->resize(m_openGLContext->getWidth(), m_openGLContext->getHeight());

    m_windowElement->setHeadsetAvailable(MixedReality::MixedRealityContext::headsetAvailable());

    /*if (m_mixedRealityContext) {
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
    }*/

    m_timers = make_unique<HoloJs::Timers>();
    m_timers->setTimersImplementation(new WinRTTimers());
    m_timers->initialize();

    return S_OK;
}

long UWPOffscreenView::draw()
{
    if (m_app) {
        RETURN_IF_FAILED(m_host->createExecutionContext());
        RETURN_IF_FAILED(initializeScriptResources());

        RETURN_IF_FAILED(m_host->executeLoadedApp(m_app));

        m_app.reset();
        m_appLoaded = true;
    }

    if (m_appLoaded) {
        RETURN_IF_FAILED(m_windowElement->vSync());
        glFlush();
    }

    return S_OK;
}