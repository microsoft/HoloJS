#include "include/holojs/private/script-host-utilities.h"
#include "include/holojs/private/xml-http-request.h"
#include "holojs-script-host.h"
#include "host-interfaces.h"
#include "include/holojs/private/error-handling.h"
#include "include/holojs/private/platform-interfaces.h"
#include "resource-management/resource-manager.h"
#include "xml-http-request-projections.h"
#include <functional>
#include <string>

using namespace HoloJs::Interfaces;
using namespace HoloJs::ResourceManagement;
using namespace std;

HRESULT XmlHttpRequestProjection::initialize()
{
    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"create", L"XmlHttpRequest", staticCreateXmlHttpRequest, this, &m_createXmlHttpRequestFunction));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"send", L"XmlHttpRequest", staticSendXmlHttpRequest, this, &m_sendXmlHttpRequestFunction));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"getResponseHeader", L"XmlHttpRequest", staticGetResponseHeader, this, &m_getResponseHeaderFunction));
    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"setRequestHeader", L"XmlHttpRequest", staticSetRequestHeader, this, &m_setRequestHeaderFunction));

    return S_OK;
}

JsValueRef XmlHttpRequestProjection::sendXmlHttpRequest(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 7);

    auto xhr = m_resourceManager->externalToObject<HoloJs::IXmlHttpRequest>(arguments[1], ObjectType::IXmlHttpRequest);
    RETURN_INVALID_REF_IF_NULL(xhr);

    JsValueRef xhrReference = arguments[2];

    wstring method;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetString(arguments[3], method));

    wstring uri;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetString(arguments[4], uri));

    wstring type;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetString(arguments[5], type));

    JsValueRef payload = arguments[6];
    RETURN_INVALID_REF_IF_FAILED(xhr->createHttpContent(payload));

    RETURN_INVALID_REF_IF_FAILED(xhr->send(method, uri, type, xhrReference));

    return JS_INVALID_REFERENCE;
}

JsValueRef XmlHttpRequestProjection::createXmlHttpRequest(JsValueRef* arguments, unsigned short argumentCount)
{
    return m_resourceManager->objectToDirectExternal(
        getPlatform()->createXmlHttpRequest(m_host), ObjectType::IXmlHttpRequest);
}

JsValueRef XmlHttpRequestProjection::getResponseHeader(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);
    auto xhr = m_resourceManager->externalToObject<HoloJs::IXmlHttpRequest>(arguments[1], ObjectType::IXmlHttpRequest);
    RETURN_INVALID_REF_IF_NULL(xhr);

    wstring header;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetString(arguments[2], header));

    return xhr->getResponseHeader(header);
}

JsValueRef XmlHttpRequestProjection::setRequestHeader(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);
    auto xhr = m_resourceManager->externalToObject<HoloJs::IXmlHttpRequest>(arguments[1], ObjectType::IXmlHttpRequest);
    RETURN_INVALID_REF_IF_NULL(xhr);

    wstring header;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetString(arguments[2], header));

    wstring value;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetString(arguments[3], value));

    xhr->setRequestHeader(header, value);

    return JS_INVALID_REFERENCE;
}