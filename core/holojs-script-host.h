#pragma once
#include "host-interfaces.h"

#include "async-work-items.h"
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
    virtual long startWithEmptyApp();

    virtual long execute(const wchar_t* script);
    virtual long executeUri(const wchar_t* scriptUri);
    virtual long executePackageFromHandle(void* platformPackageHandle);

    virtual long stopExecution();

    virtual long runInBackground(std::function<long()> backgroundWork);
    virtual long runInScriptContext(std::function<void()> contextWork);

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

    virtual long getStationaryCoordinateSystem(void** coordinateSystem)
    {
        return m_view->getStationaryCoordinateSystem(coordinateSystem);
    }

    virtual long createExecutionContext();
    virtual long executeLoadedApp(std::shared_ptr<HoloJs::AppModel::HoloJsApp> app);

	virtual long showHostRenderedElement(HoloJs::HostRenderedElements element);

    const ViewConfiguration& getViewConfiguration() const { return m_viewConfiguration; }

   private:
    std::shared_ptr<HoloJs::AppModel::HoloJsApp> m_loadedApp;
    bool m_debugRequested;

    ViewConfiguration m_viewConfiguration;

    HostRenderedElements m_hostRenderedElements = HostRenderedElements::None;
    std::shared_ptr<HoloJs::AppModel::HoloJsApp> m_internalApp;
    void showInternalUI(HostRenderedElements type);
    long initializeHostRenderedElement();

    long runInBackgroundNoContext(std::function<long()> backgroundWork);

    long loadSupportScripts();
    std::shared_ptr<std::list<HoloJs::AppModel::Script>> m_supportScripts;

    long createLoadingAnimation();

    HoloJs::IConsoleConfiguration* m_consoleConfiguration;

    std::unique_ptr<HoloJs::AppModel::ScriptsLoader> m_scriptsLoader;

    std::shared_ptr<IHoloJsView> m_view;
    std::shared_ptr<HoloJs::ScriptContext> m_activeContext;

    std::shared_ptr<BackgroundExecutionQueue> m_backgroundExecutionQueue;

    void* m_targetViewWindow;

    void* m_viewIcon;
    std::wstring m_viewTitle;

    long releaseExecutionContext();

    enum class HostState { NotInitialized, Initialized };

    void startExecution(const std::wstring appUrl);
    void startExecution(void* platformHandle);

    HostState m_runningState = HostState::NotInitialized;
};
}  // namespace HoloJs
