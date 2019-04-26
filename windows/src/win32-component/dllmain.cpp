// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "holojs/holojs-script-host.h"


#pragma warning(disable : 4447)

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

extern "C" {

__declspec(dllexport) HoloJs::IHoloJsScriptHost* __cdecl holoJsScriptHost_create()
{
    return HoloJs::CreateHoloJsScriptHost();
}

__declspec(dllexport) void __cdecl holoJsScriptHost_delete(HoloJs::IHoloJsScriptHost* scriptHost)
{
    HoloJs::DeleteHoloJsScriptHost(scriptHost);
}

__declspec(dllexport) long __cdecl holoJsScriptHost_initialize(HoloJs::IHoloJsScriptHost* scriptHost, HoloJs::ViewConfiguration* viewConfig)
{
    return scriptHost->initialize(*viewConfig);
}

__declspec(dllexport) void __cdecl holoJsScriptHost_setViewWindow(HoloJs::IHoloJsScriptHost* scriptHost, void* window)
{
    scriptHost->setViewWindow(window);
}

__declspec(dllexport) long __cdecl holoJsScriptHost_startScript(HoloJs::IHoloJsScriptHost* scriptHost, const wchar_t* script)
{
    return scriptHost->start(script);
}

__declspec(dllexport) long __cdecl holoJsScriptHost_startUri(HoloJs::IHoloJsScriptHost* scriptHost,
                                                             const wchar_t* appUri)
{
    return scriptHost->startUri(appUri);
}

__declspec(dllexport) long __cdecl holoJsScriptHost_execute(HoloJs::IHoloJsScriptHost* scriptHost,
                                                            const wchar_t* script)
{
    return scriptHost->execute(script);
}

__declspec(dllexport) long __cdecl holoJsScriptHost_executeUri(HoloJs::IHoloJsScriptHost* scriptHost,
                                                               const wchar_t* scriptUri)
{
    return scriptHost->executeUri(scriptUri);
}

__declspec(dllexport) long __cdecl holoJsScriptHost_executeImmediate(HoloJs::IHoloJsScriptHost* scriptHost,
                                                                     const wchar_t* script)
{
    return scriptHost->executeImmediate(script, nullptr);
}

__declspec(dllexport) long __cdecl holoJsScriptHost_stopExecution(HoloJs::IHoloJsScriptHost* scriptHost)
{
    return scriptHost->stopExecution();
}

__declspec(dllexport) void __cdecl holoJsScriptHost_enableDebugger(HoloJs::IHoloJsScriptHost* scriptHost)
{
    scriptHost->enableDebugger();
}
}

