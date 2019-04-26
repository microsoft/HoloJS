#pragma once

#include "include/holojs/private/chakra.h"
#include "resource-management/resource-manager.h"
#include <memory>

namespace HoloJs {

class IHoloJsScriptHostInternal;

class SurfaceMappingProjection {
   public:
    SurfaceMappingProjection(std::shared_ptr<HoloJs::ResourceManagement::ResourceManager> resourceManager,
                             HoloJs::IHoloJsScriptHostInternal* host)
        : m_resourceManager(resourceManager), m_host(host)
    {
    }
    ~SurfaceMappingProjection() {}

    virtual void Release() {}

    long initialize();

   private:
    JS_PROJECTION_DEFINE(SurfaceMappingProjection, isAvailable)
    JS_PROJECTION_DEFINE(SurfaceMappingProjection, create)
    JS_PROJECTION_DEFINE(SurfaceMappingProjection, start)
    JS_PROJECTION_DEFINE(SurfaceMappingProjection, stop)
    JS_PROJECTION_DEFINE(SurfaceMappingProjection, triangleDensity)
    JS_PROJECTION_DEFINE(SurfaceMappingProjection, boundingCube)

    std::shared_ptr<HoloJs::ResourceManagement::ResourceManager> m_resourceManager;
    HoloJs::IHoloJsScriptHostInternal* m_host;
};

}  // namespace HoloJs