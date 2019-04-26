#pragma once

#include "holojs/holojs-script-host.h"
#include "holojs/private/holojs-view.h"
#include "holojs/private/platform-interfaces.h"
#include "holojs/private/webaudio-interfaces.h"

namespace HoloJs {
namespace Platforms {

struct handle_deleter {
    void operator()(void* handle)
    {
        if (handle != nullptr) CloseHandle(handle);
    }
};

typedef std::unique_ptr<void, handle_deleter> unique_handle;

class WindowsPlatform : public HoloJs::IPlatform {
   public:
    virtual IHoloJsView* makeView(HoloJs::ViewConfiguration viewConfig) = 0;

    virtual HRESULT readResourceScript(const wchar_t* name, std::wstring& scriptText) = 0;

    virtual HRESULT downloadText(const std::wstring& uri, std::wstring& text);
    virtual HRESULT downloadBinary(const std::wstring& uri, std::vector<unsigned char>& data);
    virtual HRESULT readFileUnicode(const std::wstring& path, std::wstring& text);
    virtual HRESULT readFileUTF8(const std::wstring& path, std::wstring& text);
    virtual HRESULT readFileBinary(const std::wstring& path, std::vector<unsigned char>& data);

    virtual void debugLog(const wchar_t* message);
    virtual void debugLog(const char* message);

    virtual HRESULT getAppConfigFromJson(const std::wstring& appDefinition,
                                         std::list<std::wstring>& scriptList,
                                         std::wstring& appname,
                                         std::wstring& appIconUri);

    virtual HRESULT base64Encode(const wchar_t* toEncode, size_t toEncodeLength, std::wstring& encoded);
    virtual HRESULT base64Decode(const wchar_t* toDecode, size_t toDecodeLength, std::wstring& decoded);

    virtual long getCurrentDirectory(std::wstring& currentDirectory);

    virtual HoloJs::IXmlHttpRequest* createXmlHttpRequest(HoloJs::IHoloJsScriptHostInternal* host) = 0;
    virtual HoloJs::IWebSocket* createWebSocket(HoloJs::IHoloJsScriptHostInternal* host,
                                                const std::wstring& url,
                                                const std::vector<std::wstring>& protocols);

    virtual HoloJs::IIMage* createImage(HoloJs::IHoloJsScriptHostInternal* host) = 0;

    virtual HoloJs::IAudioContext* createAudioContext(HoloJs::IHoloJsScriptHostInternal* host);

    virtual HoloJs::IPackageReader* getPackageReaderFromPath(const std::wstring& packagePath);
    virtual HoloJs::IPackageReader* getPackageReaderFromHandle(void* platformHandle);

    static HRESULT readfileBinaryFromHandle(HANDLE handle, std::vector<unsigned char>& data);

    virtual bool isSurfaceMappingAvailable();
    virtual HoloJs::ISurfaceMapper* getSurfaceMapper(HoloJs::IHoloJsScriptHostInternal* host);

    virtual bool isSpeechRecognizerAvailable();
    virtual HoloJs::ISpeechRecognizer* getSpeechRecognizer(HoloJs::IHoloJsScriptHostInternal* host);

    virtual bool canPersistAnchors(HoloJs::IHoloJsScriptHostInternal* host);
    virtual HoloJs::ISpatialAnchorsStore* getSpatialAnchorsStore(HoloJs::IHoloJsScriptHostInternal* host);

    virtual bool spatialAnchorsSupported(HoloJs::IHoloJsScriptHostInternal* host);
    virtual ISpatialAnchor* createAnchor(HoloJs::IHoloJsScriptHostInternal* host,
                                         ISpatialAnchor* relativeTo,
                                         const std::array<double, 3>& position,
                                         const std::array<double, 4>& orientation);
};
}  // namespace Platforms
}  // namespace HoloJs
