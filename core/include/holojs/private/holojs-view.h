#pragma once

#include "app-model.h"
#include "script-host-errors.h"
#include <functional>
#include <memory>

namespace HoloJs {

class IHoloJsScriptHostInternal;

class BackgroundWorkItem {
   public:
    BackgroundWorkItem(IHoloJsScriptHostInternal* host, std::shared_ptr<std::function<long()>> workItem)
        : context(host), lambda(workItem)
    {
    }
    IHoloJsScriptHostInternal* context;
    std::shared_ptr<std::function<long()>> lambda;
};

class ScriptContextWorkItem {
   public:
    ScriptContextWorkItem(IHoloJsScriptHostInternal* host, std::shared_ptr<std::function<void()>> workItem)
        : context(host), lambda(workItem)
    {
    }
    IHoloJsScriptHostInternal* context;
    std::shared_ptr<std::function<void()>> lambda;
};

class IHoloJsView {
   public:
    virtual ~IHoloJsView() {}

    virtual long initialize(IHoloJsScriptHostInternal* host) = 0;
    virtual void setViewWindow(void* nativeWindow) = 0;
    virtual void run() = 0;
    virtual void stop() = 0;

    virtual long executeApp(std::shared_ptr<HoloJs::AppModel::HoloJsApp> app) = 0;
    virtual long executeScript(const wchar_t* script) = 0;

    virtual void executeOnViewThread(ScriptContextWorkItem* workItem) = 0;
    virtual void executeInBackground(BackgroundWorkItem* workItem) = 0;

    virtual void onError(HoloJs::ScriptHostErrorType errorType) = 0;

    virtual bool isWindingOrderReversed() = 0;

    virtual void setIcon(void* platformIcon) = 0;

    virtual void setTitle(const std::wstring& title) = 0;
};

}  // namespace HoloJs