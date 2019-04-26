---------------- HoloJS Readme -----------------

-------------------------------- Win32 embedding ------------------------------

To embed a script application in a C++ Win32 project, add the following code to the WinMain function:

#include "holojs/holojs.h"
#include <memory>
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    std::shared_ptr<HoloJs::IHoloJsScriptHost> scriptHost(HoloJs::CreateHoloJsScriptHost(), &HoloJs::DeleteHoloJsScriptHost);
    auto configuration = HoloJs::ViewConfiguration();
    scriptHost->initialize(configuration);
    scriptHost->startUri(L"http://holojs.azurewebsites.net/v7/vr-cubes.xrs");
    return 0;
}

See http://holojs.azurewebsites.net/help/win32-apps.html for more details

--------------------------------- UWP embedding -------------------------------

To embed a script application in a C++ UWP project add the following code to the main function:

[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
    auto scriptHost = ref new HoloJs::UWP::HoloJsScriptHost();
    auto viewConfiguration = ref new HoloJs::UWP::ViewConfiguration();
    if (scriptHost->initialize(viewConfiguration)) {
        auto uri = ref new Platform::String(L"http://holojs.azurewebsites.net/v7/vr-cubes.xrs");
        scriptHost->startUri(uri);
    }
    return 0;
}

See http://holojs.azurewebsites.net/help/uwp-apps.html for more details.

-------------------------------------------------------------------------------

See http://holojs.azurewebsites.net/help for more details about JavaScript VR experiences with HoloJs.

Changelog:
1.3.4: Make the UWP library Microsoft Store compliant by replacing non-compliant APIs with accepted alternatives.

1.3.5: Add a SpeechRecognizer JavaScript API for enabling voice command recognition in the guest script.
https://github.com/Microsoft/HoloJS/wiki/SpeechRecognizer

1.3.6: Fix unexpected closing of immersive view when running the HoloJs host in a .NET process.

1.3.7: Fix issues in SpeechRecognizer. Add free dictation mode to the SpeechRecognizer. Refer to the SpeechRecognizer wiki entry (see 1.3.5) for more information.

1.4.0: Implement spatial anchors for Windows Mixed Reality and HoloLens. Refer to the spatial anchors wiki entries for more information:
https://github.com/Microsoft/HoloJS/wiki/SpatialAnchor
https://github.com/Microsoft/HoloJS/wiki/SpatialAnchorStore