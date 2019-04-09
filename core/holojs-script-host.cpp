#include "holojs-script-host.h"
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

const wchar_t* g_InternalScriptNames[] = {
    L"window.js",
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
};

IHoloJsScriptHost* __cdecl HoloJs::PrivateInterface::CreateHoloJsScriptHost() { return new HoloJsScriptHost(); }

void __cdecl HoloJs::PrivateInterface::DeleteHoloJsScriptHost(IHoloJsScriptHost* scriptHost) { delete scriptHost; }

HoloJsScriptHost::HoloJsScriptHost() { m_scriptsLoader = make_unique<ScriptsLoader>(); }

HoloJsScriptHost::~HoloJsScriptHost()
{
	// Stop queueing items
    m_runningState = HostState::Stopping;

    // Wait until all outstanding work items are done (except this one)
    while (m_outstandingWorkItems > 0) {
        Sleep(100);
    }

    m_view.reset();
    cleanupScriptContext();
}

long HoloJsScriptHost::initialize(ViewConfiguration viewConfig)
{
    RETURN_IF_TRUE(m_runningState != HostState::NotInitialized);

    m_view.reset(HoloJs::getPlatform()->makeView(viewConfig));
    m_view->setIcon(m_viewIcon);
    m_view->setTitle(m_viewTitle);

    if (m_targetViewWindow != nullptr) {
        m_view->setViewWindow(m_targetViewWindow);
    }

    loadSupportScripts();

    RETURN_IF_FAILED(m_view->initialize(this));

    m_runningState = HostState::Initialized;

    return HoloJs::Success;
}

