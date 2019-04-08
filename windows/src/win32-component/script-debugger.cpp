#include "stdafx.h"
#include "script-debugger.h"

using namespace std;
using namespace HoloJs::Win32::Debugging;

HRESULT ScriptDebugger::start(JsRuntimeHandle runtime) {
	JsErrorCode result = JsNoError;
    auto protocolHandler = std::make_unique<DebugProtocolHandler>(runtime);
    auto service = std::make_unique<DebugService>();

    result = service->RegisterHandler("holojs_runtime", *protocolHandler, true /*break on next line*/);

    if (result == JsNoError)
    {
        result = service->Listen(9229);
    }

    if (result == JsNoError)
    {
        m_protocolHandler = std::move(protocolHandler);
        m_debugService = std::move(service);

		return S_OK;
    }
	else
	{
		return E_FAIL;
	}
}
HRESULT ScriptDebugger::waitForAttach() {
	return m_protocolHandler->WaitForDebugger() == JsNoError ? S_OK : E_FAIL;
}