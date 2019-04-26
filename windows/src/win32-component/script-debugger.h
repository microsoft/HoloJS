#pragma once
#pragma once

#include <holojs/private/chakra.h>
#include <memory>
#include "debug-protocol-handler.h"
#include "debug-service.h"

namespace HoloJs {
namespace Win32 {
namespace Debugging {

class ScriptDebugger {
   public:
    ScriptDebugger() {}
    ~ScriptDebugger() {}

    HRESULT start(JsRuntimeHandle runtime);
	HRESULT waitForAttach();

   private:
    std::unique_ptr<HoloJs::Win32::Debugging::DebugProtocolHandler> m_protocolHandler;
    std::unique_ptr<HoloJs::Win32::Debugging::DebugService> m_debugService;
};
}  // namespace Debugging
}  // namespace Win32
}  // namespace HoloJs