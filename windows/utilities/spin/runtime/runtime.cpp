// runtime.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "holojs\holojs.h"
#include "resource.h"
#include <string>

using namespace std;

void ShowNoAppUrlMessage() {
	MessageBox(nullptr,
                   L"The URL of the app to run is missing. Please use the \"--uri \" switch.",
                   L"Missing app URL",
                   MB_OK);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd)
{
    int argc;
    auto argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (argc < 3) {
        ShowNoAppUrlMessage();
        exit(1);
    }

    wstring appName = L"";
    bool isDebug = false;
    for (int i = 0; i < argc; i++) {
        if ((_wcsicmp(argv[i], L"-u") == 0 || _wcsicmp(argv[i], L"--uri") == 0) && i < (argc - 1)) {
            appName = argv[i + 1];
        } else if (_wcsicmp(argv[i], L"-d") == 0 || _wcsicmp(argv[i], L"--debug") == 0) {
            isDebug = true;
        }
    }

    if (appName.empty()) {
		ShowNoAppUrlMessage();
        exit(1);
    }

    std::shared_ptr<HoloJs::IHoloJsScriptHost> scriptHost(HoloJs::CreateHoloJsScriptHost(),
                                                          &HoloJs::DeleteHoloJsScriptHost);

	if (isDebug) {
        scriptHost->enableDebugger();
    }

    auto icon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    scriptHost->setWindowIcon(icon);
    scriptHost->setWindowTitle(L"Spin");
	auto configuration = HoloJs::ViewConfiguration();
    scriptHost->initialize(configuration);

    scriptHost->startUri(appName.c_str());
    return 0;
}