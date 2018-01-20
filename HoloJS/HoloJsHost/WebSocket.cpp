#include "pch.h"
#include "WebSocket.h"
#include "ScriptHostUtilities.h"
#include "ScriptResourceTracker.h"

using namespace HologramJS::Utilities;
using namespace HologramJS::API;
using namespace std;
using namespace concurrency;
using namespace Windows::Networking::Sockets;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::Storage::Streams;
using namespace Microsoft::WRL;

JsValueRef WebSocket::m_createFunction = JS_INVALID_REFERENCE;
JsValueRef WebSocket::m_closeFunction = JS_INVALID_REFERENCE;
JsValueRef WebSocket::m_sendFunction = JS_INVALID_REFERENCE;
JsValueRef WebSocket::m_setBinaryTypeFunction = JS_INVALID_REFERENCE;

WebSocket::WebSocket()
{
    m_webSocket = ref new MessageWebSocket();

    m_webSocket->Closed += ref new TypedEventHandler<IWebSocket ^, WebSocketClosedEventArgs ^>(
        [this](IWebSocket ^ socket, WebSocketClosedEventArgs ^ closedArgs) {
            Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
                CoreDispatcherPriority::Normal,
                ref new DispatchedHandler([this, closedArgs] { FireOnClose(closedArgs); }));
        });

    m_webSocket->MessageReceived +=
        ref new TypedEventHandler<MessageWebSocket ^, MessageWebSocketMessageReceivedEventArgs ^>(
            [this](MessageWebSocket ^ socket, MessageWebSocketMessageReceivedEventArgs ^ msgArgs) {
                Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
                    CoreDispatcherPriority::Normal,
                    ref new DispatchedHandler([this, msgArgs] { FireOnMessage(msgArgs); }));
            });
}

bool WebSocket::Initialize()
{
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"create", L"websocket", create, nullptr, &m_createFunction));
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"send", L"websocket", send, nullptr, &m_sendFunction));
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(
        L"setBinaryType", L"websocket", setBinaryType, nullptr, &m_setBinaryTypeFunction));
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"close", L"websocket", close, nullptr, &m_closeFunction));

    return true;
}

_Use_decl_annotations_ JsValueRef CHAKRA_CALLBACK WebSocket::create(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount > 1);

    wstring url;
    RETURN_INVALID_REF_IF_FALSE(ScriptHostUtilities::GetString(arguments[1], url));

    wstring protocols;
    if (argumentCount > 2) {
        RETURN_INVALID_REF_IF_FALSE(ScriptHostUtilities::GetString(arguments[1], protocols));
    }

    auto newWS = new WebSocket();
    newWS->m_url = url;
    newWS->m_protocols = protocols;

    ExternalObject* externalObject = new ExternalObject();
    RETURN_INVALID_REF_IF_FALSE(externalObject->Initialize(newWS));

    newWS->ConnectAsync();

    return ScriptResourceTracker::ObjectToDirectExternal(externalObject);
}

_Use_decl_annotations_ JsValueRef CHAKRA_CALLBACK WebSocket::close(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount > 1);
    auto ws = ScriptResourceTracker::ExternalToObject<WebSocket>(arguments[1]);
    RETURN_INVALID_REF_IF_NULL(ws);

    bool hasCode = argumentCount > 2;
    bool hasReason = argumentCount > 3;

    int code;
    if (hasCode) {
        code = ScriptHostUtilities::GLintFromJsRef(arguments[2]);
    }

    wstring reason;
    if (hasReason) {
        RETURN_INVALID_REF_IF_FALSE(ScriptHostUtilities::GetString(arguments[3], reason));
    }

    if (hasCode) {
        ws->m_webSocket->Close(code, Platform::StringReference(reason.c_str()).GetString());
    } else {
        // Socket will be closed on WS going out of scope
    }

    return JS_INVALID_REFERENCE;
}

_Use_decl_annotations_ JsValueRef CHAKRA_CALLBACK WebSocket::setBinaryType(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);
    auto ws = ScriptResourceTracker::ExternalToObject<WebSocket>(arguments[1]);
    RETURN_INVALID_REF_IF_NULL(ws);

    wstring binaryType;
    RETURN_INVALID_REF_IF_FALSE(ScriptHostUtilities::GetString(arguments[2], binaryType));

    if (_wcsicmp(binaryType.c_str(), L"arraybuffer") == 0) {
        ws->m_binaryType = binaryType;
    } else {
        ws->m_binaryType = L"";
    }

    return JS_INVALID_REFERENCE;
}

_Use_decl_annotations_ JsValueRef CHAKRA_CALLBACK WebSocket::send(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    auto ws = ScriptResourceTracker::ExternalToObject<WebSocket>(arguments[1]);
    RETURN_INVALID_REF_IF_NULL(ws);

    RETURN_INVALID_REF_IF_JS_ERROR(JsAddRef(arguments[2], nullptr));

    ws->SendAsync(arguments[2]);

    return JS_INVALID_REFERENCE;
}

