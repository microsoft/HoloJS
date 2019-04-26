#include "stdafx.h"
#include "audio-context.h"
#include "holojs/holojs.h"
#include "holojs/private/error-handling.h"
#include "holojs/private/surface-mapping.h"
#include "include/holojs/windows/speech-recognizer.h"
#include "include/holojs/windows/windows-platform.h"
#include "spatial-anchors.h"
#include "surface-mapper.h"
#include "websocket.h"
#include <experimental/filesystem>
#include <map>
#include <memory>
#include <ppltasks.h>
#include <robuffer.h>
#include <string>
#include <vector>

using namespace HoloJs::Platforms;
using namespace Windows::Data::Json;
using namespace Windows::Web::Http;
using namespace Windows::Web;
using namespace std;
using namespace concurrency;

void WindowsPlatform::debugLog(const wchar_t* message) { OutputDebugString(message); }
void WindowsPlatform::debugLog(const char* message) { OutputDebugStringA(message); }

HRESULT WindowsPlatform::base64Encode(const wchar_t* toEncode, size_t toEncodeLength, std::wstring& encoded)
{
    return E_FAIL;
}
HRESULT WindowsPlatform::base64Decode(const wchar_t* toDecode, size_t toDecodeLength, std::wstring& decoded)
{
    return E_FAIL;
}

HRESULT WindowsPlatform::getAppConfigFromJson(const std::wstring& appDefinition,
                                              std::list<std::wstring>& scriptList,
                                              std::wstring& appName,
                                              std::wstring& appIconUri)
{
    JsonObject ^ jsonObj;
    RETURN_IF_FALSE(JsonObject::TryParse(Platform::StringReference(appDefinition.c_str()), &jsonObj));

    auto scriptsListInJson = jsonObj->GetNamedArray(L"scripts");
    for (unsigned int i = 0; i < scriptsListInJson->Size; i++) {
        try {
            scriptList.push_back(scriptsListInJson->GetStringAt(i)->Data());
        } catch (...) {
            RETURN_IF_FALSE(false);
        }
    }

    if (jsonObj->HasKey("name")) {
        try {
            auto name = jsonObj->GetNamedString(L"name");
            appName = name->Data();
        } catch (...) {
            RETURN_IF_FALSE(false);
        }
    }

    if (jsonObj->HasKey("appIcon")) {
        try {
            auto icon = jsonObj->GetNamedString(L"appIcon");
            appIconUri = icon->Data();
        } catch (...) {
            RETURN_IF_FALSE(false);
        }
    }

    return S_OK;
}

HRESULT WindowsPlatform::downloadText(const std::wstring& uri, std::wstring& text)
{
    auto scriptUri = ref new Windows::Foundation::Uri(Platform::StringReference(uri.c_str()));

    auto filter = ref new Filters::HttpBaseProtocolFilter();
    filter->CacheControl->ReadBehavior = Filters::HttpCacheReadBehavior::MostRecent;

    HttpClient ^ httpClient = ref new Windows::Web::Http::HttpClient(filter);

    try {
        auto getAsync = httpClient->GetAsync(scriptUri);
        create_task(getAsync).wait();

        auto responseMessage = getAsync->GetResults();
        if (responseMessage->IsSuccessStatusCode) {
            auto readAsync = responseMessage->Content->ReadAsStringAsync();
            create_task(readAsync).wait();
            auto stringContent = readAsync->GetResults();
            text.assign(stringContent->Data());
        }
        return S_OK;
    } catch (...) {
    }

    return E_FAIL;
}

HRESULT WindowsPlatform::downloadBinary(const std::wstring& uri, std::vector<unsigned char>& data)
{
    auto scriptUri = ref new Windows::Foundation::Uri(Platform::StringReference(uri.c_str()));

    auto filter = ref new Filters::HttpBaseProtocolFilter();
    filter->CacheControl->ReadBehavior = Filters::HttpCacheReadBehavior::MostRecent;

    HttpClient ^ httpClient = ref new ::Windows::Web::Http::HttpClient(filter);

    try {
        auto getAsync = httpClient->GetAsync(scriptUri);
        create_task(getAsync).wait();

        auto responseMessage = getAsync->GetResults();
        if (responseMessage->IsSuccessStatusCode) {
            auto readAsync = responseMessage->Content->ReadAsBufferAsync();
            create_task(readAsync).wait();

            auto dataBuffer = readAsync->GetResults();

            Microsoft::WRL::ComPtr<::Windows::Storage::Streams::IBufferByteAccess> bufferByteAccess;
            RETURN_IF_FAILED(
                reinterpret_cast<IInspectable*>(dataBuffer)->QueryInterface(IID_PPV_ARGS(&bufferByteAccess)));

            unsigned char* responseNativeBuffer;
            RETURN_IF_FAILED(bufferByteAccess->Buffer(&responseNativeBuffer));

            data.resize(dataBuffer->Length);
            memcpy(data.data(), responseNativeBuffer, dataBuffer->Length);
        }
        return S_OK;
    } catch (...) {
    }

    return E_FAIL;
}

HRESULT WindowsPlatform::readFileUnicode(const std::wstring& path, std::wstring& text)
{
    auto fileHandle = unique_handle(CreateFile2(path.c_str(), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, nullptr));
    RETURN_IF_TRUE(fileHandle.get() == INVALID_HANDLE_VALUE);

    DWORD bytesRead = 0;
    bool readResult = false;
    vector<wchar_t> unicodeReadBuffer(1024);
    const auto unicodeReadBufferByteSize = unicodeReadBuffer.size() * sizeof(wchar_t);
    do {
        readResult = ReadFile(fileHandle.get(),
                              unicodeReadBuffer.data(),
                              static_cast<unsigned int>(unicodeReadBufferByteSize),
                              &bytesRead,
                              nullptr);

        if (readResult && bytesRead > 0) {
            text.append(unicodeReadBuffer.data(), bytesRead / sizeof(wchar_t));
        }
    } while (readResult && bytesRead > 0);

    return S_OK;
}

