#include "pch.h"

// The main function is only used to initialize our IFrameworkView class.
[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
    auto scriptHost = ref new HoloJs::UWP::HoloJsScriptHost();
    auto viewConfiguration = ref new HoloJs::UWP::ViewConfiguration();
    if (scriptHost->initialize(viewConfiguration)) {
        auto uri = ref new Platform::String(L"http://holojs.azurewebsites.net/v7/ballshooter.xrs");
        scriptHost->startUri(uri);
    }
    return 0;
}