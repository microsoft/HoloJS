// win32-app.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <memory>
#include <Windows.h>
#include "holojs/holojs.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    std::shared_ptr<HoloJs::IHoloJsScriptHost> scriptHost(HoloJs::CreateHoloJsScriptHost(),
                                                          &HoloJs::DeleteHoloJsScriptHost);
    auto configuration = HoloJs::ViewConfiguration();
    scriptHost->initialize(configuration);
    scriptHost->startUri(L"http://holojs.azurewebsites.net/v7/vr-cubes.xrs");
    return 0;
}
