#pragma once

#include "holojs/private/canvas-render-context-2d.h"
#include "holojs/private/holojs-view.h"
#include "holojs/private/image-interface.h"
#include "holojs/private/platform-interfaces.h"
#include "holojs/windows/windows-platform.h"
namespace HoloJs {
namespace UWP {

class UWPPlatform : public HoloJs::Platforms::WindowsPlatform {
   public:
    virtual IHoloJsView* makeView(HoloJs::ViewConfiguration viewConfig);

    virtual HRESULT readResourceScript(const wchar_t* name, std::wstring& scriptText);

    virtual HoloJs::IXmlHttpRequest* createXmlHttpRequest(HoloJs::IHoloJsScriptHostInternal* host);

    virtual HoloJs::ICanvasRenderContext2D* createCanvasRenderContext2D();

    virtual HoloJs::IIMage* createImage(HoloJs::IHoloJsScriptHostInternal* host);

    virtual void enableDebugger(HoloJs::IHoloJsScriptHost* host, JsRuntimeHandle runtime);
};
}  // namespace UWP
}  // namespace HoloJs

namespace HoloJs {
namespace UWP {
public
enum class ViewMode { None = 0, Default = 1, Flat = 2, FlatEmbedded = 2, VR = 3 };

public ref class ViewConfiguration sealed
{
public:
	void setViewMode(HoloJs::UWP::ViewMode viewMode) { m_viewMode = viewMode; }

	void enableVoiceCommands() { m_enableVoiceCommands = true; }
	void disableVoiceCommands() { m_enableVoiceCommands = false; }
	
	void enableQrCodeNavigation() { m_enableQrCodeNavigation = true; }
	void disableQrCodeNavigation() { m_enableQrCodeNavigation = false; }

internal:
	HoloJs::UWP::ViewMode m_viewMode;
	bool m_enableVoiceCommands;
	bool m_enableQrCodeNavigation;
};

public
ref class HoloJsScriptHost sealed {
   public:
    HoloJsScriptHost() { m_scriptHost.reset(HoloJs::PrivateInterface::CreateHoloJsScriptHost()); }
    bool initialize(HoloJs::UWP::ViewConfiguration^ viewConfig)
    {
		HoloJs::ViewConfiguration nativeConfiguration;
		nativeConfiguration.enableQrCodeNavigation = viewConfig->m_enableQrCodeNavigation;
		nativeConfiguration.enableVoiceCommands = viewConfig->m_enableVoiceCommands;
		nativeConfiguration.viewMode = static_cast<HoloJs::ViewMode>(viewConfig->m_viewMode);
        return SUCCEEDED(m_scriptHost->initialize(nativeConfiguration));
    }
    void enableDebugger() { m_scriptHost->enableDebugger(); }
    bool startUri(Platform::String ^ uri) { return SUCCEEDED(m_scriptHost->startUri(uri->Data())); }
    bool startWithEmptyApp() { return SUCCEEDED(m_scriptHost->startWithEmptyApp()); }

   private:
    std::unique_ptr<HoloJs::IHoloJsScriptHost> m_scriptHost;
};
}  // namespace UWP
}  // namespace HoloJs
