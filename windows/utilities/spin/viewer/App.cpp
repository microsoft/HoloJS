#include "pch.h"

#include <ppltasks.h>

using namespace concurrency;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;

// The main function is only used to initialize our IFrameworkView class.
[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
	auto scriptHost = ref new HoloJs::UWP::HoloJsScriptHost();
    auto config = ref new HoloJs::UWP::ViewConfiguration();
    config->enableVoiceCommands();
    config->enableQrCodeNavigation();
    if (scriptHost->initialize(config)) {
        scriptHost->start();
    }

    return 0;
}