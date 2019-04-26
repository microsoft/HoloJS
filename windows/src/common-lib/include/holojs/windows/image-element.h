#pragma once

#include "IBufferOnMemory.h"
#include "holojs/private/blob-interface.h"
#include "holojs/private/chakra.h"
#include "holojs/private/event-target.h"
#include "holojs/private/holojs-view.h"
#include "holojs/private/image-interface.h"
#include "holojs/private/object-lifetime.h"

namespace HoloJs {

class HoloJsScriptHost;

namespace Win32 {

class Image : public HoloJs::IIMage {
   public:
    Image(HoloJs::IHoloJsScriptHostInternal* host) : m_host(host) {}

    virtual ~Image();

    virtual void Release() {}

    virtual long setSource(const std::wstring& source, JsValueRef imageRef);
    virtual long setSourceFromBlob(JsValueRef blobRef, HoloJs::IBlob* blob, JsValueRef imageRef);

    virtual long getImageData(const IMAGE_FORMAT_GUID& imageFormat,
                              unsigned char** pixels,
                              unsigned int& pixelBufferSize,
                              unsigned int& pixelStride,
                              ImageFlipRotation flipOperation);

    virtual unsigned int getWidth() { return m_width; }
    virtual unsigned int getHeight() { return m_height; }

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

    JsValueRef m_imageScriptReference = JS_INVALID_REFERENCE;

    HRESULT loadImageFromBuffer(Windows::Storage::Streams::IBuffer ^ imageBuffer);
    HRESULT loadImageFromArray();
    HRESULT decodeImage(IWICImagingFactory* imagingFactory);
    void finalizeLoad();

    Windows::Storage::Streams::IBuffer ^ download(const std::wstring& source,
                                                  HoloJs::AppModel::AppConfiguration configuration);
    HRESULT read(const std::wstring& source,
                 std::vector<unsigned char>& data,
                 HoloJs::AppModel::AppConfiguration configuration);

    HRESULT readFromPackage(const std::wstring& source,
                            std::vector<unsigned char>& data,
                            HoloJs::AppModel::AppConfiguration configuration);

    static bool isAbsoluteWebUri(const std::wstring& uri)
    {
        return (_wcsnicmp(uri.c_str(), L"http://", wcslen(L"http://")) == 0) ||
               (_wcsnicmp(uri.c_str(), L"https://", wcslen(L"https://")) == 0);
    }

    unsigned int m_width;
    unsigned int m_height;
    bool m_isLoaded = false;

    std::shared_ptr<std::vector<unsigned char>> m_imageArray;
    Windows::Storage::Streams::IBuffer ^ m_imageBuffer;
    Microsoft::WRL::ComPtr<IWICStream> m_imageStream;
    Microsoft::WRL::ComPtr<IWICBitmapSource> m_bitmapSource;
    Microsoft::WRL::ComPtr<IWICBitmapDecoder> m_decoder;
    Microsoft::WRL::ComPtr<IWICBitmap> m_bitmap;
    Microsoft::WRL::ComPtr<IWICBitmapLock> m_bitmapLock;
    WICPixelFormatGUID m_sourceFormat;
    WICPixelFormatGUID m_decodedFormat;

    WICInProcPointer m_pixels;
    unsigned int m_pixelsSize;
    unsigned int m_stride;
};
}  // namespace Win32
}  // namespace HoloJs