//
// This file demonstrates how to initialize EGL in a Windows Store app, using ICoreWindow.
//

#include "pch.h"
#include <WindowsNumerics.h>

using namespace Windows::ApplicationModel::Core;
using namespace Platform;
using namespace HologramJS;

// The main function creates an IFrameworkViewSource for our app, and runs the app.
[Platform::MTAThread]
int main(Array<Platform::String^>^)
{
    // If the app is launched without protocol invocation, run the bellow script that shows a canned message
    auto holoJsAppView = ref new HoloJsAppView(ref new String(L"http://holojs.azurewebsites.net/v4/samples/webarviewer/webarviewer.json"));

    // This app handles activations on the web-ar protocol; it is invoked by the web browser when a web-ar: link is navigated to
    // The app will execute any script navigated to on the web-ar protocol
    // NB: in order for this option to work, the app's manifest must declare the web-ar protocol
    holoJsAppView->EnableWebArProtocolHandler = true;

    holoJsAppView->ImageStabilizationEnabled = false;
    holoJsAppView->WorldOriginRelativePosition = Windows::Foundation::Numerics::float3(0, 0, -2);

    // Create the source from the app and run it
    auto holoJsAppSource = ref new HoloJsAppSource(holoJsAppView);
    CoreApplication::Run(holoJsAppSource);
    return 0;
}