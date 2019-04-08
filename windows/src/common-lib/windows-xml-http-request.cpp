#include "stdafx.h"
#include "holojs/holojs-script-host.h"
#include "holojs/private/app-model.h"
#include "holojs/private/error-handling.h"
#include "holojs/private/platform-interfaces.h"
#include "holojs/private/script-host-utilities.h"
#include "include/holojs/windows/xml-http-request.h"
#include <experimental/filesystem>
#include <functional>
#include <ppltasks.h>
#include <string>

using namespace HoloJs;
using namespace HoloJs::Win32;
using namespace std;
using namespace Windows::Web::Http;
using namespace Windows::Storage::Streams;
using namespace concurrency;

XmlHttpRequest::~XmlHttpRequest() {}

JsValueRef XmlHttpRequest::getResponseHeader(const std::wstring& name)
{
    JsValueRef headerValue = JS_INVALID_REFERENCE;
    auto headerRef = Platform::StringReference(name.c_str());

    if (m_responseHeaders && m_responseHeaders->HasKey(headerRef)) {
        auto valueRef = m_responseHeaders->Lookup(headerRef);

        RETURN_INVALID_REF_IF_JS_ERROR(JsPointerToString(valueRef->Data(), valueRef->Length(), &headerValue));
    } else {
        RETURN_INVALID_REF_IF_JS_ERROR(JsGetNullValue(&headerValue));
    }

    return headerValue;
}

HRESULT XmlHttpRequest::createHttpContent(JsValueRef scriptContent)
{
    JsValueType contentType;
    RETURN_IF_JS_ERROR(JsGetValueType(scriptContent, &contentType));

    if (contentType == JsUndefined || contentType == JsNull) {
        // No payload
        return S_OK;
    } else if (contentType == JsString) {
        wstring payloadString;
        RETURN_IF_FAILED(ScriptHostUtilities::GetString(scriptContent, payloadString));
        m_httpContent = ref new HttpStringContent(Platform::StringReference(payloadString.c_str()));
        m_contentType = HttpContentType::String;
    } else if (contentType == JsTypedArray || contentType == JsArrayBuffer) {
        unsigned char* buffer;
        unsigned int bufferLength;

        if (contentType == JsTypedArray) {
            JsTypedArrayType arrayType;
            int elementSize;
            RETURN_IF_JS_ERROR(JsGetTypedArrayStorage(scriptContent, &buffer, &bufferLength, &arrayType, &elementSize));
        } else {
            RETURN_IF_JS_ERROR(JsGetArrayBufferStorage(scriptContent, &buffer, &bufferLength));
        }

        RETURN_IF_JS_ERROR(JsAddRef(scriptContent, nullptr));
        m_refScriptPayloadValue = scriptContent;

        Microsoft::WRL::Details::MakeAndInitialize<HoloJs::BufferOnMemory>(&m_contentBuffer, buffer, bufferLength);
        auto iinspectable = (IInspectable*)reinterpret_cast<IInspectable*>(m_contentBuffer.Get());
        m_contentIBuffer = reinterpret_cast<IBuffer ^>(iinspectable);

        m_httpContent = ref new HttpBufferContent(m_contentIBuffer);
        m_contentType = HttpContentType::Buffer;
    } else {
        // Other payload type?
        return E_FAIL;
    }

    return S_OK;
}

