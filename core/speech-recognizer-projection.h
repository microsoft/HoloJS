#pragma once

#include "include/holojs/private/chakra.h"
#include "resource-management/resource-manager.h"
#include <memory>

namespace HoloJs {

class IHoloJsScriptHostInternal;

class SpeechRecognizerProjection {
   public:
    SpeechRecognizerProjection(std::shared_ptr<HoloJs::ResourceManagement::ResourceManager> resourceManager,
                             HoloJs::IHoloJsScriptHostInternal* host)
        : m_resourceManager(resourceManager), m_host(host)
    {
    }

    ~SpeechRecognizerProjection() {}

    void enable() { m_isEnabled = true; }
    void disable() { m_isEnabled = false; }

    long initialize();

   private:
    JS_PROJECTION_DEFINE(SpeechRecognizerProjection, isAvailable)
    JS_PROJECTION_DEFINE(SpeechRecognizerProjection, create)
    JS_PROJECTION_DEFINE(SpeechRecognizerProjection, start)
    JS_PROJECTION_DEFINE(SpeechRecognizerProjection, stop)
    JS_PROJECTION_DEFINE(SpeechRecognizerProjection, setKeywords)
    JS_PROJECTION_DEFINE(SpeechRecognizerProjection, enableFreeFormDictation)

    std::shared_ptr<HoloJs::ResourceManagement::ResourceManager> m_resourceManager;
    HoloJs::IHoloJsScriptHostInternal* m_host;

    bool m_isEnabled = false;
};

}  // namespace HoloJs