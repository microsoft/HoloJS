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

JsValueRef WebSocket::m_createFunction = JS_INVALID_REFERENCE;
JsValueRef WebSocket::m_closeFunction = JS_INVALID_REFERENCE;
JsValueRef WebSocket::m_sendFunction = JS_INVALID_REFERENCE;

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
            [this](MessageWebSocket^ socket, MessageWebSocketMessageReceivedEventArgs ^ msgArgs) {
		Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
			CoreDispatcherPriority::Normal,
			ref new DispatchedHandler([this, msgArgs] { FireOnMessage(msgArgs); }));
	});
}

bool WebSocket::Initialize()
{
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"create", L"websocket", create, nullptr, &m_createFunction));
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"send", L"websocket", send, nullptr, &m_sendFunction));
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

    return JS_INVALID_REFERENCE;
}

_Use_decl_annotations_ JsValueRef CHAKRA_CALLBACK WebSocket::send(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    auto ws = ScriptResourceTracker::ExternalToObject<WebSocket>(arguments[1]);
    RETURN_INVALID_REF_IF_NULL(ws);

    return JS_INVALID_REFERENCE;
}

task<void> WebSocket::ConnectAsync()
{
    auto uri = ref new Windows::Foundation::Uri(Platform::StringReference(m_url.c_str()));

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

void WebSocket::FireOnMessage(MessageWebSocketMessageReceivedEventArgs^ msgArgs)
{
    if (HasCallback()) {
		auto dataReader = msgArgs->GetDataReader();
		auto message = dataReader->ReadString(dataReader->UnconsumedBufferLength);

		JsValueRef parameters[3];
        parameters[0] = m_scriptCallbackContext;
        EXIT_IF_JS_ERROR(JsPointerToString(L"message", wcslen(L"message"), &parameters[1]));
		EXIT_IF_JS_ERROR(JsPointerToString(message->Data(), message->Length(), &parameters[2]));

        JsValueRef result;
        HANDLE_EXCEPTION_IF_JS_ERROR(JsCallFunction(
            m_scriptCallback, parameters, ARRAYSIZE(parameters), &result));
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
        HANDLE_EXCEPTION_IF_JS_ERROR(JsCallFunction(
            m_scriptCallback, parameters, ARRAYSIZE(parameters), &result));
    }
}
