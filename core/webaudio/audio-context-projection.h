#pragma once

#include "../include/holojs/private/chakra.h"
#include "../include/holojs/private/holojs-view.h"
#include "audio-buffer-source-projection.h"
#include "audio-listener-projection.h"
#include "audio-node-projection.h"
#include "audio-param-projection.h"
#include "panner-node-projection.h"
#include <memory>

namespace HoloJs {

namespace ResourceManagement {
class ResourceManager;
}

namespace WebAudio {

class AudioContextProjection {
   public:
    AudioContextProjection(std::shared_ptr<HoloJs::ResourceManagement::ResourceManager> resourceManager,
                           HoloJs::IHoloJsScriptHostInternal* host)
        : m_resourceManager(resourceManager), m_host(host)
    {
        m_audioNodeProjection = std::make_unique<HoloJs::WebAudio::AudioNodeProjection>(resourceManager);
        m_pannerNodeProjection = std::make_unique<HoloJs::WebAudio::PannerNodeProjection>(resourceManager);
        m_audioListenerProjection = std::make_unique<HoloJs::WebAudio::AudioListenerProjection>(resourceManager);
        m_audioBufferSourceProjection =
            std::make_unique<HoloJs::WebAudio::AudioBufferSourceProjection>(resourceManager);
        m_audioParamProjection = std::make_unique<HoloJs::WebAudio::AudioParamProjection>(resourceManager);
    };
    HRESULT initialize();

   private:
    std::shared_ptr<HoloJs::ResourceManagement::ResourceManager> m_resourceManager;
    HoloJs::IHoloJsScriptHostInternal* m_host;

    JS_PROJECTION_DEFINE(AudioContextProjection, createContext)
    JS_PROJECTION_DEFINE(AudioContextProjection, createGain)
    JS_PROJECTION_DEFINE(AudioContextProjection, getDestination)
    JS_PROJECTION_DEFINE(AudioContextProjection, decodeAudioData)
    JS_PROJECTION_DEFINE(AudioContextProjection, createPanner)
    JS_PROJECTION_DEFINE(AudioContextProjection, getListener)
    JS_PROJECTION_DEFINE(AudioContextProjection, createBufferSource)

    JS_PROJECTION_DEFINE(AudioContextProjection, getSampleRate)
    JS_PROJECTION_DEFINE(AudioContextProjection, getCurrentTime)

    std::unique_ptr<HoloJs::WebAudio::AudioNodeProjection> m_audioNodeProjection;
    std::unique_ptr<HoloJs::WebAudio::PannerNodeProjection> m_pannerNodeProjection;
    std::unique_ptr<HoloJs::WebAudio::AudioListenerProjection> m_audioListenerProjection;
    std::unique_ptr<HoloJs::WebAudio::AudioBufferSourceProjection> m_audioBufferSourceProjection;
    std::unique_ptr<HoloJs::WebAudio::AudioParamProjection> m_audioParamProjection;
};
}  // namespace WebAudio
}  // namespace HoloJs
