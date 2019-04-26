#include "stdafx.h"
#include "holojs/private/error-handling.h"
#include "holojs/private/holojs-script-host-internal.h"
#include "holojs/private/script-host-utilities.h"
#include "include/holojs/windows/IBufferOnMemory.h"
#include "websocket.h"
#include <codecvt>
#include <ppltasks.h>

using namespace HoloJs::Platforms::Win32;
using namespace std;
using namespace std::placeholders;
using namespace Windows::Networking::Sockets;
using namespace concurrency;

void WebSocket::Release()
{
    bool waitForClose = false;

    {
        std::lock_guard<std::mutex> guard(m_asyncWorkLock);

        if (m_readyState == ReadyState::OPEN) {
            m_readyState = ReadyState::CLOSING;
            m_webSocket->Close(1000, Platform::StringReference(L"closing"));
            waitForClose = true;
        }
    }

    // Wait for close
    while (waitForClose && m_readyState == ReadyState::CLOSING) Sleep(0);

    // Wait until onClose exits
    std::lock_guard<std::mutex> guard(m_asyncWorkLock);
}

void WebSocket::close(int code, const std::wstring& reason)
{
    std::lock_guard<std::mutex> guard(m_asyncWorkLock);

    if (m_readyState == ReadyState::OPEN) {
        m_readyState = ReadyState::CLOSING;
        m_webSocket->Close(1000, Platform::StringReference(L"closing"));
    }
}

void WebSocket::connect(const wstring& url, const vector<wstring>& protocols)
{
    m_webSocket = ref new MessageWebSocket();
	m_webSocket->Control->ReceiveMode = MessageWebSocketReceiveMode::FullMessage;

    m_webSocket->Closed += ref new Windows::Foundation::TypedEventHandler<Windows::Networking::Sockets::IWebSocket ^,
                                                                          WebSocketClosedEventArgs ^>(
        std::bind(&WebSocket::onClosed, this, _1, _2));

    m_webSocket->MessageReceived += ref new Windows::Foundation::TypedEventHandler<
        Windows::Networking::Sockets::MessageWebSocket ^
        , Windows::Networking::Sockets::MessageWebSocketMessageReceivedEventArgs ^>(
        std::bind(&WebSocket::onMessageReceived, this, _1, _2));

    for (const auto& protocol : protocols) {
        m_webSocket->Control->SupportedProtocols->Append(Platform::StringReference(protocol.c_str()).GetString());
    }

    m_url = url;
    for (const auto& protocol : protocols) {
        m_protocols.append(protocol);
    }

    m_readyState = ReadyState::CONNECTING;
    m_host->runInBackground([this]() -> long {
        auto uri = ref new Windows::Foundation::Uri(Platform::StringReference(m_url.c_str()));

        try {
            create_task(m_webSocket->ConnectAsync(uri)).wait();
            m_readyState = ReadyState::OPEN;
            m_host->runInScriptContext(std::bind(&WebSocket::handleConnectSuccess, this));
        } catch (...) {
            m_readyState = ReadyState::CLOSED;
            m_host->runInScriptContext(std::bind(&WebSocket::handleConnectFail, this));
        }
        return HoloJs::Success;
    });
}

void WebSocket::send(const std::wstring& message)
{
    EXIT_IF_FALSE(m_readyState == ReadyState::OPEN);

    wstring messageCopy = message;

    m_host->runInBackground([this, messageCopy]() -> long {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
        auto utfString = myconv.to_bytes(messageCopy);

        vector<unsigned char> utf8Data;
        utf8Data.resize(utfString.length());
        memcpy(utf8Data.data(), utfString.c_str(), utfString.length());
        RETURN_IF_TRUE(utf8Data.size() > UINT32_MAX);

        Microsoft::WRL::ComPtr<HoloJs::BufferOnMemory> bufferOnMemory;
        Microsoft::WRL::Details::MakeAndInitialize<BufferOnMemory>(
            &bufferOnMemory, utf8Data.data(), static_cast<unsigned int>(utf8Data.size()));
        auto iinspectable = (IInspectable*)reinterpret_cast<IInspectable*>(bufferOnMemory.Get());
        auto sendBuffer = reinterpret_cast<Windows::Storage::Streams::IBuffer ^>(iinspectable);

        m_webSocket->Control->MessageType = SocketMessageType::Utf8;

        try {
            create_task(m_webSocket->OutputStream->WriteAsync(sendBuffer)).wait();
            create_task(m_webSocket->OutputStream->FlushAsync()).wait();
        } catch (...) {
            m_host->runInScriptContext([this]() { close(1000, L"send error"); });
        }

        return S_OK;
    });
}

