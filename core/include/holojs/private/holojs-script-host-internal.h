#pragma once

#include "../holojs-script-host.h"
#include "app-model.h"
#include "holojs-view.h"
#include "object-lifetime.h"
#include "script-window.h"
#include <memory>
namespace HoloJs {

enum class HostRenderedElements { None, LoadingAnimation, LoadingFailed, NothingLoaded, QrScanGuide };

class IHoloJsScriptHostInternal : public IHoloJsScriptHost {
   public:
    virtual HoloJs::IWindow* getWindowElement() = 0;

    virtual long createScriptObject(HoloJs::ResourceManagement::IRelease* nativeObject,
                                    HoloJs::ResourceManagement::ObjectType type,
                                    JsValueRef* scriptObjet) = 0;

    virtual long getActiveAppConfiguration(HoloJs::AppModel::AppConfiguration* configuration) = 0;

    virtual long runInBackground(std::function<long()> backgroundWork) = 0;
    virtual long runInScriptContext(std::function<void()> contextWork) = 0;

    virtual long getStationaryCoordinateSystem(void** coordinateSystem) = 0;

    virtual long createExecutionContext() = 0;
    virtual long executeLoadedApp(std::shared_ptr<HoloJs::AppModel::HoloJsApp> app) = 0;

	virtual long showHostRenderedElement(HoloJs::HostRenderedElements element) = 0;
};

}  // namespace HoloJs