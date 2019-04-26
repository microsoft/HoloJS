#include "stdafx.h"
#include "holojs/holojs-script-host.h"
#include "holojs/private/app-model.h"
#include "holojs/private/error-handling.h"
#include "holojs/private/platform-interfaces.h"
#include "holojs/private/script-host-utilities.h"
#include "include/holojs/windows/image-element.h"
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
using namespace Windows::Security::Cryptography;

Image::~Image() {}

long Image::setSource(const std::wstring& source, JsValueRef imageRef)
{
    RETURN_IF_TRUE(m_imageScriptReference != JS_INVALID_REFERENCE);

    HoloJs::AppModel::AppConfiguration configuration;
    RETURN_IF_FAILED(m_host->getActiveAppConfiguration(&configuration));

    m_imageScriptReference = imageRef;
    RETURN_IF_JS_ERROR(JsAddRef(imageRef, nullptr));

    if (source.find(L"data:") == 0) {
        // This might be a data URL; check if it's  one of the suported types
        PCWSTR pngPrefix = L"data:image/png;base64,";
        PCWSTR jpegPrefix = L"data:image/jpeg;base64,";

        Platform::StringReference base64Data;
        size_t prefixLength = 0;
        if (source.find(jpegPrefix) == 0) {
            prefixLength = wcslen(jpegPrefix);
        } else if (source.find(pngPrefix) == 0) {
            prefixLength = wcslen(pngPrefix);
        } else {
            finalizeLoad();
            RETURN_IF_FALSE(false);
        }

        base64Data = Platform::StringReference(source.c_str() + prefixLength, source.length() - prefixLength);

        IBuffer ^ imageData;
        try {
            imageData = CryptographicBuffer::DecodeFromBase64String(base64Data);
        } catch (...) {
        }

        if (imageData) {
            m_host->runInBackground([this, imageData]() ->long {
                loadImageFromBuffer(imageData);
                m_host->runInScriptContext(std::bind(&Image::finalizeLoad, this));
                return S_OK;
            });
        } else {
            // We're already on the script thread; finalize load in error now
            finalizeLoad();
        }
    } else if (isAbsoluteWebUri(source) || (configuration.source == AppModel::AppSource::Web)) {
        m_host->runInBackground([this, source, configuration]() -> long {
            IBuffer ^ data = download(source, configuration);

            if (data) {
                loadImageFromBuffer(data);
            }

            m_host->runInScriptContext(std::bind(&Image::finalizeLoad, this));
            return S_OK;
        });
    } else if (configuration.source == AppModel::AppSource::FileSystem) {
        m_host->runInBackground([this, source, configuration]() -> long {
            vector<unsigned char> imageArray;
            if (SUCCEEDED(read(source, imageArray, configuration))) {
                m_imageArray = make_shared<vector<unsigned char>>(std::move(imageArray));
                loadImageFromArray();
            }

            m_host->runInScriptContext(std::bind(&Image::finalizeLoad, this));
            return S_OK;
        });
    } else if (configuration.source == AppModel::AppSource::Package) {
        m_host->runInBackground([this, source, configuration]() ->long {
            vector<unsigned char> imageArray;
            if (SUCCEEDED(readFromPackage(source, imageArray, configuration))) {
                m_imageArray = make_shared<vector<unsigned char>>(std::move(imageArray));
                loadImageFromArray();
            }

            m_host->runInScriptContext(std::bind(&Image::finalizeLoad, this));

            return S_OK;
        });
    }

    return S_OK;
}
long Image::setSourceFromBlob(JsValueRef blobRef, IBlob* blob, JsValueRef imageRef)
{
    // Just one load operation at a time
    RETURN_IF_TRUE(m_imageScriptReference != JS_INVALID_REFERENCE);

    m_imageScriptReference = imageRef;
    RETURN_IF_JS_ERROR(JsAddRef(imageRef, nullptr));

    // Capture a reference to the blob's memory
    m_imageArray = blob->data();

    create_task([this]() {
        loadImageFromArray();
        m_host->runInScriptContext(std::bind(&Image::finalizeLoad, this));
    });

    return S_OK;
}

