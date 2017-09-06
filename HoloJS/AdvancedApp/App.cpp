#include "pch.h"
#include <WindowsNumerics.h>

using namespace Windows::ApplicationModel::Core;
using namespace Platform;
using namespace HologramJS;

// The main function creates an IFrameworkViewSource for our app, and runs the app.
[Platform::MTAThread]
int main(Array<Platform::String^>^)
{
    auto holoJsAppView= ref new HoloJsAppView(ref new String(L"scripts/app.json"));

    // use advanced mode rendering
    holoJsAppView->AutoStereoEnabled = false;

    auto holoJsAppSource = ref new HoloJsAppSource(holoJsAppView);
    CoreApplication::Run(holoJsAppSource);
    return 0;
}