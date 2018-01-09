#include "pch.h"
#include "RenderingContext2D.h"
#include "IBufferOnMemory.h"
#include "ImageElement.h"

using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::Graphics::Canvas::Brushes;
using namespace Microsoft::Graphics::Canvas::Text;
using namespace Windows::Graphics::DirectX;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Numerics;
using namespace Windows::Storage::Streams;
using namespace Windows::UI;
using namespace HologramJS::Canvas;
using namespace Microsoft::WRL;
using namespace Windows::Security::Cryptography;

RenderingContext2D::RenderingContext2D() { this->createRenderTarget(); }

void RenderingContext2D::createRenderTarget()
{
    m_canvasRenderTarget = ref new CanvasRenderTarget(CanvasDevice::GetSharedDevice(),
                                                      static_cast<float>(m_width),
                                                      static_cast<float>(m_height),
                                                      96,
                                                      m_nativePixelFormat,
                                                      CanvasAlphaMode::Ignore);
}

void RenderingContext2D::drawImage(HologramJS::API::ImageElement* imageElement, Rect& srcRect, Rect& destRect)
{
    m_isOptimizedBitmap = false;

    unsigned int imageBufferSize = 0;
    WICInProcPointer imageMemory = nullptr;
    unsigned int stride;

    imageElement->GetPixelsPointer(GUID_WICPixelFormat32bppRGBA, &imageMemory, &imageBufferSize, &stride);

    Microsoft::WRL::ComPtr<HologramJS::Utilities::BufferOnMemory> imageBuffer;
    Microsoft::WRL::Details::MakeAndInitialize<HologramJS::Utilities::BufferOnMemory>(
        &imageBuffer, imageMemory, imageBufferSize);
    auto iinspectable = (IInspectable*)reinterpret_cast<IInspectable*>(imageBuffer.Get());
    IBuffer ^ imageIBuffer = reinterpret_cast<IBuffer ^>(iinspectable);

    auto canvasBitmap = CanvasBitmap::CreateFromBytes(CanvasDevice::GetSharedDevice(),
                                                      imageIBuffer,
                                                      imageElement->Width(),
                                                      imageElement->Height(),
                                                      m_nativePixelFormat);

    CanvasDrawingSession ^ session = m_canvasRenderTarget->CreateDrawingSession();

    session->DrawImage(canvasBitmap, destRect, srcRect);
    session = nullptr;
}

void RenderingContext2D::clearRect(Rect& rect)
{
    CanvasDrawingSession ^ session = m_canvasRenderTarget->CreateDrawingSession();
    session->Blend = CanvasBlend::Copy;  // Overwrite everything!

    Color color;
    color.A = 0;
    color.R = 0;
    color.G = 0;
    color.B = 0;

    session->FillRectangle(rect, color);
    session = nullptr;
}

void RenderingContext2D::fillRect(Rect& rect, Color& color)
{
    CanvasDrawingSession ^ session = m_canvasRenderTarget->CreateDrawingSession();
    session->FillRectangle(rect, color);
    session = nullptr;
}

void RenderingContext2D::fillRectGradient(Rect& rect,
                                          float2& start,
                                          float2& end,
                                          Platform::Array<CanvasGradientStop> ^ stops)
{
    CanvasDrawingSession ^ session = m_canvasRenderTarget->CreateDrawingSession();
    auto brush = ref new CanvasLinearGradientBrush(m_canvasRenderTarget, stops);
    brush->StartPoint = start;
    brush->EndPoint = end;
    session->FillRectangle(rect, brush);
    session = nullptr;
}

void RenderingContext2D::fillText(
    std::wstring& text, float2& point, Color& color, int fontSize, std::wstring& fontFamily)
{
    CanvasDrawingSession ^ session = m_canvasRenderTarget->CreateDrawingSession();

    CanvasTextFormat ^ format = ref new CanvasTextFormat();
    format->FontFamily = ref new Platform::String(fontFamily.c_str());
    format->FontSize = fontSize;

    session->DrawText(ref new Platform::String(text.c_str()), point, color, format);
    session = nullptr;
}

Platform::Array<unsigned char> ^ RenderingContext2D::getImageData(Rect& rect, unsigned int* stride)
{
    if (!m_isOptimizedBitmap) {
        *stride = rect.Width * m_bpp;
        return m_canvasRenderTarget->GetPixelBytes(rect.X, rect.Y, rect.Width, rect.Height);
    } else {
        return nullptr;
    }
}

