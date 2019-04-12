#pragma once

namespace HoloJs {

class IConsoleConfiguration;

enum class ViewMode
{
	None = 0,
	Default,
	Flat,
	FlatEmbedded,
	VR
};

#pragma pack (push, 4)
struct ViewConfiguration
{
	ViewMode viewMode;
	bool enableVoiceCommands;
	bool enableQrCodeNavigation;
};
#pragma pack (pop)

class IHoloJsScriptHost {
   public:
    virtual ~IHoloJsScriptHost() {}
    virtual long initialize(ViewConfiguration viewConfig) = 0;
	virtual void setViewWindow(void* nativeWindow) = 0;

    virtual long startUri(const wchar_t* appUri) = 0;
    virtual long start(const wchar_t* script) = 0;
    virtual long startWithEmptyApp() = 0;

    virtual long execute(const wchar_t* script) = 0;
    virtual long executeUri(const wchar_t* scriptUri) = 0;
    virtual long executePackageFromHandle(void* platformPackageHandle) = 0;
    virtual long executeImmediate(const wchar_t* script, const wchar_t* scriptName) = 0;

    virtual long stopExecution() = 0;

    virtual void enableDebugger() = 0;

    virtual void setConsoleConfig(HoloJs::IConsoleConfiguration* consoleConfig) = 0;

    virtual void setWindowIcon(void* platformIcon) = 0;
    virtual void setWindowTitle(const wchar_t* title ) = 0;
};

__declspec(dllexport) HoloJs::IHoloJsScriptHost* __cdecl CreateHoloJsScriptHost();
__declspec(dllexport) void __cdecl DeleteHoloJsScriptHost(HoloJs::IHoloJsScriptHost* scriptHost);

}  // namespace HoloJs