void WebSocket::send(unsigned char* data, unsigned int dataSize, JsValueRef dataRef)
{
    EXIT_IF_FALSE(m_readyState == ReadyState::OPEN);

    m_host->runInBackground([this, data, dataSize, dataRef]() -> long {
        Microsoft::WRL::ComPtr<HoloJs::BufferOnMemory> bufferOnMemory;
        Microsoft::WRL::Details::MakeAndInitialize<BufferOnMemory>(&bufferOnMemory, data, dataSize);
        auto iinspectable = (IInspectable*)reinterpret_cast<IInspectable*>(bufferOnMemory.Get());
        auto sendBuffer = reinterpret_cast<Windows::Storage::Streams::IBuffer ^>(iinspectable);

        m_webSocket->Control->MessageType = SocketMessageType::Binary;

        try {
            create_task(m_webSocket->OutputStream->WriteAsync(sendBuffer)).wait();
            create_task(m_webSocket->OutputStream->FlushAsync()).wait();
            m_host->runInScriptContext([this, dataRef]() { JsRelease(dataRef, nullptr); });
        } catch (...) {
            m_host->runInScriptContext([this, dataRef]() {
                JsRelease(dataRef, nullptr);
                close(1000, L"send error");
            });
        }

        return S_OK;
    });
}

void WebSocket::send(HoloJs::IBlob* blob, JsValueRef dataRef)
{
    EXIT_IF_FALSE(m_readyState == ReadyState::OPEN);

    m_host->runInBackground([this, blob, dataRef]() -> long {
        Microsoft::WRL::ComPtr<HoloJs::BufferOnMemory> bufferOnMemory;

        RETURN_IF_TRUE(blob->data()->size() > UINT32_MAX);

        Microsoft::WRL::Details::MakeAndInitialize<BufferOnMemory>(
            &bufferOnMemory, blob->data()->data(), static_cast<unsigned int>(blob->data()->size()));
        auto iinspectable = (IInspectable*)reinterpret_cast<IInspectable*>(bufferOnMemory.Get());
        auto sendBuffer = reinterpret_cast<Windows::Storage::Streams::IBuffer ^>(iinspectable);

        m_webSocket->Control->MessageType = SocketMessageType::Binary;

        try {
            create_task(m_webSocket->OutputStream->WriteAsync(sendBuffer)).wait();
            create_task(m_webSocket->OutputStream->FlushAsync()).wait();
            m_host->runInScriptContext([this, dataRef]() { JsRelease(dataRef, nullptr); });
        } catch (...) {
            m_host->runInScriptContext([this, dataRef]() {
                JsRelease(dataRef, nullptr);
                close(1000, L"send error");
            });
        }

        return S_OK;
    });
}

void WebSocket::onClosed(Windows::Networking::Sockets::IWebSocket ^ sender, WebSocketClosedEventArgs ^ args)
{
    std::lock_guard<std::mutex> guard(m_asyncWorkLock);
    m_readyState = ReadyState::CLOSED;

    m_host->runInScriptContext([this]() -> void { this->invokeEventListeners(L"close"); });
}

void WebSocket::onMessageReceived(Windows::Networking::Sockets::MessageWebSocket ^ sender,
                                  Windows::Networking::Sockets::MessageWebSocketMessageReceivedEventArgs ^ args)
{
    std::lock_guard<std::mutex> guard(m_asyncWorkLock);

    if (m_readyState == ReadyState::CLOSING) {
        return;
    }

    auto dataReader = args->GetDataReader();

    if (args->MessageType == SocketMessageType::Utf8) {
        auto message = dataReader->ReadString(dataReader->UnconsumedBufferLength);
        m_host->runInScriptContext([this, message]() -> void {
            JsValueRef messageRef;
            EXIT_IF_JS_ERROR(JsPointerToString(message->Data(), message->Length(), &messageRef));
            invokeEventListeners(L"message", messageRef);
        });
    } else {
        auto messageBuffer = dataReader->ReadBuffer(dataReader->UnconsumedBufferLength);
        m_host->runInScriptContext([this, messageBuffer]() -> void {
            JsValueRef messageRef;
            Microsoft::WRL::ComPtr<Windows::Storage::Streams::IBufferByteAccess> messageByteAccess;
            EXIT_IF_FAILED(
                reinterpret_cast<IInspectable*>(messageBuffer)->QueryInterface(IID_PPV_ARGS(&messageByteAccess)));

            byte* messageRawData;
            EXIT_IF_FAILED(messageByteAccess->Buffer(&messageRawData));

            EXIT_IF_FAILED(HoloJs::ScriptHostUtilities::CreateArrayBufferFromBuffer(
                &messageRef, messageRawData, messageBuffer->Length));

            invokeEventListeners(L"message", messageRef);
        });
    }
}

void WebSocket::handleConnectFail()
{
    invokeEventListeners(L"error");
    invokeEventListeners(L"close");
}

void WebSocket::handleConnectSuccess() { invokeEventListeners(L"open"); }