bool RenderingContext2D::toDataURL(const std::wstring& type, double encoderOptions, std::wstring* encodedImage)
{
    RETURN_IF_TRUE(m_isOptimizedBitmap);

    enum class EncodingType { PNG, JPEG, Unknown } encodingType = EncodingType::Unknown;

    if (_wcsicmp(type.c_str(), L"image/png") == 0) {
        encodingType = EncodingType::PNG;
    } else if (_wcsicmp(type.c_str(), L"image/jpeg") == 0) {
        encodingType = EncodingType::JPEG;
    }

    // Only PNG and Jpeg are supported
    RETURN_IF_TRUE(encodingType == EncodingType::Unknown);

    ComPtr<IWICImagingFactory> imagingFactory = NULL;
    RETURN_IF_FAILED(CoCreateInstance(CLSID_WICImagingFactory,
                                      NULL,
                                      CLSCTX_INPROC_SERVER,
                                      IID_IWICImagingFactory,
                                      (LPVOID*)imagingFactory.ReleaseAndGetAddressOf()));

    // Get the raw pixels from the underlying canvas
    auto canvasPixels = m_canvasRenderTarget->GetPixelBytes();
    auto canvasPixelsLength = canvasPixels->Length;
    auto canvasPixelData = canvasPixels->begin();

    // Convert from 32bpp RGBA to 24bpp BGR
    std::vector<byte> bgrPixels((canvasPixelsLength * 3) / 4);

    for (int i = 0, j = 0; i < canvasPixelsLength; i += 4, j += 3) {
        bgrPixels[j + 0] = canvasPixelData[i + 2];
        bgrPixels[j + 1] = canvasPixelData[i + 1];
        bgrPixels[j + 2] = canvasPixelData[i + 0];
    }

    // Create a memory stream to hold the encoded image
    ComPtr<IStream> memoryStream;
    HRESULT hr = ::CreateStreamOnHGlobal(nullptr, true /*delete on release*/, memoryStream.ReleaseAndGetAddressOf());
    
    // Create a WIC stream over our memory stream
    ComPtr<IWICStream> imageStream = NULL;
    RETURN_IF_FAILED(imagingFactory->CreateStream(imageStream.ReleaseAndGetAddressOf()));
    RETURN_IF_FAILED(imageStream->InitializeFromIStream(memoryStream.Get()));

    // Create the encoder corresponding to the requested type
    ComPtr<IWICBitmapEncoder> encoder = NULL;
    RETURN_IF_FAILED(imagingFactory->CreateEncoder(
        encodingType == EncodingType::JPEG ? GUID_ContainerFormatJpeg : GUID_ContainerFormatPng,
        NULL,
        encoder.ReleaseAndGetAddressOf()));

    RETURN_IF_FAILED(encoder->Initialize(imageStream.Get(), WICBitmapEncoderNoCache));

    ComPtr<IWICBitmapFrameEncode> bitmapFrame = NULL;
    ComPtr<IPropertyBag2> propertyBag = NULL;
    RETURN_IF_FAILED(
        encoder->CreateNewFrame(bitmapFrame.ReleaseAndGetAddressOf(), propertyBag.ReleaseAndGetAddressOf()));

    // If encoding to JPEG, set the image quality to what the caller requested;
    // PNG does not support image quality settings
    if (encodingType == EncodingType::JPEG) {
        PROPBAG2 option = {0};
        option.pstrName = L"ImageQuality";
        VARIANT varValue;
        VariantInit(&varValue);
        varValue.vt = VT_R4;
        varValue.fltVal = encoderOptions;
        RETURN_IF_FAILED(propertyBag->Write(1, &option, &varValue));
    }

    // Initialize the encoder
    RETURN_IF_FAILED(bitmapFrame->Initialize(propertyBag.Get()));
    RETURN_IF_FAILED(bitmapFrame->SetSize(m_canvasRenderTarget->Size.Width, m_canvasRenderTarget->Size.Height));

    // Set the input format to the encoder and make sure the format is acceptable
    WICPixelFormatGUID formatGUID = GUID_WICPixelFormat24bppBGR;
    RETURN_IF_FAILED(bitmapFrame->SetPixelFormat(&formatGUID));
    RETURN_IF_FALSE(IsEqualGUID(formatGUID, GUID_WICPixelFormat24bppBGR));

    // Write the canvas size to the encoder
    auto height = m_canvasRenderTarget->Size.Height;
    auto stride = bgrPixels.size() / m_canvasRenderTarget->Size.Height;
    hr = bitmapFrame->WritePixels(height, stride, bgrPixels.size(), bgrPixels.data());

    // Finalize the encoding operation
    RETURN_IF_FAILED(bitmapFrame->Commit());
    RETURN_IF_FAILED(encoder->Commit());

    // Figure out how long is the encoded stream
    LARGE_INTEGER seekSize;
    seekSize.QuadPart = 0;
    ULARGE_INTEGER streamLength;
    RETURN_IF_FAILED(imageStream->Seek(seekSize, STREAM_SEEK_END, &streamLength));

    // Get the underlying memory handle and pointer to the bits
    HGLOBAL streamMemHandle;
    RETURN_IF_FAILED(GetHGlobalFromStream(memoryStream.Get(), &streamMemHandle));
    byte* streamMemPtr = reinterpret_cast<byte*>(GlobalLock(streamMemHandle));
    RETURN_IF_NULL(streamMemPtr);

    // Create an IBuffer over the bits
    Microsoft::WRL::ComPtr<HologramJS::Utilities::BufferOnMemory> imageBuffer;
    Details::MakeAndInitialize<HologramJS::Utilities::BufferOnMemory>(
        &imageBuffer, streamMemPtr, static_cast<unsigned int>(streamLength.QuadPart));
    auto iinspectable = (IInspectable*)reinterpret_cast<IInspectable*>(imageBuffer.Get());
    IBuffer ^ imageIBuffer = reinterpret_cast<IBuffer ^>(iinspectable);

    // base64 encode the resulting image
    auto encodedImagePlatString = CryptographicBuffer::EncodeToBase64String(imageIBuffer);

    // Release the memory lock
    GlobalUnlock(streamMemHandle);

    // Create the dataURL
    encodedImage->reserve(encodedImagePlatString->Length() + 128);
    encodedImage->assign(L"data:");
    encodedImage->append(type);
    encodedImage->append(L";base64,");
    encodedImage->append(encodedImagePlatString->Data());

    return true;
}
