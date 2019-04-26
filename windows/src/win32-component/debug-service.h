#pragma once
#include "debug-protocol-handler.h"
#include <ChakraDebugProtocolHandler.h>
#include <ChakraDebugService.h>
#include <exception>
#include <string>

namespace HoloJs {
namespace Win32 {
namespace Debugging {
class DebugService {
   private:
    JsDebugService m_service{nullptr};

   public:
    DebugService()
    {
        JsDebugService service;

        JsErrorCode result = JsDebugServiceCreate(&service);

        if (result != JsNoError) {
            throw new std::exception("Unable to create debug service.");
        }

        m_service = service;
    }

    ~DebugService() { Destroy(); }

    JsErrorCode Close()
    {
        JsErrorCode result = JsDebugServiceClose(m_service);

        return result;
    }

    JsErrorCode Destroy()
    {
        JsErrorCode result = JsNoError;

        if (m_service != nullptr) {
            result = JsDebugServiceDestroy(m_service);

            if (result == JsNoError) {
                m_service = nullptr;
            }
        }

        return result;
    }

    JsErrorCode Listen(uint16_t port)
    {
        JsErrorCode result = JsDebugServiceListen(m_service, port);

        return result;
    }

    JsErrorCode RegisterHandler(std::string const& runtimeName,
                                DebugProtocolHandler& protocolHandler,
                                bool breakOnNextLine)
    {
        JsErrorCode result =
            JsDebugServiceRegisterHandler(m_service, runtimeName.c_str(), protocolHandler.GetHandle(), breakOnNextLine);

        return result;
    }

    JsErrorCode UnregisterHandler(std::string const& runtimeName)
    {
        JsErrorCode result = JsDebugServiceUnregisterHandler(m_service, runtimeName.c_str());

        return result;
    }
};

}  // namespace Debugging
}  // namespace Win32
}  // namespace HoloJs
