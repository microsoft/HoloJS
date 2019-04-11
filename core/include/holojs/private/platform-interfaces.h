#pragma once

#include "app-model.h"
#include "holojs-script-host-internal.h"
#include <list>
#include <memory>
#include <string>
namespace HoloJs {

class ISurfaceMapper;
class IPackageReader;
class IAudioContext;
class IWebSocket;
class IIMage;
class IXmlHttpRequest;
class ICanvasRenderContext2D;
class IHoloJsView;

class IPlatform {
   public:
    virtual IHoloJsView* makeView(HoloJs::ViewConfiguration viewConfig) = 0;

    virtual long readResourceScript(const wchar_t* name, std::wstring& scriptText) = 0;

    virtual long downloadText(const std::wstring& uri, std::wstring& text) = 0;
    virtual long downloadBinary(const std::wstring& uri, std::vector<unsigned char>& data) = 0;
    virtual long readFileUnicode(const std::wstring& path, std::wstring& text) = 0;
    virtual long readFileUTF8(const std::wstring& path, std::wstring& text) = 0;
    virtual long readFileBinary(const std::wstring& path, std::vector<unsigned char>& data) = 0;

    virtual void debugLog(const wchar_t* message) = 0;
    virtual void debugLog(const char* message) = 0;

    virtual long getAppConfigFromJson(const std::wstring& appDefinition,
                                      std::list<std::wstring>& scriptList,
                                      std::wstring& appName,
                                      std::wstring& appIconUri) = 0;

    virtual long base64Encode(const wchar_t* toEncode, size_t toEncodeLength, std::wstring& encoded) = 0;
    virtual long base64Decode(const wchar_t* toDecode, size_t toDecodeLength, std::wstring& decoded) = 0;

    virtual HoloJs::ICanvasRenderContext2D* createCanvasRenderContext2D() = 0;

    virtual HoloJs::IAudioContext* createAudioContext(HoloJs::IHoloJsScriptHostInternal* host) = 0;

    virtual HoloJs::IIMage* createImage(HoloJs::IHoloJsScriptHostInternal* host) = 0;
    virtual HoloJs::IXmlHttpRequest* createXmlHttpRequest(HoloJs::IHoloJsScriptHostInternal* host) = 0;
    virtual HoloJs::IWebSocket* createWebSocket(HoloJs::IHoloJsScriptHostInternal* host,
                                                const std::wstring& url,
                                                const std::vector<std::wstring>& protocols) = 0;

    virtual long getCurrentDirectory(std::wstring& currentDirectory) = 0;

    virtual void enableDebugger(HoloJs::IHoloJsScriptHost* host, JsRuntimeHandle runtime) = 0;

    virtual HoloJs::IPackageReader* getPackageReaderFromPath(const std::wstring& packagePath) = 0;
    virtual HoloJs::IPackageReader* getPackageReaderFromHandle(void* platformHandle) = 0;

    virtual bool isSurfaceMappingAvailable() = 0;
	virtual HoloJs::ISurfaceMapper* getSurfaceMapper(HoloJs::IHoloJsScriptHostInternal* host) = 0;
};

IPlatform* __cdecl getPlatform();

namespace PrivateInterface {
class ::HoloJs::IHoloJsScriptHost;
IHoloJsScriptHost* __cdecl CreateHoloJsScriptHost();
void __cdecl DeleteHoloJsScriptHost(::HoloJs::IHoloJsScriptHost* scriptHost);
}  // namespace PrivateInterface

}  // namespace HoloJs