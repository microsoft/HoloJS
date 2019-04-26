#pragma once

#include "include/holojs/private/chakra.h"
#include "include/holojs/private/spatial-anchors.h"
#include "resource-management/resource-manager.h"
#include <memory>

namespace HoloJs {

class IHoloJsScriptHostInternal;

class SpatialAnchorsProjection {
   public:
    SpatialAnchorsProjection(std::shared_ptr<HoloJs::ResourceManagement::ResourceManager> resourceManager,
                             HoloJs::IHoloJsScriptHostInternal* host)
        : m_resourceManager(resourceManager), m_host(host)
    {
    }

    ~SpatialAnchorsProjection() {}

    long initialize();

   private:
    JS_PROJECTION_DEFINE(SpatialAnchorsProjection, areAvailable)
    JS_PROJECTION_DEFINE(SpatialAnchorsProjection, canPersist)
    JS_PROJECTION_DEFINE(SpatialAnchorsProjection, createStore)
    JS_PROJECTION_DEFINE(SpatialAnchorsProjection, initializeStore)
    JS_PROJECTION_DEFINE(SpatialAnchorsProjection, createAnchor)
    JS_PROJECTION_DEFINE(SpatialAnchorsProjection, openAnchor)
    JS_PROJECTION_DEFINE(SpatialAnchorsProjection, saveAnchor)
    JS_PROJECTION_DEFINE(SpatialAnchorsProjection, deleteAnchor)
    JS_PROJECTION_DEFINE(SpatialAnchorsProjection, enumerateAnchors)
    JS_PROJECTION_DEFINE(SpatialAnchorsProjection, tryGetTransformTo)

    std::shared_ptr<HoloJs::ResourceManagement::ResourceManager> m_resourceManager;
    HoloJs::IHoloJsScriptHostInternal* m_host;
};

}  // namespace HoloJs