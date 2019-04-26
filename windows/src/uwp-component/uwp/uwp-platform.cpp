#include "stdafx.h"
#include "holojs/holojs.h"
#include "holojs/windows/render-context-2d.h"
#include "holojs/windows/xml-http-request.h"
#include "holojs/windows/image-element.h"
#include "holojs/private/error-handling.h"
#include "uwp-platform.h"
#include "winrt-view.h"
#include <map>
#include <ppltasks.h>
#include <string>

static HoloJs::UWP::UWPPlatform UWPHoloJsPlatform;

namespace HoloJs {
HoloJs::IPlatform* __cdecl HoloJs::getPlatform() { return &UWPHoloJsPlatform; }
}  // namespace HoloJs

using namespace HoloJs::UWP;
using namespace Windows::Data::Json;
using namespace Windows::Web::Http;
using namespace Windows::Web;
using namespace std;
using namespace concurrency;

HoloJs::IHoloJsView* UWPPlatform::makeView(HoloJs::ViewConfiguration viewConfig) { return new WinRTHoloJsView(viewConfig); }

void UWPPlatform::enableDebugger(HoloJs::IHoloJsScriptHost* host, JsRuntimeHandle runtime) {
	JsStartDebugging();
}

HRESULT UWPPlatform::readResourceScript(const wchar_t* name, std::wstring& scriptText)
{
    wstring path = L"HoloJs.UWP\\";
    path += name;

    return WindowsPlatform::readFileUnicode(path, scriptText);
}

HoloJs::IXmlHttpRequest* UWPPlatform::createXmlHttpRequest(HoloJs::IHoloJsScriptHostInternal* host)
{
    return new HoloJs::Win32::XmlHttpRequest(host);
}

HoloJs::ICanvasRenderContext2D* UWPPlatform::createCanvasRenderContext2D()
{
    return new HoloJs::WindowsPlatform::RenderContext2D();
}

HoloJs::IIMage* UWPPlatform::createImage(HoloJs::IHoloJsScriptHostInternal* host)
{
    return new HoloJs::Win32::Image(host);
}
