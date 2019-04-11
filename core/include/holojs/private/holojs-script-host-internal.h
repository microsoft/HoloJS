#pragma once

#include "../holojs-script-host.h"
#include "holojs-view.h"
#include "script-window.h"
#include "object-lifetime.h"
#include "app-model.h"
#include <memory>
namespace HoloJs {

class IHoloJsScriptHostInternal : public IHoloJsScriptHost {
   public:
    virtual HoloJs::IWindow* getWindowElement() = 0;

    virtual long createScriptObject(HoloJs::ResourceManagement::IRelease* nativeObject,
                                    HoloJs::ResourceManagement::ObjectType type,
                                    JsValueRef* scriptObjet) = 0;

    virtual long getActiveAppConfiguration(HoloJs::AppModel::AppConfiguration* configuration) = 0;

    virtual void runInBackground(std::function<long()> backgroundWork) = 0;
    virtual void runInScriptContext(std::function<void()> contextWork) = 0;

    virtual void backgroundWorkItemComplete(BackgroundWorkItem* workItem) = 0;
    virtual void contextWorkItemComplete(ScriptContextWorkItem* workItem) = 0;

	virtual long getStationaryCoordinateSystem(void** coordinateSystem) = 0;
};

}  // namespace HoloJs