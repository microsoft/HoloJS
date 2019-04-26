#include "pch.h"
#include "App.h"
#include <string>

// The main function is only used to start the script host
[Platform::MTAThread] int main(Platform::Array<Platform::String ^> ^) {
    auto scriptHost = ref new HoloJs::UWP::HoloJsScriptHost();
    auto viewConfiguration = ref new HoloJs::UWP::ViewConfiguration();

    std::wstring deviceFamily = Windows::System::Profile::AnalyticsInfo::VersionInfo->DeviceFamily->Data();
    //if (_wcsicmp(deviceFamily.c_str(), L"Windows.Holographic") == 0) {
        viewConfiguration->enableVoiceCommands();
        viewConfiguration->enableQrCodeNavigation();
    //}

    viewConfiguration->setViewMode(HoloJs::UWP::ViewMode::Default);
    if (scriptHost->initialize(viewConfiguration)) {
        auto uri = ref new Platform::String(L"e:\\src\\first-app\first-app.json");
        scriptHost->startUri(uri);
        //scriptHost->startWithEmptyApp();
    }

    return 0;
}