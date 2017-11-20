#include "pch.h"
#include "XmlHttpRequest.h"
#include "ExternalObject.h"
#include "ScriptHostUtilities.h"
#include "ScriptResourceTracker.h"
#include "ScriptsLoader.h"

using namespace HologramJS::API;
using namespace Windows::Foundation;
using namespace Windows::Web::Http;
using namespace HologramJS::Utilities;
using namespace concurrency;
using namespace std;
using namespace Microsoft::WRL;
using namespace Windows::Storage::Streams;

bool XmlHttpRequest::UseFileSystem = false;
wstring XmlHttpRequest::BaseUrl = L"";
wstring XmlHttpRequest::BasePath = L"";
JsValueRef XmlHttpRequest::m_createXHRFunction = JS_INVALID_REFERENCE;
JsValueRef XmlHttpRequest::m_getResponseFunction = JS_INVALID_REFERENCE;
JsValueRef XmlHttpRequest::m_getResponseTextFunction = JS_INVALID_REFERENCE;
JsValueRef XmlHttpRequest::m_sendXHRFunction = JS_INVALID_REFERENCE;
JsValueRef XmlHttpRequest::m_getHeaderFunction = JS_INVALID_REFERENCE;
JsValueRef XmlHttpRequest::m_setHeaderFunction = JS_INVALID_REFERENCE;

XmlHttpRequest::XmlHttpRequest() : m_refScriptPayloadValue(JS_INVALID_REFERENCE) {}

XmlHttpRequest::~XmlHttpRequest()
{
    if (m_refScriptPayloadValue != JS_INVALID_REFERENCE) {
        JsRelease(m_refScriptPayloadValue, nullptr);
        m_refScriptPayloadValue = JS_INVALID_REFERENCE;
    }
}

bool XmlHttpRequest::Initialize()
{
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"create", L"xhr", createXHR, nullptr, &m_createXHRFunction));
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"send", L"xhr", sendXHR, nullptr, &m_sendXHRFunction));
    RETURN_IF_FALSE(
        ScriptHostUtilities::ProjectFunction(L"getResponse", L"xhr", getResponse, nullptr, &m_getResponseFunction));
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(
        L"getResponseText", L"xhr", getResponseText, nullptr, &m_getResponseTextFunction));
    RETURN_IF_FALSE(
        ScriptHostUtilities::ProjectFunction(L"getHeader", L"xhr", getHeader, nullptr, &m_getHeaderFunction));
    RETURN_IF_FALSE(
        ScriptHostUtilities::ProjectFunction(L"setHeader", L"xhr", setHeader, nullptr, &m_setHeaderFunction));

    return true;
}

_Use_decl_annotations_ JsValueRef CHAKRA_CALLBACK XmlHttpRequest::createXHR(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    ExternalObject* externalObject = new ExternalObject();
    RETURN_INVALID_REF_IF_FALSE(externalObject->Initialize(new XmlHttpRequest()));
    return ScriptResourceTracker::ObjectToDirectExternal(externalObject);
}

_Use_decl_annotations_ JsValueRef CHAKRA_CALLBACK XmlHttpRequest::setHeader(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);
    auto xhr = ScriptResourceTracker::ExternalToObject<XmlHttpRequest>(arguments[1]);
    RETURN_INVALID_REF_IF_NULL(xhr);

    wstring header;
    RETURN_INVALID_REF_IF_FALSE(ScriptHostUtilities::GetString(arguments[2], header));

    wstring value;
    RETURN_INVALID_REF_IF_FALSE(ScriptHostUtilities::GetString(arguments[3], value));

    xhr->m_requestHeaders.emplace_back(header, value);

    return JS_INVALID_REFERENCE;
}

_Use_decl_annotations_ JsValueRef CHAKRA_CALLBACK XmlHttpRequest::getHeader(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);
    auto xhr = ScriptResourceTracker::ExternalToObject<XmlHttpRequest>(arguments[1]);
    RETURN_INVALID_REF_IF_NULL(xhr);

    wstring header;
    RETURN_INVALID_REF_IF_FALSE(ScriptHostUtilities::GetString(arguments[2], header));

    auto headerRef = Platform::StringReference(header.c_str());
    if (xhr->m_responseHeaders->HasKey(headerRef)) {
        auto valueRef = xhr->m_responseHeaders->Lookup(headerRef);
        JsValueRef returnValue;
        RETURN_INVALID_REF_IF_JS_ERROR(JsPointerToString(valueRef->Data(), valueRef->Length(), &returnValue));

        return returnValue;
    } else {
        return JS_INVALID_REFERENCE;
    }
}

