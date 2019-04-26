#include "holojs-script-host.h"
#include "async-work-items.h"
#include "host-interfaces.h"
#include "include/holojs/private/chakra.h"
#include "include/holojs/private/platform-interfaces.h"
#include "timers.h"
#include "window-element.h"
#include <cassert>
#include <string>

using namespace HoloJs;
using namespace HoloJs::AppModel;
using namespace std;

const wchar_t* g_InternalScriptNames[] = {L"window.js",
                                          L"URL.js",
                                          L"console.js",
                                          L"timers.js",
                                          L"2d-context.js",
                                          L"image.js",
                                          L"canvas.js",
                                          L"canvas-vr.js",
                                          L"webgl-context.js",
                                          L"document.js",
                                          L"webvr.js",
                                          L"gamepad.js",
                                          L"xmlhttprequest.js",
                                          L"webaudio.js",
                                          L"websocket.js",
                                          L"surface-mapper.js",
                                          L"speech-recognizer.js",
                                          L"spatial-anchors.js"};

const wchar_t* g_loadingAnimationScripts[] = {L"three.js", L"loading-animation.js"};

IHoloJsScriptHost* __cdecl HoloJs::PrivateInterface::CreateHoloJsScriptHost() { return new HoloJsScriptHost(); }

void __cdecl HoloJs::PrivateInterface::DeleteHoloJsScriptHost(IHoloJsScriptHost* scriptHost) { delete scriptHost; }

HoloJsScriptHost::HoloJsScriptHost()
{
    m_scriptsLoader = make_unique<ScriptsLoader>();
    m_backgroundExecutionQueue = make_shared<HoloJs::BackgroundExecutionQueue>();
}

HoloJsScriptHost::~HoloJsScriptHost()
{
    while (!m_backgroundExecutionQueue->isEmpty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }

    m_view.reset();
    releaseExecutionContext();
}

long HoloJsScriptHost::initialize(ViewConfiguration viewConfig)
{
    RETURN_IF_TRUE(m_runningState != HostState::NotInitialized);

    m_viewConfiguration = viewConfig;

    m_view.reset(HoloJs::getPlatform()->makeView(viewConfig));
    m_view->setIcon(m_viewIcon);
    m_view->setTitle(m_viewTitle);

    if (m_targetViewWindow != nullptr) {
        m_view->setViewWindow(m_targetViewWindow);
    }

    RETURN_IF_FAILED(loadSupportScripts());
    RETURN_IF_FAILED(createLoadingAnimation());

    RETURN_IF_FAILED(m_view->initialize(this));

    m_runningState = HostState::Initialized;

    return HoloJs::Success;
}

long HoloJsScriptHost::executeLoadedApp(std::shared_ptr<HoloJs::AppModel::HoloJsApp> app)
{
    m_loadedApp = app;

    auto loadedScripts = app->loadedScriptsList();

    for (list<Script>::const_iterator it = loadedScripts->begin(); it != loadedScripts->end(); ++it) {
        RETURN_IF_FAILED(executeImmediate(it->getCode().c_str(), it->getName().c_str()));
    }

    if (app == m_internalApp) {
        RETURN_IF_FAILED(initializeHostRenderedElement());
    } else {
        m_hostRenderedElements = HostRenderedElements::None;
    }

    return S_OK;
}

long HoloJsScriptHost::initializeHostRenderedElement()
{
    if (m_hostRenderedElements == HostRenderedElements::LoadingAnimation) {
        RETURN_IF_FAILED(executeImmediate(L"renderLoadingAnimation();", L"inline"));
    } else if (m_hostRenderedElements == HostRenderedElements::LoadingFailed) {
        RETURN_IF_FAILED(executeImmediate(L"loadingFailedMessage();", L"inline"));
    } else if (m_hostRenderedElements == HostRenderedElements::NothingLoaded) {
        if (m_view)
            RETURN_IF_FAILED(executeImmediate(m_viewConfiguration.enableQrCodeNavigation ? L"getStartedMessageWithQr();"
                                                                                         : L"getStartedMessageNoQr();",
                                              L"inline"));
    } else if (m_hostRenderedElements == HostRenderedElements::QrScanGuide) {
        RETURN_IF_FAILED(executeImmediate(L"renderQRScanGuide();", L"inline"));
    }

    return S_OK;
}

