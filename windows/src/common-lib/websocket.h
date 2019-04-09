#pragma once

#include "holojs/private/websocket.h"
#include <string>
#include <vector>
#include <mutex>

namespace HoloJs {
namespace Platforms {
namespace Win32 {
class WebSocket : public HoloJs::IWebSocket, public HoloJs::EventTarget {
   public:
    WebSocket(HoloJs::IHoloJsScriptHostInternal* host) : m_host(host) {}
	virtual ~WebSocket() { Release(); }
	virtual void Release();

    virtual int getReadyState() { return static_cast<int>(m_readyState); }

    virtual void connect(const std::wstring& url, const std::vector<std::wstring>& protocols);
	virtual void send(const std::wstring& message);
	virtual void send(unsigned char* data, unsigned int dataSize, JsValueRef dataRef);
	virtual void send(HoloJs::IBlob* blob, JsValueRef dataRef);
	virtual void close(int code, const std::wstring& reason);

   private:
    HoloJs::IHoloJsScriptHostInternal* m_host;

    // Must match https://www.w3.org/TR/websockets/
    enum class ReadyState : unsigned short { CONNECTING = 0, OPEN = 1, CLOSING = 2, CLOSED = 3 };

    ReadyState m_readyState;

    std::wstring m_url;
    std::wstring m_protocols;
    std::wstring m_binaryType;

    Windows::Networking::Sockets::MessageWebSocket ^ m_webSocket;
    void onClosed(Windows::Networking::Sockets::IWebSocket ^ sender,
                  Windows::Networking::Sockets::WebSocketClosedEventArgs ^ args);
    void onMessageReceived(Windows::Networking::Sockets::MessageWebSocket ^ sender,
                                  Windows::Networking::Sockets::MessageWebSocketMessageReceivedEventArgs ^ args);

	void handleConnectFail();
	void handleConnectSuccess();

	std::mutex m_asyncWorkLock;
};

}  // namespace Win32
}  // namespace Platforms
}  // namespace HoloJs
