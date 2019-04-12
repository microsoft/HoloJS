#include "pch.h"
#include "App.h"

// The main function is only used to start the script host
[Platform::MTAThread] int main(Platform::Array<Platform::String ^> ^) {
    auto scriptHost = ref new HoloJs::UWP::HoloJsScriptHost();
	auto viewConfiguration = ref new HoloJs::UWP::ViewConfiguration();
	viewConfiguration->enableQrCodeNavigation();
	viewConfiguration->enableVoiceCommands();
	viewConfiguration->setViewMode(HoloJs::UWP::ViewMode::Default);
    if (scriptHost->initialize(viewConfiguration)) {
#ifdef _DEBUG
        scriptHost->enableDebugger();
#endif
        auto uri = ref new Platform::String(L"http://holojs.azurewebsites.net/v7/vr-cubes.xrs");
		scriptHost->startUri(uri);
        //scriptHost->startWithEmptyApp();
    }

    return 0;
}