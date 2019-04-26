#pragma once

#include "../include/holojs/private/chakra.h"
#include "../resource-management/resource-manager.h"
#include <memory>

namespace HoloJs {

namespace WebAudio {

class PannerNodeProjection {
   public:
    PannerNodeProjection(std::shared_ptr<HoloJs::ResourceManagement::ResourceManager> resourceManager)
        : m_resourceManager(resourceManager)
    {
    }

    HRESULT initialize();

   private:
    JS_PROJECTION_DEFINE(PannerNodeProjection, setPosition)
    JS_PROJECTION_DEFINE(PannerNodeProjection, setOrientation)
    JS_PROJECTION_DEFINE(PannerNodeProjection, setVelocity)

    JS_PROJECTION_DEFINE(PannerNodeProjection, setRefDistance)
    JS_PROJECTION_DEFINE(PannerNodeProjection, getRefDistance)

    JS_PROJECTION_DEFINE(PannerNodeProjection, getPanningModel)
    JS_PROJECTION_DEFINE(PannerNodeProjection, setPanningModel)

    JS_PROJECTION_DEFINE(PannerNodeProjection, getDistanceModel)
    JS_PROJECTION_DEFINE(PannerNodeProjection, setDistanceModel)

    JS_PROJECTION_DEFINE(PannerNodeProjection, positionX)
    JS_PROJECTION_DEFINE(PannerNodeProjection, positionY)
    JS_PROJECTION_DEFINE(PannerNodeProjection, positionZ)

    JS_PROJECTION_DEFINE(PannerNodeProjection, orientationX)
    JS_PROJECTION_DEFINE(PannerNodeProjection, orientationY)
    JS_PROJECTION_DEFINE(PannerNodeProjection, orientationZ)

    JS_PROJECTION_DEFINE(PannerNodeProjection, getMaxDistance)
    JS_PROJECTION_DEFINE(PannerNodeProjection, setMaxDistance)

    JS_PROJECTION_DEFINE(PannerNodeProjection, getRolloffFactor)
    JS_PROJECTION_DEFINE(PannerNodeProjection, setRolloffFactor)

    JS_PROJECTION_DEFINE(PannerNodeProjection, getConeInnerAngle)
    JS_PROJECTION_DEFINE(PannerNodeProjection, setConeInnerAngle)

    JS_PROJECTION_DEFINE(PannerNodeProjection, getConeOuterAngle)
    JS_PROJECTION_DEFINE(PannerNodeProjection, setConeOuterAngle)

    JS_PROJECTION_DEFINE(PannerNodeProjection, getConeOuterGain)
    JS_PROJECTION_DEFINE(PannerNodeProjection, setConeOuterGain)

   private:
    std::shared_ptr<HoloJs::ResourceManagement::ResourceManager> m_resourceManager;
};
}  // namespace WebAudio
}  // namespace HoloJs