_Use_decl_annotations_ JsValueRef CHAKRA_CALLBACK XmlHttpRequest::sendXHR(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 6);
    auto xhr = ScriptResourceTracker::ExternalToObject<XmlHttpRequest>(arguments[1]);
    RETURN_INVALID_REF_IF_NULL(xhr);

    wstring method;
    RETURN_INVALID_REF_IF_FALSE(ScriptHostUtilities::GetString(arguments[2], method));

    wstring uri;
    RETURN_INVALID_REF_IF_FALSE(ScriptHostUtilities::GetString(arguments[3], uri));

    wstring type;
    RETURN_INVALID_REF_IF_FALSE(ScriptHostUtilities::GetString(arguments[4], type));

    RETURN_INVALID_REF_IF_FALSE(xhr->CreateHttpContent(arguments[5]));

    RETURN_INVALID_REF_IF_FAILED(xhr->SendRequest(method, uri, type));

    return JS_INVALID_REFERENCE;
}

_Use_decl_annotations_ JsValueRef CHAKRA_CALLBACK XmlHttpRequest::getResponseText(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);
    auto xhr = ScriptResourceTracker::ExternalToObject<XmlHttpRequest>(arguments[1]);
    RETURN_INVALID_REF_IF_NULL(xhr);

    RETURN_INVALID_REF_IF_FALSE(xhr->IsTextResponse());

    JsValueRef returnValue;
    RETURN_INVALID_REF_IF_JS_ERROR(
        JsPointerToString(xhr->m_responseText.c_str(), xhr->m_responseText.length(), &returnValue));

    return returnValue;
}

_Use_decl_annotations_ JsValueRef CHAKRA_CALLBACK XmlHttpRequest::getResponse(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);
    auto xhr = ScriptResourceTracker::ExternalToObject<XmlHttpRequest>(arguments[1]);
    RETURN_INVALID_REF_IF_NULL(xhr);

    if (xhr->IsTextResponse()) {
        return getResponseText(callee, isConstructCall, arguments, argumentCount, callbackData);
    } else {
        Microsoft::WRL::ComPtr<Windows::Storage::Streams::IBufferByteAccess> bufferByteAccess;
        RETURN_INVALID_REF_IF_FAILED(
            reinterpret_cast<IInspectable*>(xhr->m_response)->QueryInterface(IID_PPV_ARGS(&bufferByteAccess)));

        byte* responseNativeBuffer;
        RETURN_INVALID_REF_IF_FAILED(bufferByteAccess->Buffer(&responseNativeBuffer));

        JsValueRef returnArray;
        void* responesExternalBuffer;
        RETURN_INVALID_REF_IF_JS_ERROR(ScriptResourceTracker::CreateAndTrackExternalBuffer(
            xhr->m_responseLength, &responesExternalBuffer, &returnArray));

        CopyMemory(responesExternalBuffer, responseNativeBuffer, xhr->m_responseLength);

        return returnArray;
    }
}

bool XmlHttpRequest::CreateHttpContent(JsValueRef scriptContent)
{
    JsValueType contentType;
    RETURN_IF_JS_ERROR(JsGetValueType(scriptContent, &contentType));

    if (contentType == JsUndefined || contentType == JsNull) {
        // No payload
        return true;
    } else if (contentType == JsString) {
        wstring payload;
        RETURN_IF_FALSE(ScriptHostUtilities::GetString(scriptContent, payload));
        m_httpContent = ref new HttpStringContent(Platform::StringReference(payload.c_str()));
    } else if (contentType == JsTypedArray || contentType == JsArrayBuffer) {
        byte* buffer;
        unsigned int bufferLength;

        if (contentType == JsTypedArray) {
            JsTypedArrayType arrayType;
            int elementSize;
            RETURN_INVALID_REF_IF_JS_ERROR(
                JsGetTypedArrayStorage(scriptContent, &buffer, &bufferLength, &arrayType, &elementSize));
        } else {
            RETURN_IF_JS_ERROR(JsGetArrayBufferStorage(scriptContent, &buffer, &bufferLength));
        }

        RETURN_IF_JS_ERROR(JsAddRef(scriptContent, nullptr));
        m_refScriptPayloadValue = scriptContent;

        Details::MakeAndInitialize<BufferOnMemory>(&m_contentBuffer, buffer, bufferLength);
        auto iinspectable = (IInspectable*)reinterpret_cast<IInspectable*>(m_contentBuffer.Get());
        m_contentIBuffer = reinterpret_cast<IBuffer ^>(iinspectable);

        m_httpContent = ref new HttpBufferContent(m_contentIBuffer);
        m_contentIsBufferType = true;
    } else {
        // Other payload type?
        return false;
    }

    return true;
}

