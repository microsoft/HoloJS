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

    static JsValueRef m_setBinaryTypeFunction;
    static JsValueRef CHAKRA_CALLBACK setBinaryType(_In_ JsValueRef callee,
                                                    _In_ bool isConstructCall,
                                                    _In_ JsValueRef *arguments,
                                                    _In_ unsigned short argumentCount,
                                                    _In_ PVOID callbackData);

    std::wstring m_url;
    std::wstring m_protocols;
    std::wstring m_binaryType;

    bool IsArrayBuffer() { return _wcsicmp(m_binaryType.c_str(), L"arraybuffer") == 0; }

    concurrency::task<void> ConnectAsync();

    concurrency::task<void> SendAsync(JsValueRef dataRef);

    Windows::Networking::Sockets::MessageWebSocket ^ m_webSocket;

    void FireOnError();

    void FireOnMessage(Windows::Networking::Sockets::MessageWebSocketMessageReceivedEventArgs ^ msgArgs);

    void FireOnOpen();

    void FireOnClose(Windows::Networking::Sockets::WebSocketClosedEventArgs ^ closedArgs);
};
}  // namespace API
}  // namespace HologramJS
