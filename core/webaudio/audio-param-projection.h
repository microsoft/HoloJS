#pragma once

#include "../include/holojs/private/chakra.h"
#include "../resource-management/resource-manager.h"
#include <memory>

namespace HoloJs {

namespace WebAudio {

class AudioParamProjection {
   public:
    AudioParamProjection(std::shared_ptr<HoloJs::ResourceManagement::ResourceManager> resourceManager)
        : m_resourceManager(resourceManager)
    {
    }

    HRESULT initialize();

   private:
    JS_PROJECTION_DEFINE(AudioParamProjection, setValueAtTime)
    JS_PROJECTION_DEFINE(AudioParamProjection, setTargetAtTime)

    JS_PROJECTION_DEFINE(AudioParamProjection, getValue)
    JS_PROJECTION_DEFINE(AudioParamProjection, setValue)

    JS_PROJECTION_DEFINE(AudioParamProjection, getDefaultValue)
    JS_PROJECTION_DEFINE(AudioParamProjection, getMinValue)
    JS_PROJECTION_DEFINE(AudioParamProjection, getMaxValue)

    JS_PROJECTION_DEFINE(AudioParamProjection, linearRampToValueAtTime)
    JS_PROJECTION_DEFINE(AudioParamProjection, exponentialRampToValueAtTime)
    JS_PROJECTION_DEFINE(AudioParamProjection, setValueCurveAtTime)

    JS_PROJECTION_DEFINE(AudioParamProjection, cancelScheduledValues)

   private:
    std::shared_ptr<HoloJs::ResourceManagement::ResourceManager> m_resourceManager;
};
}  // namespace WebAudio
}  // namespace HoloJs