task<void> WebSocket::SendAsync(JsValueRef dataRef)
{
    vector<byte> utf8Data;
    byte* rawSendBuffer;
    unsigned int rawSendBufferLength;

    JsValueType dataType;
    EXIT_IF_JS_ERROR(JsGetValueType(dataRef, &dataType));

    if (dataType == JsString) {
        PCWSTR stringPointer;
        EXIT_IF_JS_ERROR(JsStringToPointer(dataRef, &stringPointer, &rawSendBufferLength));

        // Convert text to utf8
        wstring unicodeValue(stringPointer);
        std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
        auto utfString = myconv.to_bytes(unicodeValue);

        // Copy utf8 string to a buffer
        utf8Data.resize(utfString.length());
        memcpy(utf8Data.data(), utfString.c_str(), utfString.length());

        // capture a pointer to the utf8 buffer
        rawSendBuffer = utf8Data.data();
        m_webSocket->Control->MessageType = SocketMessageType::Utf8;
    } else if (dataType == JsArrayBuffer) {
        EXIT_IF_JS_ERROR(JsGetArrayBufferStorage(dataRef, &rawSendBuffer, &rawSendBufferLength));
        m_webSocket->Control->MessageType = SocketMessageType::Binary;
    }

    ComPtr<HologramJS::Utilities::BufferOnMemory> bufferOnMemory;
    Details::MakeAndInitialize<BufferOnMemory>(&bufferOnMemory, rawSendBuffer, rawSendBufferLength);
    auto iinspectable = (IInspectable*)reinterpret_cast<IInspectable*>(bufferOnMemory.Get());
    auto sendBuffer = reinterpret_cast<IBuffer ^>(iinspectable);

    auto result = await m_webSocket->OutputStream->WriteAsync(sendBuffer);
    m_webSocket->OutputStream->FlushAsync();

    EXIT_IF_JS_ERROR(JsRelease(dataRef, nullptr));
}

task<void> WebSocket::ConnectAsync()
{
    auto uri = ref new Windows::Foundation::Uri(Platform::StringReference(m_url.c_str()));

    if (!m_protocols.empty()) {
        m_webSocket->Control->SupportedProtocols->Append(Platform::StringReference(m_protocols.c_str()).GetString());
    }

    m_webSocket->Control->ReceiveMode = MessageWebSocketReceiveMode::FullMessage;

    try {
        await m_webSocket->ConnectAsync(uri);
    } catch (...) {
        FireOnError();
    }

    FireOnOpen();
}

void WebSocket::FireOnError()
{
    if (HasCallback()) {
        vector<JsValueRef> parameters(2);

        parameters[0] = m_scriptCallbackContext;

        JsValueRef* typeParam = &parameters[1];
        EXIT_IF_JS_ERROR(JsPointerToString(L"error", wcslen(L"error"), typeParam));

        JsValueRef result;
        HANDLE_EXCEPTION_IF_JS_ERROR(JsCallFunction(
            m_scriptCallback, parameters.data(), static_cast<unsigned short>(parameters.size()), &result));
    }
}

void WebSocket::FireOnMessage(MessageWebSocketMessageReceivedEventArgs ^ msgArgs)
{
    auto dataReader = msgArgs->GetDataReader();

    if (!HasCallback()) {
        if (msgArgs->MessageType == SocketMessageType::Utf8) {
            dataReader->ReadString(dataReader->UnconsumedBufferLength);
        } else {
            dataReader->ReadBuffer(dataReader->UnconsumedBufferLength);
        }
    } else {
        JsValueRef parameters[3];

        if (msgArgs->MessageType == SocketMessageType::Utf8) {
            auto message = dataReader->ReadString(dataReader->UnconsumedBufferLength);
            EXIT_IF_JS_ERROR(JsPointerToString(message->Data(), message->Length(), &parameters[2]));
        } else {
            auto messageBuffer = dataReader->ReadBuffer(dataReader->UnconsumedBufferLength);
            Microsoft::WRL::ComPtr<Windows::Storage::Streams::IBufferByteAccess> messageByteAccess;
            EXIT_IF_FAILED(
                reinterpret_cast<IInspectable*>(messageBuffer)->QueryInterface(IID_PPV_ARGS(&messageByteAccess)));

            byte* messageRawData;
            EXIT_IF_FAILED(messageByteAccess->Buffer(&messageRawData));

            void* messageArrayBufferStorage;
            EXIT_IF_JS_ERROR(ScriptResourceTracker::CreateAndTrackExternalBuffer(
                messageBuffer->Length, &messageArrayBufferStorage, &parameters[2]));

            CopyMemory(messageArrayBufferStorage, messageRawData, messageBuffer->Length);
        }

        parameters[0] = m_scriptCallbackContext;
        EXIT_IF_JS_ERROR(JsPointerToString(L"message", wcslen(L"message"), &parameters[1]));

        JsValueRef result;
        HANDLE_EXCEPTION_IF_JS_ERROR(JsCallFunction(m_scriptCallback, parameters, ARRAYSIZE(parameters), &result));
    }
}

void WebSocket::FireOnOpen()
{
    if (HasCallback()) {
        vector<JsValueRef> parameters(2);

        parameters[0] = m_scriptCallbackContext;

        JsValueRef* typeParam = &parameters[1];
        EXIT_IF_JS_ERROR(JsPointerToString(L"open", wcslen(L"open"), typeParam));

        JsValueRef result;
        HANDLE_EXCEPTION_IF_JS_ERROR(JsCallFunction(
            m_scriptCallback, parameters.data(), static_cast<unsigned short>(parameters.size()), &result));
    }
}

void WebSocket::FireOnClose(WebSocketClosedEventArgs ^ closedArgs)
{
    if (HasCallback()) {
        JsValueRef parameters[4];
        parameters[0] = m_scriptCallbackContext;
        EXIT_IF_JS_ERROR(JsPointerToString(L"close", wcslen(L"close"), &parameters[1]));
        EXIT_IF_JS_ERROR(JsPointerToString(closedArgs->Reason->Data(), closedArgs->Reason->Length(), &parameters[2]));
        EXIT_IF_JS_ERROR(JsIntToNumber(closedArgs->Code, &parameters[3]));

        JsValueRef result;
        HANDLE_EXCEPTION_IF_JS_ERROR(JsCallFunction(m_scriptCallback, parameters, ARRAYSIZE(parameters), &result));
    }
}
