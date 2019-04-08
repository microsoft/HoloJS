#pragma once

#include "../include/holojs/private/chakra.h"
#include "../resource-management/resource-manager.h"
#include <memory>

namespace HoloJs {

namespace WebAudio {

class AudioListenerProjection {
   public:
    AudioListenerProjection(std::shared_ptr<HoloJs::ResourceManagement::ResourceManager> resourceManager)
        : m_resourceManager(resourceManager)
    {
    }

    HRESULT initialize();

   private:
    enum class AudioParamType {
        PositionX,
        PositionY,
        PositionZ,
        ForwardX,
        ForwardY,
        ForwardZ,
        UpX,
        UpY,
        UpZ,
    };

    JsValueRef getParamRefByType(JsValueRef listenerRef, AudioParamType type);

    JS_PROJECTION_DEFINE(AudioListenerProjection, positionX)
    JS_PROJECTION_DEFINE(AudioListenerProjection, positionY)
    JS_PROJECTION_DEFINE(AudioListenerProjection, positionZ)

    JS_PROJECTION_DEFINE(AudioListenerProjection, forwardX)
    JS_PROJECTION_DEFINE(AudioListenerProjection, forwardY)
    JS_PROJECTION_DEFINE(AudioListenerProjection, forwardZ)

    JS_PROJECTION_DEFINE(AudioListenerProjection, upX)
    JS_PROJECTION_DEFINE(AudioListenerProjection, upY)
    JS_PROJECTION_DEFINE(AudioListenerProjection, upZ)

    JS_PROJECTION_DEFINE(AudioListenerProjection, setPosition)
    JS_PROJECTION_DEFINE(AudioListenerProjection, setOrientation)

   private:
    std::shared_ptr<HoloJs::ResourceManagement::ResourceManager> m_resourceManager;
};
}  // namespace WebAudio
}  // namespace HoloJs
