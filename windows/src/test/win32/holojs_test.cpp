// holojs_test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "holojs/holojs.h"
#include <Windows.h>
#include <iostream>
#include <memory>

using namespace HoloJs;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    std::shared_ptr<HoloJs::IHoloJsScriptHost> scriptHost(HoloJs::CreateHoloJsScriptHost(),
                                                          &HoloJs::DeleteHoloJsScriptHost);

	auto configuration = HoloJs::ViewConfiguration();
    scriptHost->initialize(configuration);

	//scriptHost->startUri(L"C:\\Users\\crispet\\Desktop\\qr-guide\\qr-guide.xrs");
    scriptHost->startWithEmptyApp();
    return 0;
}