long HoloJsScriptHost::initializeScriptContext()
{
    m_activeContext = make_unique<HoloJs::ScriptContext>();
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

long HoloJsScriptHost::cleanupScriptContext()
{
    m_activeContext.reset();
    return HoloJs::Success;
}

long HoloJsScriptHost::startUri(const wchar_t* appUrl)
{
    RETURN_IF_TRUE(m_runningState != HostState::Initialized);

    wstring appUrlCapture(appUrl);

    m_runningState = HostState::Loading;
    runInBackground([this, appUrlCapture]() -> long {
        auto result = m_scriptsLoader->loadApp(appUrlCapture, m_loadedApp);

        if (HOLOJS_FAILED(result)) {
            m_runningState = HostState::Idle;
            m_view->onError(ScriptHostErrorType::LoadingError);
        } else {
            m_runningState = HostState::Running;
            runInScriptContext([this]() {
                EXIT_IF_FAILED(initializeScriptContext());
                m_view->executeApp(m_loadedApp);
            });
        }

        return HoloJs::Success;
    });

    m_view->run();

    return HoloJs::Success;
}

long HoloJsScriptHost::start(const wchar_t* script)
{
    RETURN_IF_TRUE(m_runningState != HostState::Initialized);

    RETURN_IF_FAILED(initializeScriptContext());

    wstring scriptCapture(script);

    m_runningState = HostState::Loading;
    runInBackground([this, scriptCapture]() -> long {
        auto result = m_scriptsLoader->loadScriptInline(scriptCapture, m_loadedApp);

        if (HOLOJS_FAILED(result)) {
            m_runningState = HostState::Idle;
            m_view->onError(ScriptHostErrorType::LoadingError);
        } else {
            m_runningState = HostState::Running;
            m_view->executeApp(m_loadedApp);
        }

        return HoloJs::Success;
    });

    m_view->run();

    return HoloJs::Success;
}

long HoloJsScriptHost::start()
{
    m_runningState = HostState::Idle;
    m_view->run();
    return HoloJs::Success;
}

long HoloJsScriptHost::startWithEmptyApp()
{
    RETURN_IF_TRUE(m_runningState != HostState::Initialized);

    RETURN_IF_FAILED(cleanupScriptContext());
    RETURN_IF_FAILED(initializeScriptContext());

    m_runningState = HostState::Loading;
    auto result = m_scriptsLoader->createEmptyApp(m_loadedApp);
    m_runningState = HostState::Running;
    m_view->executeApp(m_loadedApp);
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
    auto result = m_scriptsLoader->loadApp(platformHandle, m_loadedApp);

    if (HOLOJS_FAILED(result)) {
        m_runningState = HostState::Idle;
        m_view->onError(ScriptHostErrorType::LoadingError);
    } else {
        m_runningState = HostState::Running;
        m_view->executeApp(m_loadedApp);
    }
}

void HoloJsScriptHost::startExecution(const wstring appUrl)
{
    auto result = m_scriptsLoader->loadApp(appUrl, m_loadedApp);

    if (HOLOJS_FAILED(result)) {
        m_runningState = HostState::Idle;
        m_view->onError(ScriptHostErrorType::LoadingError);
    } else {
        m_runningState = HostState::Running;
        m_view->executeApp(m_loadedApp);
    }
}

long HoloJsScriptHost::executeUri(const wchar_t* appUrl)
{
    wstring appUrlCapture(appUrl);
    RETURN_IF_TRUE(m_runningState != HostState::Idle && m_runningState != HostState::Running);

    if (m_runningState == HostState::Running) {
        runInBackground([this, appUrlCapture]() -> long {
            // Stop queueing items
            m_runningState = HostState::Stopping;

            // Wait until all outstanding work items are done (except this one)
            while (m_outstandingWorkItems > 1) {
                Sleep(100);
            }

            // We're sneaking a work item on the view thread to cleanup the script context
            {
                std::lock_guard<std::mutex> guard(m_workItemQueueLock);
                m_outstandingWorkItems++;
            }

            m_view->executeOnViewThread(
                new ScriptContextWorkItem(this, make_shared<std::function<void()>>([this, appUrlCapture]() {
                                              EXIT_IF_FAILED(cleanupScriptContext());
                                              EXIT_IF_FAILED(initializeScriptContext());

                                              m_runningState = HostState::Loading;
                                              runInBackground([this, appUrlCapture]() -> long {
                                                  startExecution(appUrlCapture);
                                                  return HoloJs::Success;
                                              });
                                          })));

            return HoloJs::Success;
        });
    } else {
        RETURN_IF_FAILED(cleanupScriptContext());
        RETURN_IF_FAILED(initializeScriptContext());

        m_runningState = HostState::Loading;
        runInBackground([this, appUrlCapture]() -> long {
            startExecution(appUrlCapture);
            return HoloJs::Success;
        });
    }

    return HoloJs::Success;
}

long HoloJsScriptHost::executePackageFromHandle(void* platformPackageHandle)
{
    RETURN_IF_TRUE(m_runningState != HostState::Idle && m_runningState != HostState::Running);

    if (m_runningState == HostState::Running) {
        runInBackground([this, platformPackageHandle]() -> long {
            // Stop queueing items
            m_runningState = HostState::Stopping;

            // Wait until all outstanding work items are done (except this one)
            while (m_outstandingWorkItems > 1) {
                Sleep(100);
            }

            // We're sneaking a work item on the view thread to cleanup the script context
            {
                std::lock_guard<std::mutex> guard(m_workItemQueueLock);
                m_outstandingWorkItems++;
            }

            m_view->executeOnViewThread(
                new ScriptContextWorkItem(this, make_shared<std::function<void()>>([this, platformPackageHandle]() {
                                              EXIT_IF_FAILED(cleanupScriptContext());
                                              EXIT_IF_FAILED(initializeScriptContext());

                                              m_runningState = HostState::Loading;
                                              runInBackground([this, platformPackageHandle]() -> long {
                                                  startExecution(platformPackageHandle);
                                                  return HoloJs::Success;
                                              });
                                          })));

            return HoloJs::Success;
        });
    } else {
        RETURN_IF_FAILED(cleanupScriptContext());
        RETURN_IF_FAILED(initializeScriptContext());

        m_runningState = HostState::Loading;
        runInBackground([this, platformPackageHandle]() -> long {
            startExecution(platformPackageHandle);
            return HoloJs::Success;
        });
    }

    return HoloJs::Success;
}

long HoloJsScriptHost::execute(const wchar_t* script)
{
    wstring scriptCapture(script);
    RETURN_IF_TRUE(m_runningState != HostState::Running);
    m_view->executeScript(script);

    return HoloJs::Success;
}

long HoloJsScriptHost::executeImmediate(const wchar_t* script, const wchar_t* scriptName)
{
    return m_activeContext->execute(script, scriptName);
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

void HoloJsScriptHost::runInBackground(std::function<long()> backgroundWork)
{
    std::lock_guard<std::mutex> guard(m_workItemQueueLock);

    EXIT_IF_TRUE(m_runningState != HostState::Loading && m_runningState != HostState::Running);

    m_view->executeInBackground(new BackgroundWorkItem(this, make_shared<std::function<long()>>(backgroundWork)));
    m_outstandingWorkItems++;
}

void HoloJsScriptHost::runInScriptContext(std::function<void()> contextWork)
{
    std::lock_guard<std::mutex> guard(m_workItemQueueLock);

    EXIT_IF_TRUE(m_runningState != HostState::Loading && m_runningState != HostState::Running);

    m_view->executeOnViewThread(new ScriptContextWorkItem(this, make_shared<std::function<void()>>(contextWork)));
    m_outstandingWorkItems++;
}

void HoloJsScriptHost::backgroundWorkItemComplete(BackgroundWorkItem* workItem)
{
    std::lock_guard<std::mutex> guard(m_workItemQueueLock);

    assert(m_outstandingWorkItems > 0);
    m_outstandingWorkItems--;
}

void HoloJsScriptHost::contextWorkItemComplete(ScriptContextWorkItem* workItem)
{
    std::lock_guard<std::mutex> guard(m_workItemQueueLock);
    assert(m_outstandingWorkItems > 0);
    m_outstandingWorkItems--;
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