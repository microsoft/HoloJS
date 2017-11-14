#include "pch.h"
#include "ImageElement.h"
#include "ExternalObject.h"
#include "IBufferOnMemory.h"
#include "ScriptHostUtilities.h"
#include "ScriptResourceTracker.h"
#include "ScriptsLoader.h"

using namespace HologramJS::Utilities;
using namespace HologramJS::API;
using namespace std;
using namespace concurrency;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Media::Capture;
using namespace Windows::Media::MediaProperties;
using namespace Windows::Storage::Streams;

bool ImageElement::UseFileSystem = false;
wstring ImageElement::BaseUrl = L"";
wstring ImageElement::BasePath = L"";
JsValueRef ImageElement::m_createImageFunction = JS_INVALID_REFERENCE;
JsValueRef ImageElement::m_setImageSourceFunction = JS_INVALID_REFERENCE;
JsValueRef ImageElement::m_setImageSourceFromBlobFunction = JS_INVALID_REFERENCE;
JsValueRef ImageElement::m_getImageDataFunction = JS_INVALID_REFERENCE;

ImageElement::ImageElement() {}

ImageElement::~ImageElement() {}

bool ImageElement::Initialize()
{
    RETURN_IF_FALSE(
        ScriptHostUtilities::ProjectFunction(L"createImage", L"image", createImage, nullptr, &m_createImageFunction));
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(
        L"setImageSource", L"image", setImageSource, nullptr, &m_setImageSourceFunction));
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(
        L"setImageSourceFromBlob", L"image", setImageSourceFromBlob, nullptr, &m_setImageSourceFromBlobFunction));
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(
        L"getImageData", L"image", getImageData, nullptr, &m_getImageDataFunction));

    return true;
}

_Use_decl_annotations_ JsValueRef CHAKRA_CALLBACK ImageElement::createImage(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    ExternalObject* externalObject = new ExternalObject();
    RETURN_INVALID_REF_IF_FALSE(externalObject->Initialize(new ImageElement()));
    return ScriptResourceTracker::ObjectToDirectExternal(externalObject);
}

_Use_decl_annotations_ JsValueRef CHAKRA_CALLBACK ImageElement::setImageSource(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);
    auto image = ScriptResourceTracker::ExternalToObject<ImageElement>(arguments[1]);
    RETURN_INVALID_REF_IF_NULL(image);

    RETURN_INVALID_REF_IF_FALSE(ScriptHostUtilities::GetString(arguments[2], image->m_source));

    image->LoadAsync();

    return JS_INVALID_REFERENCE;
}

_Use_decl_annotations_ JsValueRef CHAKRA_CALLBACK ImageElement::setImageSourceFromBlob(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    return JS_INVALID_REFERENCE;
}

_Use_decl_annotations_ JsValueRef CHAKRA_CALLBACK ImageElement::getImageData(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    return JS_INVALID_REFERENCE;
}

void ImageElement::LoadAsync()
{
    if (ScriptsLoader::IsAbsoluteWebUri(m_source) || !UseFileSystem) {
        DownloadAsync();
    } else if (_wcsicmp(m_source.c_str(), L"camera://local/default") == 0) {
        GetFromCameraAsync();
    } else {
        ReadFromPackageAsync();
    }
}

task<void> ImageElement::GetFromCameraAsync()
{
    auto mediaCapture = ref new MediaCapture();

    await mediaCapture->InitializeAsync();

    auto captureFormat = ImageEncodingProperties::CreateJpeg();

    auto captureStream = ref new InMemoryRandomAccessStream();

    // take photo
    await mediaCapture->CapturePhotoToStreamAsync(captureFormat, captureStream);
    captureStream->Seek(0);

    std::vector<byte> rawBuffer(static_cast<unsigned int>(captureStream->Size));

    Microsoft::WRL::ComPtr<HologramJS::Utilities::BufferOnMemory> imageBuffer;
    Microsoft::WRL::Details::MakeAndInitialize<HologramJS::Utilities::BufferOnMemory>(
        &imageBuffer, rawBuffer.data(), rawBuffer.size());
    auto iinspectable = (IInspectable*)reinterpret_cast<IInspectable*>(imageBuffer.Get());
    IBuffer ^ imageIBuffer = reinterpret_cast<IBuffer ^>(iinspectable);

    await captureStream->ReadAsync(
        imageIBuffer, static_cast<unsigned int>(captureStream->Size), InputStreamOptions::None);

    LoadImageFromBuffer(imageIBuffer);
}

