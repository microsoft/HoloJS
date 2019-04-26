#include "websocket-projection.h"
#include "include/holojs/private/error-handling.h"
#include "include/holojs/private/platform-interfaces.h"
#include "include/holojs/private/script-host-utilities.h"
#include "include/holojs/private/websocket.h"
#include "resource-management/external-object.h"
#include "blob.h"

#include <string>
#include <vector>

using namespace HoloJs;
using namespace HoloJs::ResourceManagement;
using namespace std;

long WebSocketProjection::initialize()
{
    JS_PROJECTION_REGISTER(L"WebSocket", L"create", create);
    JS_PROJECTION_REGISTER(L"WebSocket", L"readyState", readyState);
	JS_PROJECTION_REGISTER(L"WebSocket", L"close", close);
	JS_PROJECTION_REGISTER(L"WebSocket", L"send", send);

    return HoloJs::Success;
}

JsValueRef WebSocketProjection::_create(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3 || argumentCount == 4);

    wstring url;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetString(arguments[2], url));

    // Get the protocol(s), if any
    vector<wstring> protocols;
    if (argumentCount > 2) {
        JsValueType protocolArgType;
        JsValueRef protocolRef = arguments[3];
        RETURN_INVALID_REF_IF_JS_ERROR(JsGetValueType(protocolRef, &protocolArgType));

        if (protocolArgType != JsUndefined) {
            RETURN_INVALID_REF_IF_FALSE(protocolArgType == JsString || protocolArgType == JsArray);

            if (protocolArgType == JsString) {
                wstring singleProtocol;
                RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetString(protocolRef, singleProtocol));
                protocols.push_back(singleProtocol);
            } else {
                // Get the length of the protocols array
                JsValueRef protocolCountRef;
                RETURN_INVALID_REF_IF_FAILED(
                    ScriptHostUtilities::GetJsProperty(protocolRef, L"length", &protocolCountRef));
                auto protocolCount = ScriptHostUtilities::GLintFromJsRef(protocolCountRef);

                for (int i = 0; i < protocolCount; i++) {
                    JsValueRef indexRef;
                    RETURN_INVALID_REF_IF_JS_ERROR(JsIntToNumber(i, &indexRef));

                    JsValueRef protocolEntryRef;
                    RETURN_INVALID_REF_IF_JS_ERROR(JsGetIndexedProperty(protocolRef, indexRef, &protocolEntryRef));

                    JsValueType protocolEntryType;
                    RETURN_INVALID_REF_IF_JS_ERROR(JsGetValueType(protocolEntryRef, &protocolEntryType));

                    RETURN_INVALID_REF_IF_FALSE(protocolEntryType == JsString);

                    wstring singleProtocol;
                    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetString(protocolEntryRef, singleProtocol));
                    protocols.push_back(singleProtocol);
                }
            }
        }
    }

    auto socketExternal = m_resourceManager->objectToDirectExternal(
        getPlatform()->createWebSocket(m_host, url, protocols), ObjectType::IWebSocket);

    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::SetJsProperty(arguments[1], L"native", socketExternal));

    return JS_INVALID_REFERENCE;
}

JsValueRef WebSocketProjection::_readyState(JsValueRef* arguments, unsigned short argumentCount)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    auto websocket = m_resourceManager->externalToObject<IWebSocket>(arguments[1], ObjectType::IWebSocket);
    RETURN_INVALID_REF_IF_NULL(websocket);

    auto state = websocket->getReadyState();

    JsValueRef readyStateRef;
    RETURN_INVALID_REF_IF_JS_ERROR(JsIntToNumber(state, &readyStateRef));

    return readyStateRef;
}

JsValueRef WebSocketProjection::_close(JsValueRef* arguments, unsigned short argumentCount)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount >= 2);

    auto websocket = m_resourceManager->externalToObject<IWebSocket>(arguments[1], ObjectType::IWebSocket);
    RETURN_INVALID_REF_IF_NULL(websocket);

	int code = 1000;
	if (argumentCount > 2) {
		code = ScriptHostUtilities::GLintFromJsRef(arguments[2]);
	}

	wstring reason = L"close";
	if (argumentCount > 3) {
		RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetString(arguments[3], reason));
	}

	websocket->close(code, reason);

    return JS_INVALID_REFERENCE;
}

JsValueRef WebSocketProjection::_send(JsValueRef* arguments, unsigned short argumentCount)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    auto websocket = m_resourceManager->externalToObject<IWebSocket>(arguments[1], ObjectType::IWebSocket);
    RETURN_INVALID_REF_IF_NULL(websocket);

	JsValueRef dataRef = arguments[2];
	JsValueType dataType;
	RETURN_INVALID_REF_IF_JS_ERROR(JsGetValueType(dataRef, &dataType));

	if (dataType == JsString) {
		wstring textMessage;
		RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetString(dataRef, textMessage));
		websocket->send(textMessage);
	} else if (dataType == JsArrayBuffer) {
		unsigned char* rawSendBuffer;
		unsigned int rawSendBufferLength;
        RETURN_INVALID_REF_IF_JS_ERROR(JsGetArrayBufferStorage(dataRef, &rawSendBuffer, &rawSendBufferLength));
		RETURN_INVALID_REF_IF_JS_ERROR(JsAddRef(dataRef, nullptr));
		websocket->send(rawSendBuffer, rawSendBufferLength, dataRef);
	}
	else {
		// Try blob
		JsValueRef blobNativeRef;
		RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetJsProperty(dataRef, L"native", &blobNativeRef));

		auto blob = m_resourceManager->externalToObject<HoloJs::Interfaces::Blob>(blobNativeRef, ObjectType::Blob);
		RETURN_INVALID_REF_IF_FALSE(blob != nullptr);

		RETURN_INVALID_REF_IF_JS_ERROR(JsAddRef(dataRef, nullptr));
		websocket->send(blob, dataRef);
	}

    return JS_INVALID_REFERENCE;
}