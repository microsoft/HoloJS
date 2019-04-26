#include "stdafx.h"
#include "holojs/holojs.h"
#include "holojs/private/error-handling.h"
#include "holojs/windows/image-element.h"
#include "holojs/windows/render-context-2d.h"
#include "holojs/windows/xml-http-request.h"
#include "script-resources.h"
#include "win32-embedded-view.h"
#include "win32-platform.h"
#include "win32-view.h"
#include <map>
#include <ppltasks.h>
#include <string>

static HoloJs::Win32::Win32Platform Win32HoloJsPlatform;

namespace HoloJs {
HoloJs::IPlatform* __cdecl HoloJs::getPlatform() { return &Win32HoloJsPlatform; }
}  // namespace HoloJs

using namespace HoloJs::Win32;
using namespace Windows::Data::Json;
using namespace Windows::Web::Http;
using namespace Windows::Web;
using namespace std;
using namespace concurrency;

__declspec(dllexport) HoloJs::IHoloJsScriptHost* __cdecl HoloJs::CreateHoloJsScriptHost()
{
    return HoloJs::PrivateInterface::CreateHoloJsScriptHost();
}
__declspec(dllexport) void __cdecl HoloJs::DeleteHoloJsScriptHost(HoloJs::IHoloJsScriptHost* scriptHost)
{
    return HoloJs::PrivateInterface::DeleteHoloJsScriptHost(scriptHost);
}

map<std::wstring, int> g_scriptNamesMap = {{L"URL.js", URL_SCRIPT},
                                           {L"console.js", CONSOLE_SCRIPT},
                                           {L"timers.js", TIMERS_SCRIPT},
                                           {L"2d-context.js", CONTEXT_2D_SCRIPT},
                                           {L"image.js", IMAGE_SCRIPT},
                                           {L"canvas.js", CANVAS_SCRIPT},
                                           {L"canvas-vr.js", CANVASVR_SCRIPT},
                                           {L"document.js", DOCUMENT_SCRIPT},
                                           {L"webgl-context.js", WEBGL_CONTEXT_SCRIPT},
                                           {L"window.js", WINDOW_SCRIPT},
                                           {L"webvr.js", WEBVR_SCRIPT},
                                           {L"gamepad.js", GAMEPAD_SCRIPT},
                                           {L"xmlhttprequest.js", XHR_SCRIPT},
                                           {L"webaudio.js", WEBAUDIO_SCRIPT},
                                           {L"websocket.js", WEBSOCKET_SCRIPT},
                                           {L"surface-mapper.js", SURFACE_MAPPER_SCRIPT},
                                           {L"three.js", THREEJS_SCRIPT},
                                           {L"loading-animation.js", LOADING_ANIMATION_SCRIPT},
                                           {L"speech-recognizer.js", SPEECH_RECOGNIZER_SCRIPT},
                                           {L"spatial-anchors.js", SPATIAL_ANCHORS_SCRIPT}};

HoloJs::IHoloJsView* Win32Platform::makeView(HoloJs::ViewConfiguration viewConfig)
{
    if (viewConfig.viewMode == ViewMode::FlatEmbedded) {
        return new Win32HoloJsEmbeddedView(viewConfig);
    } else {
        return new Win32HoloJsView(viewConfig);
    }
}

HRESULT Win32Platform::readResourceScript(const wchar_t* name, std::wstring& scriptText)
{
    auto id = g_scriptNamesMap.find(name);
    if (id == g_scriptNamesMap.end()) {
        return E_FAIL;
    }

    HMODULE holoJs_module = NULL;

    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCWSTR)&Win32HoloJsPlatform,
                      &holoJs_module);

    auto script_resource = FindResource(holoJs_module, MAKEINTRESOURCEW(id->second), L"TEXT");
    auto script_resource_memory = LoadResource(holoJs_module, script_resource);
    auto script_size = SizeofResource(holoJs_module, script_resource);
    auto script_text_raw = LockResource(script_resource_memory);

    scriptText.assign(reinterpret_cast<wchar_t*>(script_text_raw), script_size / sizeof(wchar_t));

    return S_OK;
}

HoloJs::IXmlHttpRequest* Win32Platform::createXmlHttpRequest(HoloJs::IHoloJsScriptHostInternal* host)
{
    return new HoloJs::Win32::XmlHttpRequest(host);
}

HoloJs::ICanvasRenderContext2D* Win32Platform::createCanvasRenderContext2D()
{
    return new HoloJs::WindowsPlatform::RenderContext2D();
}

HoloJs::IIMage* Win32Platform::createImage(HoloJs::IHoloJsScriptHostInternal* host)
{
    return new HoloJs::Win32::Image(host);
}

void Win32Platform::enableDebugger(HoloJs::IHoloJsScriptHost* host, JsRuntimeHandle runtime)
{
    m_debugger.reset(new HoloJs::Win32::Debugging::ScriptDebugger());
    if (SUCCEEDED(m_debugger->start(runtime))) {
        m_debugger->waitForAttach();
    }
}