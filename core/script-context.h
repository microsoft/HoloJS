#pragma once
#include "blob.h"
#include "canvas-2d-projections.h"
#include "dom-events.h"
#include "host-interfaces.h"
#include "image-element-projections.h"
#include "include/holojs/private/chakra.h"
#include "resource-management/resource-manager.h"
#include "spatial-anchors-projection.h"
#include "speech-recognizer-projection.h"
#include "surface-mapping-projection.h"
#include "webaudio/audio-context-projection.h"
#include "webgl/webgl-projections.h"
#include "websocket-projection.h"
#include "xml-http-request-projections.h"
#include <memory>

namespace HoloJs {

namespace Interfaces {
class WindowElement;
}

class HoloJsScriptHost;

class ScriptContext {
   public:
    ScriptContext() {}
    ~ScriptContext() { destroy(); }

    long initialize();

    long destroy();

    void setView(std::shared_ptr<IHoloJsView> view) { m_view = view; }
    void setHost(HoloJs::HoloJsScriptHost* host) { m_host = host; }

    void setConsoleConfig(HoloJs::IConsoleConfiguration* consoleConfig) { m_console->setConsoleConfig(consoleConfig); }
    HoloJs::Interfaces::WindowElement* getWindowElement() { return m_windowElement; }

    void enableDebug(bool enable) { m_debugEnabled = enable; }

    long execute(const wchar_t* script, const wchar_t* name);

    void reverseWebGlWindingOrder() { m_webglProjections->isContextFrontFaceSwitched = true; }

    std::shared_ptr<ResourceManagement::ResourceManager>& getResourceManager() { return m_resourceManager; }

    void asyncWorkerStarted() { m_asyncWorkerCount++; }
    void asyncWorkerExited() { m_asyncWorkerCount--; }

   private:
    std::shared_ptr<ResourceManagement::ResourceManager> m_resourceManager;

    std::unique_ptr<HoloJs::Interfaces::Console> m_console;
    std::unique_ptr<HoloJs::Interfaces::XmlHttpRequestProjection> m_xmlHttpRequestProjections;
    std::unique_ptr<HoloJs::Interfaces::ImageProjections> m_imageProjections;
    std::unique_ptr<HoloJs::WebGL::WebGLProjections> m_webglProjections;
    std::unique_ptr<HoloJs::Interfaces::DOMEventRegistration> m_domEventRegistration;
    std::unique_ptr<HoloJs::CanvasProjections> m_canvas2dProjections;
    std::unique_ptr<HoloJs::WebAudio::AudioContextProjection> m_audioContextProjection;
    std::unique_ptr<HoloJs::Interfaces::BlobProjection> m_blobProjection;
    std::unique_ptr<HoloJs::Interfaces::PromiseContinuation> m_promiseContinuation;
    std::unique_ptr<HoloJs::WebSocketProjection> m_websocketProjection;
    std::unique_ptr<HoloJs::SurfaceMappingProjection> m_surfaceMappingProjection;
    std::unique_ptr<HoloJs::SpeechRecognizerProjection> m_speechRecognizerProjection;
    std::unique_ptr<HoloJs::SpatialAnchorsProjection> m_spatialAnchorsProjection;

    // This pointer is tracked through script ref counting; It gets released automatically when the context is detroyed
    HoloJs::Interfaces::WindowElement* m_windowElement;

    std::shared_ptr<IHoloJsView> m_view;
    HoloJs::HoloJsScriptHost* m_host;

    bool m_debugEnabled = false;

    JsRuntimeHandle m_jsRuntime = nullptr;
    JsContextRef m_jsContext = nullptr;
    JsSourceContext m_jsSourceContext = 0;

    JsValueRef m_nativeInterfaceRef = JS_INVALID_REFERENCE;

    std::atomic<unsigned int> m_asyncWorkerCount = 0;
};

}  // namespace HoloJs