HRESULT XmlHttpRequest::send(const wstring& method, const wstring& uri, const wstring& type, JsValueRef xhrRef)
{
    // Only one async send at any give time
    RETURN_IF_TRUE(m_xhrScriptReference != JS_INVALID_REFERENCE);

    HoloJs::AppModel::AppConfiguration configuration;
    RETURN_IF_FAILED(m_host->getActiveAppConfiguration(&configuration));

    m_method = method;
    m_url = uri;
    m_state = RequestState::OPENED;
    m_responseType = type;

    m_xhrScriptReference = xhrRef;
    RETURN_IF_FAILED(JsAddRef(xhrRef, nullptr));

    if (m_url.empty()) {
        return E_INVALIDARG;
    }

    if (isAbsoluteWebUri(m_url) || (configuration.source == AppModel::AppSource::Web)) {
        create_task([this, configuration]() {
#ifdef LIBHOLOJS_WIN32
            Windows::Foundation::Initialize();
#endif
            sendSync(configuration);
            m_host->runInScriptContext(std::function<void()>(std::bind(&XmlHttpRequest::finalizeRequest, this)));
        });
    } else if (configuration.source == AppModel::AppSource::FileSystem) {
        if (m_httpContent != nullptr || _wcsicmp(method.c_str(), L"get") != 0) {
            // Cannot have request payload when resolving from local app package
            // Only GET is supported for loading from the local app package
            return E_NOTIMPL;
        }

        create_task([this, configuration]() {
#ifdef LIBHOLOJS_WIN32
            Windows::Foundation::Initialize();
#endif
            readFromFileSystemSync(configuration);
            m_host->runInScriptContext(std::function<void()>(std::bind(&XmlHttpRequest::finalizeRequest, this)));
        });
    } else if (configuration.source == AppModel::AppSource::Package) {
        create_task([this, configuration]() {
#ifdef LIBHOLOJS_WIN32
            Windows::Foundation::Initialize();
#endif
            readFromPackageSync(configuration);
            m_host->runInScriptContext(std::function<void()>(std::bind(&XmlHttpRequest::finalizeRequest, this)));
        });
    }

    return S_OK;
}

HRESULT XmlHttpRequest::sendSync(HoloJs::AppModel::AppConfiguration configuration)
{
    Windows::Foundation::Uri ^ uri;

    if (isAbsoluteWebUri(m_url)) {
        uri = ref new Windows::Foundation::Uri(Platform::StringReference(m_url.c_str()));
    } else {
        wstring completeUrl = configuration.baseUri + L"/" + m_url;
        uri = ref new Windows::Foundation::Uri(Platform::StringReference(completeUrl.c_str()));
    }

    auto filter = ref new Filters::HttpBaseProtocolFilter();
    filter->CacheControl->ReadBehavior = Filters::HttpCacheReadBehavior::MostRecent;
    Windows::Web::Http::HttpClient ^ httpClient = ref new Windows::Web::Http::HttpClient(filter);

    HttpRequestMessage ^ requestMessage = ref new HttpRequestMessage();
    setHeaders(requestMessage);

    HttpResponseMessage ^ responseMessage;

    requestMessage->RequestUri = uri;

    if (m_httpContent != nullptr) {
        requestMessage->Content = m_httpContent;
    }

    if (_wcsicmp(m_method.c_str(), L"get") == 0) {
        requestMessage->Method = Windows::Web::Http::HttpMethod::Get;
    } else if (_wcsicmp(m_method.c_str(), L"post") == 0) {
        requestMessage->Method = Windows::Web::Http::HttpMethod::Post;
    } else if (_wcsicmp(m_method.c_str(), L"put") == 0) {
        requestMessage->Method = Windows::Web::Http::HttpMethod::Put;
    } else if (_wcsicmp(m_method.c_str(), L"delete") == 0) {
        requestMessage->Method = Windows::Web::Http::HttpMethod::Delete;
    } else if (_wcsicmp(m_method.c_str(), L"head") == 0) {
        requestMessage->Method = Windows::Web::Http::HttpMethod::Head;
    } else if (_wcsicmp(m_method.c_str(), L"patch") == 0) {
        requestMessage->Method = Windows::Web::Http::HttpMethod::Patch;
    } else {
        requestMessage = nullptr;
        m_status = -1;
    }

    if (requestMessage) {
        try {
            auto sendRequestAsync = httpClient->SendRequestAsync(requestMessage);
            create_task(sendRequestAsync).wait();
            responseMessage = sendRequestAsync->GetResults();
        } catch (Platform::Exception ^ ex) {
            m_status = 503;  // Service unavailable
            m_statusText.assign(ex->Message->Data());
        } catch (...) {
            m_status = -1;
        }
    }

    if (responseMessage) {
        if (responseMessage->IsSuccessStatusCode) {
            if (isTextResponse()) {
                auto readAsStringAsync = responseMessage->Content->ReadAsStringAsync();
                create_task(readAsStringAsync).wait();
                auto responseText = readAsStringAsync->GetResults();

                m_responseText.assign(responseText->Data());
            } else {
                auto readAsBufferAsync = responseMessage->Content->ReadAsBufferAsync();
                create_task(readAsBufferAsync).wait();
                m_response = readAsBufferAsync->GetResults();
                m_responseLength = m_response->Length;
            }

            m_status = 200;
            m_statusText = L"OK";
            m_responseHeaders = responseMessage->Content->Headers;
        } else {
            m_status = static_cast<int>(responseMessage->StatusCode);
            m_statusText.assign(responseMessage->ReasonPhrase->Data());
        }
    } else {
        m_status = static_cast<int>(500);
        m_statusText = L"Internal server error";
    }

    m_state = RequestState::DONE;

    return S_OK;
}