long Image::getImageData(const IMAGE_FORMAT_GUID& imageFormat,
                         unsigned char** pixels,
                         unsigned int& pixelBufferSize,
                         unsigned int& pixelStride,
                         ImageFlipRotation flipOperation)
{
    static_assert(sizeof(WICPixelFormatGUID) == sizeof(IMAGE_FORMAT_GUID), "image format type mismatch");
    WICPixelFormatGUID format;
    memcpy(&format, &imageFormat, sizeof(WICPixelFormatGUID));

    if (!m_bitmapLock) {
        // Do format conversion if required
        if (!IsEqualGUID(m_sourceFormat, format)) {
            Microsoft::WRL::ComPtr<IWICBitmapSource> converter;
            RETURN_IF_FAILED(WICConvertBitmapSource(format, m_bitmapSource.Get(), &converter));
            RETURN_IF_FAILED(converter.As(&m_bitmapSource));
        }

        Microsoft::WRL::ComPtr<IWICImagingFactory> imagingFactory;
        RETURN_IF_FAILED(CoCreateInstance(
            CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, &imagingFactory));

        // Flip image if needed
        Microsoft::WRL::ComPtr<IWICBitmapFlipRotator> flipRotator;
        if (flipOperation == ImageFlipRotation::FlipY) {
            RETURN_IF_FAILED(imagingFactory->CreateBitmapFlipRotator(&flipRotator));

            RETURN_IF_FAILED(flipRotator->Initialize(m_bitmapSource.Get(), WICBitmapTransformFlipVertical));
            RETURN_IF_FAILED(flipRotator.As(&m_bitmapSource));
        }

        RETURN_IF_FAILED(
            imagingFactory->CreateBitmapFromSource(m_bitmapSource.Get(), WICBitmapCacheOnDemand, &m_bitmap));

        // Lock the pixels
        WICRect allBitmapRect;
        allBitmapRect.X = 0;
        allBitmapRect.Y = 0;
        allBitmapRect.Width = m_width;
        allBitmapRect.Height = m_height;
        RETURN_IF_FAILED(m_bitmap->Lock(&allBitmapRect, WICBitmapLockRead, &m_bitmapLock));

        WICInProcPointer localPixels;
        unsigned int localPixelsSize;
        unsigned int localStride;

        RETURN_IF_FAILED(m_bitmapLock->GetStride(&localStride));

        RETURN_IF_FAILED(m_bitmapLock->GetDataPointer(&localPixelsSize, &localPixels));

        m_decodedFormat = format;
        m_pixels = localPixels;
        m_pixelsSize = localPixelsSize;
        m_stride = localStride;

    } else {
        // The already decoded format does not match the requested format
        // TODO: re-decoding the image would be possible if we create a 'release pixels pointer' method
        if (!IsEqualGUID(format, m_decodedFormat)) {
            return E_FAIL;
        }
    }

    *pixels = m_pixels;
    pixelBufferSize = m_pixelsSize;
    pixelStride = m_stride;

    return S_OK;
}

void Image::finalizeLoad()
{
    static const std::wstring widthId = L"width";
    static const std::wstring heightId = L"height";

    JsValueRef widthRef;
    EXIT_IF_JS_ERROR(JsIntToNumber(static_cast<int>(m_width), &widthRef));
    EXIT_IF_FAILED(ScriptHostUtilities::SetJsProperty(m_imageScriptReference, widthId, widthRef));

    JsValueRef heightRef;
    EXIT_IF_JS_ERROR(JsIntToNumber(static_cast<int>(m_height), &heightRef));
    EXIT_IF_FAILED(ScriptHostUtilities::SetJsProperty(m_imageScriptReference, heightId, heightRef));

    if (m_isLoaded) {
        m_eventTargetImplementation.invokeEventListeners(L"load");
    } else {
        m_eventTargetImplementation.invokeEventListeners(L"error");
    }

    JsRelease(m_imageScriptReference, nullptr);
    m_imageScriptReference = JS_INVALID_REFERENCE;
}

