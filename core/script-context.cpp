#include "script-context.h"
#include "holojs-script-host.h"
#include "include/holojs/private/error-handling.h"
#include "include/holojs/private/platform-interfaces.h"
#include "include/holojs/private/script-host-utilities.h"
#include "window-element.h"

using namespace HoloJs;
using namespace std;

long ScriptContext::destroy()
{
    while (m_asyncWorkerCount > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }

    m_console.reset();
    m_domEventRegistration.reset();
    m_promiseContinuation.reset();
    m_webglProjections.reset();
    m_xmlHttpRequestProjections.reset();
    m_imageProjections.reset();
    m_canvas2dProjections.reset();
    m_audioContextProjection.reset();
    m_blobProjection.reset();
    m_surfaceMappingProjection.reset();
    m_speechRecognizerProjection.reset();
    m_spatialAnchorsProjection.reset();
    m_view.reset();

    if (m_nativeInterfaceRef != JS_INVALID_REFERENCE) {
        JsRelease(m_nativeInterfaceRef, nullptr);
        m_nativeInterfaceRef = JS_INVALID_REFERENCE;
    }

    if (m_jsContext != nullptr) {
        JsSetCurrentContext(nullptr);
        m_jsContext = nullptr;
    }

    if (m_jsRuntime != nullptr) {
        JsDisposeRuntime(m_jsRuntime);
        m_jsRuntime = nullptr;
    }

    m_windowElement = nullptr;

    m_resourceManager.reset();

    return HoloJs::Success;
}

long ScriptContext::initialize()
{
    RETURN_IF_JS_ERROR(JsCreateRuntime(JsRuntimeAttributeNone, nullptr, &m_jsRuntime));
    RETURN_IF_JS_ERROR(JsCreateContext(m_jsRuntime, &m_jsContext));
    RETURN_IF_JS_ERROR(JsSetCurrentContext(m_jsContext));

    RETURN_IF_FAILED(ScriptHostUtilities::GetNativeInterfaceJsProperty(&m_nativeInterfaceRef));
    RETURN_IF_JS_ERROR(JsAddRef(m_nativeInterfaceRef, nullptr));

    if (m_debugEnabled) {
        getPlatform()->enableDebugger(static_cast<HoloJs::IHoloJsScriptHost*>(m_host), m_jsRuntime);
    }

    m_resourceManager = make_shared<ResourceManagement::ResourceManager>();

    m_windowElement = new HoloJs::Interfaces::WindowElement(m_host);
    RETURN_IF_FAILED(m_windowElement->initialize());

    m_console = make_unique<HoloJs::Interfaces::Console>();
    RETURN_IF_FAILED(m_console->initialize());

    m_domEventRegistration = make_unique<HoloJs::Interfaces::DOMEventRegistration>();
    RETURN_IF_FAILED(m_domEventRegistration->initialize());

    m_promiseContinuation = make_unique<HoloJs::Interfaces::PromiseContinuation>();
    RETURN_IF_FAILED(m_promiseContinuation->initialize());

    m_webglProjections = make_unique<HoloJs::WebGL::WebGLProjections>(m_resourceManager);
    RETURN_IF_FAILED(m_webglProjections->initialize());

    m_xmlHttpRequestProjections = make_unique<HoloJs::Interfaces::XmlHttpRequestProjection>(m_resourceManager, m_host);
    RETURN_IF_FAILED(m_xmlHttpRequestProjections->initialize());

    m_imageProjections = make_unique<HoloJs::Interfaces::ImageProjections>(m_resourceManager, m_host);
    RETURN_IF_FAILED(m_imageProjections->initialize());

    m_canvas2dProjections = make_unique<HoloJs::CanvasProjections>(m_resourceManager);
    RETURN_IF_FAILED(m_canvas2dProjections->initialize());

    m_audioContextProjection = make_unique<HoloJs::WebAudio::AudioContextProjection>(m_resourceManager, m_host);
    RETURN_IF_FAILED(m_audioContextProjection->initialize());

    m_blobProjection = make_unique<HoloJs::Interfaces::BlobProjection>(m_resourceManager);
    RETURN_IF_FAILED(m_blobProjection->initialize());

    m_websocketProjection = make_unique<HoloJs::WebSocketProjection>(m_resourceManager, m_host);
    RETURN_IF_FAILED(m_websocketProjection->initialize());

    m_surfaceMappingProjection = make_unique<HoloJs::SurfaceMappingProjection>(m_resourceManager, m_host);
    RETURN_IF_FAILED(m_surfaceMappingProjection->initialize());

    m_speechRecognizerProjection = make_unique<HoloJs::SpeechRecognizerProjection>(m_resourceManager, m_host);
    RETURN_IF_FAILED(m_speechRecognizerProjection->initialize());

    if (m_host->getViewConfiguration().enableVoiceCommands) {
        m_speechRecognizerProjection->enable();
    }

    m_spatialAnchorsProjection = make_unique<HoloJs::SpatialAnchorsProjection>(m_resourceManager, m_host);
    RETURN_IF_FAILED(m_spatialAnchorsProjection->initialize());

    return HoloJs::Success;
}

long ScriptContext::execute(const wchar_t* script, const wchar_t* name)
{
    JsValueRef result;
    HANDLE_EXCEPTION_IF_JS_ERROR(JsRunScript(script, m_jsSourceContext++, name == nullptr ? L"inline" : name, &result));

    return HoloJs::Success;
}