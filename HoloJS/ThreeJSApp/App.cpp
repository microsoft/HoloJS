//
// This file demonstrates how to initialize EGL in a Windows Store app, using ICoreWindow.
//

#include "pch.h"
#include <WindowsNumerics.h>

using namespace Windows::ApplicationModel::Core;
using namespace Platform;
using namespace HologramJS;

void RunLocalSimple()
{
    // Run a basic HoloJS app
    auto holoJsAppSource = ref new HoloJsAppSource(ref new String(L"scripts/app.json"));
    CoreApplication::Run(holoJsAppSource);
}

void RunLocalWithOptions()
{
	// Run with some custom options for the app

	// 1. Create the app and set the options
	// If more customizations are required, copy the HoloJsAppView class in this project and make further changes as needed
	auto holoJsAppView = ref new HoloJsAppView(ref new String(L"scripts/app.json"));
	holoJsAppView->ImageStabilizationEnabled = false;
	holoJsAppView->LaunchMode = HoloJsLaunchMode::AsActivated;
	holoJsAppView->WorldOriginRelativePosition = Windows::Foundation::Numerics::float3(0, 0, -2);

	// 2. Create the source from the app and run it
	auto holoJsAppSource = ref new HoloJsAppSource(holoJsAppView);
	CoreApplication::Run(holoJsAppSource);
}

void RunWebWithOptions()
{
    // Run with some custom options for the app

    // 1. Create the app and set the options
    // If more customizations are required, copy the HoloJsAppView class in this project and make further changes as needed
    auto holoJsAppView = ref new HoloJsAppView(ref new String(L"http://holojs.azurewebsites.net/samples/photosphere/photosphere.json"));
    holoJsAppView->ImageStabilizationEnabled = false;
    holoJsAppView->WorldOriginRelativePosition = Windows::Foundation::Numerics::float3(0, 0, -2);

    // 2. Create the source from the app and run it
    auto holoJsAppSource = ref new HoloJsAppSource(holoJsAppView);
    CoreApplication::Run(holoJsAppSource);
}

// The main function creates an IFrameworkViewSource for our app, and runs the app.
[Platform::MTAThread]
int main(Array<Platform::String^>^)
{
    //RunLocalSimple();
    //RunWebWithOptions();
	RunLocalWithOptions();
	return 0;
}