HRESULT XmlHttpRequest::readFromFileSystemSync(HoloJs::AppModel::AppConfiguration configuration)
{
    RETURN_IF_TRUE(configuration.source != AppModel::AppSource::FileSystem);

    experimental::filesystem::path completePath(configuration.baseUri);
    completePath.append(m_url);

    RETURN_IF_TRUE(completePath.is_relative());

    if (isTextResponse()) {
        wstring text;
        RETURN_IF_FAILED(HoloJs::getPlatform()->readFileUTF8(completePath, m_responseText));
        m_responseType.assign(L"text");
    } else  // arraybuffer or blob
    {
        RETURN_IF_FAILED(HoloJs::getPlatform()->readFileBinary(completePath, m_responseBuffer1));

        Microsoft::WRL::Details::MakeAndInitialize<HoloJs::BufferOnMemory>(
            &m_responseBuffer, m_responseBuffer1.data(), static_cast<unsigned int>(m_responseBuffer1.size()));
        auto iinspectable = (IInspectable*)reinterpret_cast<IInspectable*>(m_responseBuffer.Get());
        m_response = reinterpret_cast<IBuffer ^>(iinspectable);

        m_responseLength = m_response->Length;
    }

    m_state = RequestState::DONE;
    m_status = 200;
    m_statusText = L"OK";

    return S_OK;

    // FireStateChanged();
}

HRESULT XmlHttpRequest::readFromPackageSync(HoloJs::AppModel::AppConfiguration configuration)
{
    RETURN_IF_TRUE(configuration.source != AppModel::AppSource::Package);

    if (isTextResponse()) {
        wstring text;
        RETURN_IF_FAILED(configuration.packageReader->readFileFromPackageUTF8(m_url, m_responseText));
        m_responseType.assign(L"text");
    } else  // arraybuffer or blob
    {
        RETURN_IF_FAILED(configuration.packageReader->readFileFromPackageBinary(m_url, m_responseBuffer1));

        Microsoft::WRL::Details::MakeAndInitialize<HoloJs::BufferOnMemory>(
            &m_responseBuffer, m_responseBuffer1.data(), static_cast<unsigned int>(m_responseBuffer1.size()));
        auto iinspectable = (IInspectable*)reinterpret_cast<IInspectable*>(m_responseBuffer.Get());
        m_response = reinterpret_cast<IBuffer ^>(iinspectable);

        m_responseLength = m_response->Length;
    }

    m_state = RequestState::DONE;
    m_status = 200;
    m_statusText = L"OK";

    return S_OK;

    // FireStateChanged();
}