long HoloJsScriptHost::createExecutionContext()
{
    releaseExecutionContext();

    m_activeContext = make_shared<HoloJs::ScriptContext>();

    m_activeContext->enableDebug(m_debugRequested);
    m_activeContext->setView(m_view);
    m_activeContext->setHost(this);

    RETURN_IF_FAILED(m_activeContext->initialize());

    if (m_view->isWindingOrderReversed()) {
        m_activeContext->reverseWebGlWindingOrder();
    }

    m_activeContext->setConsoleConfig(m_consoleConfiguration);

    return HoloJs::Success;
}

long HoloJsScriptHost::releaseExecutionContext()
{
    // Destroys all resources associated, waits for queued work items, etc.
    if (m_activeContext) {
        m_activeContext->destroy();
    }

    // Release the script context; because the context might have queued work items, deletion might be delayed until
    // all the work items exit
    m_activeContext.reset();
    return HoloJs::Success;
}

long HoloJsScriptHost::startUri(const wchar_t* appUrl)
{
    RETURN_IF_TRUE(m_runningState != HostState::Initialized);

    wstring appUrlCapture(appUrl);

    runInBackgroundNoContext([this, appUrlCapture]() -> long {
        showInternalUI(HostRenderedElements::LoadingAnimation);

        shared_ptr<HoloJs::AppModel::HoloJsApp> newApp;
        auto result = m_scriptsLoader->loadApp(appUrlCapture, newApp);

        if (HOLOJS_FAILED(result)) {
            showInternalUI(HostRenderedElements::LoadingFailed);
        } else {
            m_view->executeApp(newApp);
        }

        return HoloJs::Success;
    });

    m_view->run();

    return HoloJs::Success;
}

long HoloJsScriptHost::start(const wchar_t* script)
{
    RETURN_IF_TRUE(m_runningState != HostState::Initialized);

    wstring scriptCapture(script);

    runInBackgroundNoContext([this, scriptCapture]() -> long {
        showInternalUI(HostRenderedElements::LoadingAnimation);

        shared_ptr<HoloJs::AppModel::HoloJsApp> newApp;
        auto result = m_scriptsLoader->loadScriptInline(scriptCapture, newApp);

        if (HOLOJS_FAILED(result)) {
            showInternalUI(HostRenderedElements::LoadingFailed);
        } else {
            m_view->executeApp(newApp);
        }

        return HoloJs::Success;
    });

    m_view->run();

    return HoloJs::Success;
}

long HoloJsScriptHost::startWithEmptyApp()
{
    RETURN_IF_TRUE(m_runningState != HostState::Initialized);

    runInBackgroundNoContext([this]() -> long {
        showInternalUI(HostRenderedElements::NothingLoaded);
        return S_OK;
    });

    m_view->run();

    return HoloJs::Success;
}

long HoloJsScriptHost::stopExecution()
{
    m_view->stop();
    m_runningState = HostState::NotInitialized;

    return HoloJs::Success;
}

void HoloJsScriptHost::startExecution(void* platformHandle)
{
    shared_ptr<HoloJs::AppModel::HoloJsApp> newApp;
    auto result = m_scriptsLoader->loadApp(platformHandle, newApp);

    if (HOLOJS_FAILED(result)) {
        showInternalUI(HostRenderedElements::LoadingFailed);
    } else {
        m_view->executeApp(newApp);
    }
}

void HoloJsScriptHost::startExecution(const wstring appUrl)
{
    shared_ptr<HoloJs::AppModel::HoloJsApp> newApp;
    auto result = m_scriptsLoader->loadApp(appUrl, newApp);

    if (HOLOJS_FAILED(result)) {
        showInternalUI(HostRenderedElements::LoadingFailed);
    } else {
        m_view->executeApp(newApp);
    }
}

long HoloJsScriptHost::executeUri(const wchar_t* appUrl)
{
    RETURN_IF_TRUE(m_runningState != HostState::Initialized);

    wstring appUrlCapture(appUrl);

    showInternalUI(HostRenderedElements::LoadingAnimation);

    runInBackgroundNoContext([this, appUrlCapture]() -> long {
        startExecution(appUrlCapture);
        return HoloJs::Success;
    });

    return HoloJs::Success;
}

long HoloJsScriptHost::showHostRenderedElement(HoloJs::HostRenderedElements element)
{
    showInternalUI(element);
    return S_OK;
}

