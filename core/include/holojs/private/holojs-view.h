#pragma once

#include "app-model.h"
#include "script-host-errors.h"
#include <functional>
#include <memory>

namespace HoloJs {

class IHoloJsScriptHostInternal;

class IBackgroundWorkItem {
   public:
    virtual ~IBackgroundWorkItem() {}
    virtual long execute() = 0;
};

class IForegroundWorkItem {
   public:
    virtual ~IForegroundWorkItem(){}
    virtual void execute() = 0;
    virtual long long getTag() = 0;
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

    virtual long executeOnViewThread(IForegroundWorkItem* workItem) = 0;
    virtual long executeInBackground(IBackgroundWorkItem* workItem) = 0;

    virtual void onError(HoloJs::ScriptHostErrorType errorType) = 0;

    virtual bool isWindingOrderReversed() = 0;

    virtual void setIcon(void* platformIcon) = 0;

    virtual void setTitle(const std::wstring& title) = 0;

    virtual long getStationaryCoordinateSystem(void** coordinateSystem) = 0;
};

}  // namespace HoloJs