HRESULT WindowsPlatform::readFileUTF8(const std::wstring& path, std::wstring& text)
{
    auto fileHandle = unique_handle(CreateFile2(path.c_str(), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, nullptr));
    RETURN_IF_TRUE(fileHandle.get() == INVALID_HANDLE_VALUE);

    DWORD bytesRead = 0;
    bool readResult = false;
    vector<char> readBuffer(1024);
    vector<wchar_t> unicodeReadBuffer(1024);
    do {
        readResult = ReadFile(
            fileHandle.get(), readBuffer.data(), static_cast<unsigned int>(readBuffer.size()), &bytesRead, nullptr);

        if (readResult && bytesRead > 0) {
            auto convertedSize = MultiByteToWideChar(CP_UTF8,
                                                     0,
                                                     readBuffer.data(),
                                                     bytesRead,
                                                     unicodeReadBuffer.data(),
                                                     static_cast<unsigned int>(unicodeReadBuffer.capacity()));
            RETURN_IF_TRUE(convertedSize == 0);
            text.append(unicodeReadBuffer.data(), convertedSize);
        }
    } while (readResult && bytesRead > 0);

    return S_OK;
}

HRESULT WindowsPlatform::readFileBinary(const std::wstring& path, std::vector<unsigned char>& data)
{
    auto fileHandle = unique_handle(CreateFile2(path.c_str(), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, nullptr));
    RETURN_IF_TRUE(fileHandle.get() == INVALID_HANDLE_VALUE);

    return readfileBinaryFromHandle(fileHandle.get(), data);
}

HRESULT WindowsPlatform::readfileBinaryFromHandle(HANDLE handle, std::vector<unsigned char>& data)
{
    DWORD bytesRead = 0;
    bool readResult = false;
    vector<char> readBuffer(1024);
    do {
        readResult =
            ReadFile(handle, readBuffer.data(), static_cast<unsigned int>(readBuffer.size()), &bytesRead, nullptr);

        if (readResult && bytesRead > 0) {
            data.insert(data.end(), readBuffer.begin(), readBuffer.begin() + bytesRead);
        }
    } while (readResult && bytesRead > 0);

    return S_OK;
}

long WindowsPlatform::getCurrentDirectory(std::wstring& currentDirectory)
{
    unsigned int requiredBufferSize = GetCurrentDirectory(0, nullptr);

    RETURN_IF_TRUE(requiredBufferSize == 0);

    auto buffer = vector<wchar_t>(requiredBufferSize);

    RETURN_IF_TRUE(GetCurrentDirectory(requiredBufferSize, buffer.data()) == 0);

    currentDirectory = buffer.data();

    return true;
}

HoloJs::IAudioContext* WindowsPlatform::createAudioContext(HoloJs::IHoloJsScriptHostInternal* host)
{
    return new HoloJs::Win32::WebAudio::AudioContext(host);
}

HoloJs::IWebSocket* WindowsPlatform::createWebSocket(HoloJs::IHoloJsScriptHostInternal* host,
                                                     const std::wstring& url,
                                                     const std::vector<std::wstring>& protocols)
{
    auto newWebSocket = new HoloJs::Platforms::Win32::WebSocket(host);
    newWebSocket->connect(url, protocols);
    return newWebSocket;
}

bool WindowsPlatform::isSurfaceMappingAvailable()
{
    return Windows::Perception::Spatial::Surfaces::SpatialSurfaceObserver::IsSupported();
}

HoloJs::ISurfaceMapper* WindowsPlatform::getSurfaceMapper(HoloJs::IHoloJsScriptHostInternal* host)
{
    RETURN_NULL_IF_FALSE(isSurfaceMappingAvailable());

    return new HoloJs::Platforms::Win32::SurfaceMapper(host);
}

bool WindowsPlatform::isSpeechRecognizerAvailable() { return true; }

HoloJs::ISpeechRecognizer* WindowsPlatform::getSpeechRecognizer(HoloJs::IHoloJsScriptHostInternal* host)
{
    return new HoloJs::Platforms::Win32::SpeechRecognizer(host);
}

bool WindowsPlatform::canPersistAnchors(HoloJs::IHoloJsScriptHostInternal* host)
{
    return spatialAnchorsSupported(host);
}

HoloJs::ISpatialAnchorsStore* WindowsPlatform::getSpatialAnchorsStore(HoloJs::IHoloJsScriptHostInternal* host)
{
    return new HoloJs::Platforms::Win32::SpatialAnchors(host);
}

bool WindowsPlatform::spatialAnchorsSupported(HoloJs::IHoloJsScriptHostInternal* host)
{
    void* coordinateSystem = nullptr;
    return SUCCEEDED(host->getStationaryCoordinateSystem(&coordinateSystem)) && coordinateSystem != nullptr;
}

HoloJs::ISpatialAnchor* WindowsPlatform::createAnchor(HoloJs::IHoloJsScriptHostInternal* host,
                                                      HoloJs::ISpatialAnchor* relativeTo,
                                                      const std::array<double, 3>& position,
                                                      const std::array<double, 4>& orientation)
{
    return HoloJs::Platforms::Win32::HoloJsSpatialAnchor::create(host, relativeTo, position, orientation);
}