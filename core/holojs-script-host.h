#pragma once
#include "host-interfaces.h"

#include "include/holojs/private/chakra.h"
#include "include/holojs/private/holojs-script-host-internal.h"
#include "resource-management/resource-manager.h"
#include "script-context.h"
#include "script-loader.h"

namespace HoloJs {

class HoloJsScriptHost : public IHoloJsScriptHostInternal {
   public:
    HoloJsScriptHost();

    virtual ~HoloJsScriptHost();

    virtual long initialize(ViewConfiguration viewConfig);
    virtual void setViewWindow(void* nativeWindow) { m_targetViewWindow = nativeWindow; }

    virtual long startUri(const wchar_t* appUri);
    virtual long start(const wchar_t* script);
    virtual long start();
    virtual long startWithEmptyApp();

    virtual long execute(const wchar_t* script);
    virtual long executeUri(const wchar_t* scriptUri);
    virtual long executePackageFromHandle(void* platformPackageHandle);

    virtual long stopExecution();

    virtual void runInBackground(std::function<long()> backgroundWork);
    virtual void runInScriptContext(std::function<void()> contextWork);
    virtual void backgroundWorkItemComplete(BackgroundWorkItem* workItem);
    virtual void contextWorkItemComplete(ScriptContextWorkItem* workItem);

    virtual void enableDebugger();
    virtual void setConsoleConfig(HoloJs::IConsoleConfiguration* consoleConfig)
    {
        m_consoleConfiguration = consoleConfig;
    }

    long executeImmediate(const wchar_t* script, const wchar_t* scriptName);

    virtual long createScriptObject(HoloJs::ResourceManagement::IRelease* nativeObject,
                                    HoloJs::ResourceManagement::ObjectType type,
                                    JsValueRef* scriptObject);

    virtual HoloJs::IWindow* getWindowElement();

    virtual void setWindowIcon(void* platformIcon) { m_viewIcon = platformIcon; }
    virtual void setWindowTitle(const wchar_t* title) { m_viewTitle = title; }

    virtual std::shared_ptr<HoloJs::IHoloJsView> getView() { return m_view; }

    virtual long getActiveAppConfiguration(HoloJs::AppModel::AppConfiguration* configuration);

   private:
    std::shared_ptr<HoloJs::AppModel::HoloJsApp> m_loadedApp;
    bool m_debugRequested;

    long loadSupportScripts();
    std::shared_ptr<std::list<HoloJs::AppModel::Script>> m_supportScripts;

    HoloJs::IConsoleConfiguration* m_consoleConfiguration;

    std::unique_ptr<HoloJs::AppModel::ScriptsLoader> m_scriptsLoader;

    std::shared_ptr<IHoloJsView> m_view;
    std::unique_ptr<HoloJs::ScriptContext> m_activeContext;

    void* m_targetViewWindow;

    void* m_viewIcon;
    std::wstring m_viewTitle;

    long initializeScriptContext();
    long cleanupScriptContext();

    std::mutex m_workItemQueueLock;
    unsigned int m_outstandingWorkItems = 0;

    enum class HostState
    {
        NotInitialized,
        Initialized,
        Idle,
        Loading,
        Stopping,
        Running
    };

    void startExecution(const std::wstring appUrl);
    void startExecution(void* platformHandle);

    HostState m_runningState = HostState::NotInitialized;

};
}  // namespace HoloJs