void XmlHttpRequest::setHeaders(HttpRequestMessage ^ requestMessage)
{
    for (const auto& headerPair : m_requestHeaders) {
        if (isContentHeader(headerPair.first)) {
            if (m_httpContent != nullptr) {
                m_httpContent->Headers->Insert(Platform::StringReference(headerPair.first.c_str()),
                                               Platform::StringReference(headerPair.second.c_str()));
            }
        } else {
            requestMessage->Headers->Insert(Platform::StringReference(headerPair.first.c_str()),
                                            Platform::StringReference(headerPair.second.c_str()));
        }
    }
}

bool XmlHttpRequest::isContentHeader(const std::wstring& headerName)
{
    return (_wcsicmp(headerName.c_str(), L"content-type") == 0);
}

void XmlHttpRequest::finalizeRequest()
{
    static const std::wstring readyStateId = L"readyState";
    static const std::wstring statusId = L"status";
    static const std::wstring statusTextId = L"statusText";
    static const std::wstring responseTypeId = L"responseType";
    static const std::wstring responseTextId = L"responseText";
    static const std::wstring responseId = L"response";

    JsValueRef readyStateRef;
    EXIT_IF_JS_ERROR(JsIntToNumber(static_cast<int>(m_state), &readyStateRef));
    EXIT_IF_FAILED(ScriptHostUtilities::SetJsProperty(m_xhrScriptReference, readyStateId, readyStateRef));

    JsValueRef statusRef;
    EXIT_IF_JS_ERROR(JsIntToNumber(static_cast<int>(m_status), &statusRef));
    EXIT_IF_FAILED(ScriptHostUtilities::SetJsProperty(m_xhrScriptReference, statusId, statusRef));

    JsValueRef statusTextRef;
    EXIT_IF_JS_ERROR(JsPointerToString(m_statusText.c_str(), m_statusText.length(), &statusTextRef));
    EXIT_IF_FAILED(ScriptHostUtilities::SetJsProperty(m_xhrScriptReference, statusTextId, statusTextRef));

    JsValueRef responseTypeRef;
    EXIT_IF_JS_ERROR(JsPointerToString(m_responseType.c_str(), m_responseType.length(), &responseTypeRef));
    EXIT_IF_FAILED(ScriptHostUtilities::SetJsProperty(m_xhrScriptReference, responseTypeId, responseTypeRef));

    if (isTextResponse()) {
        JsValueRef responseTextRef;
        EXIT_IF_JS_ERROR(JsPointerToString(m_responseText.c_str(), m_responseText.length(), &responseTextRef));
        EXIT_IF_FAILED(ScriptHostUtilities::SetJsProperty(m_xhrScriptReference, responseTextId, responseTextRef));
        EXIT_IF_FAILED(ScriptHostUtilities::SetJsProperty(m_xhrScriptReference, responseId, responseTextRef));
    } else {
        JsValueRef responseRef;
        if (m_response) {
            Microsoft::WRL::ComPtr<Windows::Storage::Streams::IBufferByteAccess> bufferByteAccess;
            EXIT_IF_FAILED(
                reinterpret_cast<IInspectable*>(m_response)->QueryInterface(IID_PPV_ARGS(&bufferByteAccess)));

            unsigned char* responseNativeBuffer;
            EXIT_IF_FAILED(bufferByteAccess->Buffer(&responseNativeBuffer));

            EXIT_IF_FAILED(ScriptHostUtilities::CreateArrayBufferFromBuffer(
                &responseRef, responseNativeBuffer, m_response->Length));
        } else {
            EXIT_IF_JS_ERROR(JsGetNullValue(&responseRef));
        }

        EXIT_IF_FAILED(ScriptHostUtilities::SetJsProperty(m_xhrScriptReference, responseId, responseRef));
    }

    if (m_status == 200) {
        m_eventTargetImplementation.invokeEventListeners(L"load");
    } else {
        m_eventTargetImplementation.invokeEventListeners(L"error");
    }

    JsRelease(m_xhrScriptReference, nullptr);
    m_xhrScriptReference = JS_INVALID_REFERENCE;
}