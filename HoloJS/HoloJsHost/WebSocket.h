#pragma once

#include "ChakraForHoloJS.h"
#include "IBufferOnMemory.h"
#include "IRelease.h"
#include "ObjectEvents.h"

namespace HologramJS {
namespace API {
class WebSocket : public HologramJS::Utilities::ElementWithEvents, public HologramJS::Utilities::IRelease {
   public:
    WebSocket();
    ~WebSocket() {}

    virtual void Release() {}

    static bool Initialize();

   private:
    static JsValueRef m_createFunction;
    static JsValueRef CHAKRA_CALLBACK create(_In_ JsValueRef callee,
                                             _In_ bool isConstructCall,
                                             _In_ JsValueRef *arguments,
                                             _In_ unsigned short argumentCount,
                                             _In_ PVOID callbackData);

    static JsValueRef m_closeFunction;
    static JsValueRef CHAKRA_CALLBACK close(_In_ JsValueRef callee,
                                            _In_ bool isConstructCall,
                                            _In_ JsValueRef *arguments,
                                            _In_ unsigned short argumentCount,
                                            _In_ PVOID callbackData);

    static JsValueRef m_sendFunction;
    static JsValueRef CHAKRA_CALLBACK send(_In_ JsValueRef callee,
                                           _In_ bool isConstructCall,
                                           _In_ JsValueRef *arguments,
                                           _In_ unsigned short argumentCount,
                                           _In_ PVOID callbackData);

    std::wstring m_url;
    std::wstring m_protocols;

	concurrency::task<void> ConnectAsync();

    Windows::Networking::Sockets::MessageWebSocket ^ m_webSocket;

	void FireOnError();

	void FireOnMessage();

	void FireOnOpen();

	void FireOnClose();
};
}  // namespace API
}  // namespace HologramJS