HRESULT Image::loadImageFromArray()
{
    Microsoft::WRL::ComPtr<IWICImagingFactory> imagingFactory;
    RETURN_IF_FAILED(
        CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, &imagingFactory));

    RETURN_IF_FAILED(imagingFactory->CreateStream(&m_imageStream));

    RETURN_IF_TRUE(m_imageArray->size() > UINT_MAX);
    RETURN_IF_FAILED(m_imageStream->InitializeFromMemory(m_imageArray->data(), static_cast<unsigned int>(m_imageArray->size())));

    return decodeImage(imagingFactory.Get());
}

HRESULT Image::loadImageFromBuffer(Windows::Storage::Streams::IBuffer ^ imageBuffer)
{
    Microsoft::WRL::ComPtr<Windows::Storage::Streams::IBufferByteAccess> bufferByteAccess;
    RETURN_IF_FAILED(reinterpret_cast<IInspectable*>(imageBuffer)->QueryInterface(IID_PPV_ARGS(&bufferByteAccess)));

    byte* imageNativeBuffer;
    RETURN_IF_FAILED(bufferByteAccess->Buffer(&imageNativeBuffer));

    Microsoft::WRL::ComPtr<IWICImagingFactory> imagingFactory;
    RETURN_IF_FAILED(
        CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, &imagingFactory));

    RETURN_IF_FAILED(imagingFactory->CreateStream(&m_imageStream));

    RETURN_IF_FAILED(m_imageStream->InitializeFromMemory(imageNativeBuffer, imageBuffer->Length));
    // Now that we initialized the image stream, keep a reference to the image buffer underneath it
    m_imageBuffer = imageBuffer;

    return decodeImage(imagingFactory.Get());
}

HRESULT Image::decodeImage(IWICImagingFactory* imagingFactory)
{
    RETURN_IF_FAILED(imagingFactory->CreateDecoderFromStream(
        m_imageStream.Get(), nullptr, WICDecodeMetadataCacheOnDemand, &m_decoder));

    unsigned int frameCount;
    RETURN_IF_FAILED(m_decoder->GetFrameCount(&frameCount));
    RETURN_IF_FALSE(frameCount > 0);

    Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> bitmapFrameDecode;

    RETURN_IF_FAILED(m_decoder->GetFrame(0, &bitmapFrameDecode));

    RETURN_IF_FAILED(bitmapFrameDecode.As(&m_bitmapSource));

    RETURN_IF_FAILED(m_bitmapSource->GetSize(&m_width, &m_height));
    RETURN_IF_FAILED(m_bitmapSource->GetPixelFormat(&m_sourceFormat));

    m_isLoaded = true;

    return S_OK;
}

IBuffer ^ Image::download(const std::wstring& source, HoloJs::AppModel::AppConfiguration configuration)
{
    Windows::Foundation::Uri ^ resolvedUri;

    if (isAbsoluteWebUri(source)) {
        resolvedUri = ref new Windows::Foundation::Uri(Platform::StringReference(source.c_str()));
    } else {
        wstring completeUrl = configuration.baseUri + L"/" + source;
        resolvedUri = ref new Windows::Foundation::Uri(Platform::StringReference(completeUrl.c_str()));
    }

    Windows::Web::Http::HttpClient ^ httpClient = ref new Windows::Web::Http::HttpClient();
    try {
        auto getAsync = httpClient->GetAsync(resolvedUri);
        create_task(getAsync).wait();

        auto responseMessage = getAsync->GetResults();
        RETURN_NULL_IF_FALSE(responseMessage->IsSuccessStatusCode);

        auto readAsync = responseMessage->Content->ReadAsBufferAsync();
        create_task(readAsync).wait();
        return readAsync->GetResults();
    } catch (...) {
    }

    return nullptr;
}

HRESULT Image::read(const std::wstring& source,
                    vector<unsigned char>& data,
                    HoloJs::AppModel::AppConfiguration configuration)
{
    RETURN_IF_TRUE(configuration.source != AppModel::AppSource::FileSystem);
    experimental::filesystem::path absolutePath(configuration.baseUri);
    absolutePath.append(source);

    RETURN_IF_TRUE(absolutePath.is_relative());

    return getPlatform()->readFileBinary(absolutePath, data);
}

HRESULT Image::readFromPackage(const std::wstring& source,
                               vector<unsigned char>& data,
                               HoloJs::AppModel::AppConfiguration configuration)
{
    return configuration.packageReader->readFileFromPackageBinary(source, data);
}