long HoloJsScriptHost::executePackageFromHandle(void* platformPackageHandle)
{
    RETURN_IF_TRUE(m_runningState != HostState::Initialized);

    showInternalUI(HostRenderedElements::LoadingAnimation);

    runInBackgroundNoContext([this, platformPackageHandle]() -> long {
        startExecution(platformPackageHandle);
        return HoloJs::Success;
    });

    return HoloJs::Success;
}

long HoloJsScriptHost::execute(const wchar_t* script)
{
    m_view->executeScript(script);
    return HoloJs::Success;
}

long HoloJsScriptHost::executeImmediate(const wchar_t* script, const wchar_t* scriptName)
{
    return m_activeContext->execute(script, scriptName);
}

void HoloJsScriptHost::showInternalUI(HostRenderedElements type)
{
    if (m_debugRequested) {
        return;
    }

    if (m_loadedApp != m_internalApp) {
        m_hostRenderedElements = type;
        m_view->executeApp(m_internalApp);
    } else if (m_hostRenderedElements != type) {
        m_hostRenderedElements = type;
        runInScriptContext([this]() { initializeHostRenderedElement(); });
    }
}

void HoloJsScriptHost::enableDebugger() { m_debugRequested = true; }

long HoloJsScriptHost::createScriptObject(HoloJs::ResourceManagement::IRelease* nativeObject,
                                          HoloJs::ResourceManagement::ObjectType type,
                                          JsValueRef* scriptObject)
{
    *scriptObject = m_activeContext->getResourceManager()->objectToDirectExternal(nativeObject, type);

    return (*scriptObject == JS_INVALID_REFERENCE ? HoloJs::Error : HoloJs::Success);
}

long HoloJsScriptHost::getActiveAppConfiguration(HoloJs::AppModel::AppConfiguration* configuration)
{
    RETURN_IF_FALSE((bool)m_loadedApp);
    *configuration = m_loadedApp->getAppConfiguration();

    return HoloJs::Success;
}

HoloJs::IWindow* HoloJsScriptHost::getWindowElement() { return m_activeContext->getWindowElement(); }

long HoloJsScriptHost::runInBackgroundNoContext(std::function<long()> backgroundWork)
{
    RETURN_IF_FALSE(m_view);

    m_view->executeInBackground(
        new HostBackgroundWorkItem(m_backgroundExecutionQueue, make_shared<std::function<long()>>(backgroundWork)));

	return HoloJs::Success;
}

long HoloJsScriptHost::runInBackground(std::function<long()> backgroundWork)
{
    RETURN_IF_FALSE(m_view);

    if (!m_activeContext) {
        runInBackgroundNoContext(backgroundWork);
    } else {
        m_view->executeInBackground(
            new BackgroundWorkItem(m_activeContext, make_shared<std::function<long()>>(backgroundWork)));
    }

	return HoloJs::Success;
}

long HoloJsScriptHost::runInScriptContext(std::function<void()> contextWork)
{
    RETURN_IF_FALSE(m_view);

    m_view->executeOnViewThread(
        new ForegroundWorkItem(m_activeContext, make_shared<std::function<void()>>(contextWork)));

	return HoloJs::Success;
}

long HoloJsScriptHost::loadSupportScripts()
{
    m_supportScripts = make_shared<std::list<HoloJs::AppModel::Script>>();

    for (auto& internalScriptName : g_InternalScriptNames) {
        std::wstring scriptText;
        RETURN_IF_FAILED(HoloJs::getPlatform()->readResourceScript(internalScriptName, scriptText));
        m_supportScripts->emplace_back(internalScriptName, move(scriptText));
        m_supportScripts->back().setName(internalScriptName);
    }

    m_scriptsLoader->setSupportScripts(m_supportScripts);

    return HoloJs::Success;
}

long HoloJsScriptHost::createLoadingAnimation()
{
    auto loadingAnimationScripts = make_shared<std::list<HoloJs::AppModel::Script>>();

    for (auto& internalScriptName : g_loadingAnimationScripts) {
        std::wstring scriptText;
        RETURN_IF_FAILED(HoloJs::getPlatform()->readResourceScript(internalScriptName, scriptText));
        loadingAnimationScripts->emplace_back(internalScriptName, move(scriptText));
        loadingAnimationScripts->back().setName(internalScriptName);
    }

    return m_scriptsLoader->createAppFromScripts(L"Loading", loadingAnimationScripts, m_internalApp);
}