task<void> ImageElement::ReadFromPackageAsync()
{
    wstring completePath = BasePath + m_source;

    auto imageBuffer = await HologramJS::Loaders::FileSystemLoader::ReadBinaryFromFileAsync(
        Windows::ApplicationModel::Package::Current->InstalledLocation, completePath);

    LoadImageFromBuffer(imageBuffer);
}

task<void> ImageElement::DownloadAsync()
{
    Windows::Foundation::Uri ^ uri;

    if (ScriptsLoader::IsAbsoluteWebUri(m_source)) {
        uri = ref new Windows::Foundation::Uri(Platform::StringReference(m_source.c_str()));
    } else {
        wstring completeUrl = BaseUrl + L"/" + m_source;
        uri = ref new Windows::Foundation::Uri(Platform::StringReference(completeUrl.c_str()));
    }

    Windows::Web::Http::HttpClient ^ httpClient = ref new Windows::Web::Http::HttpClient();

    auto responseMessage = await httpClient->GetAsync(uri);
    if (responseMessage->IsSuccessStatusCode) {
        Windows::Storage::Streams::IBuffer ^ responseBuffer = await responseMessage->Content->ReadAsBufferAsync();
        LoadImageFromBuffer(responseBuffer);
    }

    return;
}

void ImageElement::LoadImageFromBuffer(Windows::Storage::Streams::IBuffer ^ imageBuffer)
{
    Microsoft::WRL::ComPtr<IWICImagingFactory> imagingFactory;
    EXIT_IF_FAILED(
        CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, &imagingFactory));

    EXIT_IF_FAILED(imagingFactory->CreateStream(&m_imageStream));

    Microsoft::WRL::ComPtr<Windows::Storage::Streams::IBufferByteAccess> bufferByteAccess;
    EXIT_IF_FAILED(reinterpret_cast<IInspectable*>(imageBuffer)->QueryInterface(IID_PPV_ARGS(&bufferByteAccess)));

    byte* imageNativeBuffer;
    EXIT_IF_FAILED(bufferByteAccess->Buffer(&imageNativeBuffer));

    EXIT_IF_FAILED(m_imageStream->InitializeFromMemory(imageNativeBuffer, imageBuffer->Length));
    // Now that we initialized the image stream, keep a reference to the image buffer underneath it
    m_imageBuffer = imageBuffer;

    EXIT_IF_FAILED(imagingFactory->CreateDecoderFromStream(
        m_imageStream.Get(), nullptr, WICDecodeMetadataCacheOnDemand, &m_decoder));

    unsigned int frameCount;
    EXIT_IF_FAILED(m_decoder->GetFrameCount(&frameCount));
    EXIT_IF_FALSE(frameCount > 0);

    Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> bitmapFrameDecode;

    EXIT_IF_FAILED(m_decoder->GetFrame(0, &bitmapFrameDecode));

    EXIT_IF_FAILED(bitmapFrameDecode.As(&m_bitmapSource));

    EXIT_IF_FAILED(m_bitmapSource->GetSize(&m_width, &m_height));
    EXIT_IF_FAILED(m_bitmapSource->GetPixelFormat(&m_sourceFormat));

    FireOnLoadEvent();
}

void ImageElement::FireOnLoadEvent()
{
    if (HasCallback()) {
        vector<JsValueRef> parameters(4);

        parameters[0] = m_scriptCallbackContext;

        JsValueRef* typeParam = &parameters[1];
        EXIT_IF_JS_ERROR(JsPointerToString(L"load", wcslen(L"load"), typeParam));

        JsValueRef* widthParam = &parameters[2];
        EXIT_IF_JS_ERROR(JsIntToNumber(static_cast<int>(m_width), widthParam));

        JsValueRef* heightParam = &parameters[3];
        EXIT_IF_JS_ERROR(JsIntToNumber(static_cast<int>(m_height), heightParam));

        JsValueRef result;
        HANDLE_EXCEPTION_IF_JS_ERROR(JsCallFunction(
            m_scriptCallback, parameters.data(), static_cast<unsigned short>(parameters.size()), &result));
    }
}

HRESULT ImageElement::GetPixelsPointer(const GUID& format,
                                       WICInProcPointer* pixels,
                                       unsigned int* pixelsSize,
                                       unsigned int* stride,
                                       ImageFlipRotation flipRotation)
{
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
        if (flipRotation == ImageFlipRotation::FlipY) {
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
    *pixelsSize = m_pixelsSize;
    *stride = m_stride;

    return S_OK;
}
