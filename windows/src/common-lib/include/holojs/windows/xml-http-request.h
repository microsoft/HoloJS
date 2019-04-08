#pragma once

#include "IBufferOnMemory.h"
#include "holojs/private/holojs-script-host-internal.h"
#include "holojs/private/chakra.h"
#include "holojs/private/event-target.h"
#include "holojs/private/holojs-view.h"
#include "holojs/private/object-lifetime.h"
#include "holojs/private/xml-http-request.h"

namespace HoloJs {

class HoloJsScriptHost;

namespace Win32 {

class XmlHttpRequest : public HoloJs::IXmlHttpRequest {
   public:
    XmlHttpRequest(HoloJs::IHoloJsScriptHostInternal* host) : m_host(host) {}

    virtual ~XmlHttpRequest();

    virtual void Release() {}

    HRESULT createHttpContent(JsValueRef payload);
    HRESULT send(const std::wstring& method, const std::wstring& uri, const std::wstring& type, JsValueRef xhrRef);

    void setRequestHeader(const std::wstring& name, const std::wstring& value)
    {
        m_requestHeaders.emplace_back(name, value);
    }

    JsValueRef getResponseHeader(const std::wstring& name);

    virtual long addEventListener(const std::wstring& eventName, JsValueRef handler, JsValueRef handlerContext)
    {
        return m_eventTargetImplementation.addEventListener(eventName, handler, handlerContext);
    }

    virtual long removeEventListener(const std::wstring& eventName, JsValueRef handler)
    {
        return m_eventTargetImplementation.removeEventListener(eventName, handler);
    }

    virtual long invokeEventListeners(const std::wstring& eventName)
    {
        return m_eventTargetImplementation.invokeEventListeners(eventName);
    }

    virtual long invokeEventListeners(const std::wstring& eventName, const std::vector<JsValueRef>& arguments)
    {
        return m_eventTargetImplementation.invokeEventListeners(eventName, arguments);
    }

   private:
    HoloJs::IHoloJsScriptHostInternal* m_host;

    HoloJs::EventTarget m_eventTargetImplementation;

    HRESULT sendSync(HoloJs::AppModel::AppConfiguration appConfig);
    HRESULT readFromFileSystemSync(HoloJs::AppModel::AppConfiguration appConfig);
    HRESULT readFromPackageSync(HoloJs::AppModel::AppConfiguration appConfig);

    // The payload that came from script; keep a reference to it for the duration of the request
    JsValueRef m_refScriptPayloadValue;
    Windows::Web::Http::IHttpContent ^ m_httpContent;

    enum class HttpContentType { NoContent, String, Buffer };
    HttpContentType m_contentType = HttpContentType::NoContent;

    Microsoft::WRL::ComPtr<HoloJs::BufferOnMemory> m_contentBuffer;
    Windows::Storage::Streams::IBuffer ^ m_contentIBuffer;

    std::wstring m_url = L"";
    std::wstring m_method = L"GET";
    std::wstring m_responseType = L"";

    void setHeaders(Windows::Web::Http::HttpRequestMessage ^ requestMessage);
    bool isContentHeader(const std::wstring& headerName);
    std::vector<std::pair<std::wstring, std::wstring>> m_requestHeaders;
    Windows::Web::Http::Headers::HttpContentHeaderCollection ^ m_responseHeaders;

    enum class RequestState { DONE = 4, OPENED = 1, HEADERS_RECEIVED = 2 };
    RequestState m_state;

    int m_status;
    std::wstring m_statusText = L"";

    bool isTextResponse() { return (m_responseType.empty() || (_wcsicmp(m_responseType.c_str(), L"text") == 0)); }
    std::wstring m_responseText;
    Microsoft::WRL::ComPtr<HoloJs::BufferOnMemory> m_responseBuffer;
    std::vector<unsigned char> m_responseBuffer1;
    Windows::Storage::Streams::IBuffer ^ m_response;
    unsigned long m_responseLength;

    JsValueRef m_xhrScriptReference;

    void finalizeRequest();

    static bool isAbsoluteWebUri(const std::wstring& uri)
    {
        return (_wcsnicmp(uri.c_str(), L"http://", wcslen(L"http://")) == 0) ||
               (_wcsnicmp(uri.c_str(), L"https://", wcslen(L"https://")) == 0);
    }
};
}  // namespace Win32
}  // namespace HoloJs