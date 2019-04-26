#pragma once

#include "../include/holojs/private/chakra.h"
#include "../resource-management/resource-manager.h"
#include <memory>

namespace HoloJs {

namespace WebAudio {

class AudioBufferSourceProjection {
   public:
    AudioBufferSourceProjection(std::shared_ptr<HoloJs::ResourceManagement::ResourceManager> resourceManager)
        : m_resourceManager(resourceManager)
    {
    }

    HRESULT initialize();

   private:
    JS_PROJECTION_DEFINE(AudioBufferSourceProjection, setBuffer)
    JS_PROJECTION_DEFINE(AudioBufferSourceProjection, start)
    JS_PROJECTION_DEFINE(AudioBufferSourceProjection, stop)
    JS_PROJECTION_DEFINE(AudioBufferSourceProjection, getPlaybackRate)
    JS_PROJECTION_DEFINE(AudioBufferSourceProjection, getDetune)

    JS_PROJECTION_DEFINE(AudioBufferSourceProjection, getLoop)
    JS_PROJECTION_DEFINE(AudioBufferSourceProjection, setLoop)

    JS_PROJECTION_DEFINE(AudioBufferSourceProjection, getLoopStart)
    JS_PROJECTION_DEFINE(AudioBufferSourceProjection, setLoopStart)

    JS_PROJECTION_DEFINE(AudioBufferSourceProjection, getLoopEnd)
    JS_PROJECTION_DEFINE(AudioBufferSourceProjection, setLoopEnd)

   private:
    std::shared_ptr<HoloJs::ResourceManagement::ResourceManager> m_resourceManager;
};
}  // namespace WebAudio
}  // namespace HoloJs
