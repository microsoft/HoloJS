#pragma once

#include "holojs/windows/windows-platform.h"
#include "holojs/private/image-interface.h"
#include "holojs/private/canvas-render-context-2d.h"
#include "holojs/private/holojs-view.h"
#include "holojs/private/platform-interfaces.h"
#include "script-debugger.h"

namespace HoloJs {
namespace Win32 {

class Win32Platform : public HoloJs::Platforms::WindowsPlatform {
   public:
    virtual IHoloJsView* makeView(HoloJs::ViewConfiguration viewConfig);

    virtual HRESULT readResourceScript(const wchar_t* name, std::wstring& scriptText);

    virtual HoloJs::IXmlHttpRequest* createXmlHttpRequest(HoloJs::IHoloJsScriptHostInternal* host);

    virtual HoloJs::ICanvasRenderContext2D* createCanvasRenderContext2D();

    virtual HoloJs::IIMage* createImage(HoloJs::IHoloJsScriptHostInternal* host);

	virtual void enableDebugger(HoloJs::IHoloJsScriptHost* host, JsRuntimeHandle runtime);

private:
	std::unique_ptr<HoloJs::Win32::Debugging::ScriptDebugger> m_debugger;
};
}  // namespace Win32
}  // namespace HoloJs
