#pragma once

#include "include/holojs/private/chakra.h"
#include "resource-management/resource-manager.h"
#include <memory>

namespace HoloJs {

class IHoloJsScriptHostInternal;

class WebSocketProjection {
   public:
    WebSocketProjection(std::shared_ptr<HoloJs::ResourceManagement::ResourceManager> resourceManager,
                        HoloJs::IHoloJsScriptHostInternal* host)
        : m_resourceManager(resourceManager), m_host(host)
    {
    }
    ~WebSocketProjection() {}

    virtual void Release() {}

    long initialize();

   private:
    JS_PROJECTION_DEFINE(WebSocketProjection, create)
    JS_PROJECTION_DEFINE(WebSocketProjection, close)
    JS_PROJECTION_DEFINE(WebSocketProjection, send)
    JS_PROJECTION_DEFINE(WebSocketProjection, binaryType)
    JS_PROJECTION_DEFINE(WebSocketProjection, readyState)
    JS_PROJECTION_DEFINE(WebSocketProjection, bufferedAmount)

    std::shared_ptr<HoloJs::ResourceManagement::ResourceManager> m_resourceManager;
    HoloJs::IHoloJsScriptHostInternal* m_host;
};

}  // namespace HoloJs