HRESULT XmlHttpRequest::SendRequest(const std::wstring& method, const std::wstring& uri, const std::wstring type)
{
    m_method = method;
    m_url = uri;
    m_state = RequestState::OPENED;
    m_responseType = type;

    if (m_url.empty()) {
        return E_INVALIDARG;
    }

    if (ScriptsLoader::IsAbsoluteWebUri(m_url) || !UseFileSystem) {
        SendAsync();
    } else {
        if (m_httpContent != nullptr || _wcsicmp(method.c_str(), L"get") != 0) {
            // Cannot have request payload when resolving from local app package
            // Only GET is supported for loading from the local app package
            return E_NOTIMPL;
        }
        ReadFromPackageAsync();
    }

    return S_OK;
}

task<void> XmlHttpRequest::ReadFromPackageAsync()
{
    wstring completePath = BasePath + m_url;

    if (IsTextResponse()) {
        auto responsePlatformString = await HologramJS::Loaders::FileSystemLoader::ReadTextFromFileAsync(
            Windows::ApplicationModel::Package::Current->InstalledLocation, completePath);
        m_responseText = responsePlatformString->Data();
        m_responseType.assign(L"text");
    } else  // arraybuffer or blob
    {
        m_response = await HologramJS::Loaders::FileSystemLoader::ReadBinaryFromFileAsync(
            Windows::ApplicationModel::Package::Current->InstalledLocation, completePath);

        m_responseLength = m_response->Length;
    }

    m_state = RequestState::DONE;
    m_status = 200;
    m_statusText = L"OK";

    FireStateChanged();
}

task<void> XmlHttpRequest::SendAsync()
{
    Windows::Foundation::Uri ^ uri;

    if ((_wcsnicmp(m_url.c_str(), L"http://", wcslen(L"http://")) == 0) ||
        (_wcsnicmp(m_url.c_str(), L"https://", wcslen(L"https://")) == 0)) {
        uri = ref new Windows::Foundation::Uri(Platform::StringReference(m_url.c_str()));
    } else {
        wstring completeUrl = BaseUrl + L"/" + m_url;
        uri = ref new Windows::Foundation::Uri(Platform::StringReference(completeUrl.c_str()));
    }

    Windows::Web::Http::HttpClient ^ httpClient = ref new Windows::Web::Http::HttpClient();

    HttpRequestMessage ^ requestMessage = ref new HttpRequestMessage();
    for (const auto& headerPair : m_requestHeaders) {
        if (_wcsicmp(headerPair.first.c_str(), L"content-type") == 0) {
            if (m_httpContent != nullptr) {
                m_httpContent->Headers->ContentType->MediaType = Platform::StringReference(headerPair.second.c_str());
            }
        } else {
            requestMessage->Headers->Append(Platform::StringReference(headerPair.first.c_str()),
                                            Platform::StringReference(headerPair.second.c_str()));
        }
    }

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
            responseMessage = await httpClient->SendRequestAsync(requestMessage);
        } catch (...) {
            m_status = -1;
        }
    }

    if (responseMessage) {
        if (responseMessage->IsSuccessStatusCode) {
            if (IsTextResponse()) {
                auto responseText = await responseMessage->Content->ReadAsStringAsync();
                m_responseText.assign(responseText->Data());
            } else {
                m_response = await responseMessage->Content->ReadAsBufferAsync();
                m_responseLength = m_response->Length;
            }

            m_status = 200;
            m_statusText = L"OK";
            m_responseHeaders = responseMessage->Headers;
        } else {
            m_status = static_cast<int>(responseMessage->StatusCode);
            m_statusText.assign(responseMessage->ReasonPhrase->Data());
        }
    }

    m_state = RequestState::DONE;

    FireStateChanged();

    return;
}

void XmlHttpRequest::FireStateChanged()
{
    if (HasCallback()) {
        vector<JsValueRef> parameters(6);

        parameters[0] = m_scriptCallbackContext;

        JsValueRef* typeParam = &parameters[1];
        EXIT_IF_JS_ERROR(JsPointerToString(L"change", wcslen(L"change"), typeParam));

        JsValueRef* stateParam = &parameters[2];
        EXIT_IF_JS_ERROR(JsIntToNumber(static_cast<int>(m_state), stateParam));

        JsValueRef* statusParam = &parameters[3];
        EXIT_IF_JS_ERROR(JsIntToNumber(static_cast<int>(m_status), statusParam));

        JsValueRef* statusTextParam = &parameters[4];
        EXIT_IF_JS_ERROR(JsPointerToString(m_statusText.c_str(), m_statusText.length(), statusTextParam));

        JsValueRef* responesTypeParam = &parameters[5];
        EXIT_IF_JS_ERROR(JsPointerToString(m_responseType.c_str(), m_responseType.length(), responesTypeParam));

        JsValueRef result;
        HANDLE_EXCEPTION_IF_JS_ERROR(JsCallFunction(
            m_scriptCallback, parameters.data(), static_cast<unsigned short>(parameters.size()), &result));
    }
}
