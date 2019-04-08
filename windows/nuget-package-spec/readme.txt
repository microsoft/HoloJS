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

-------------------------------------------------------------------------------

The XRS file in the URI is a JSON file that contains the list of scripts to execute:
{ "scripts": [ "three.js", "app.js" ] }

For a sample ThreeJS app, see http://holojs.azurewebsites.net/v7/appcode/vr-cubes.js
