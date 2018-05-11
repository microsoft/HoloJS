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
using namespace std;

RenderingContext2D::RenderingContext2D() { this->createRenderTarget(); }

void RenderingContext2D::createRenderTarget()
{
    // Create the render target
    m_canvasRenderTarget = ref new CanvasRenderTarget(CanvasDevice::GetSharedDevice(),
                                                      static_cast<float>(m_width),
                                                      static_cast<float>(m_height),
                                                      96,
                                                      m_nativePixelFormat,
                                                      CanvasAlphaMode::Premultiplied);
    // Create global drawing session
    m_session = m_canvasRenderTarget->CreateDrawingSession();
    m_session->Blend = CanvasBlend::SourceOver;
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

    m_session->DrawImage(canvasBitmap, destRect, srcRect);
}

void RenderingContext2D::clearRect(Rect& rect)
{
    // Save the previous blend value
    CanvasBlend blend = m_session->Blend;

    m_session->Blend = CanvasBlend::Copy;

    Color color;
    color.A = 0;
    color.R = 0;
    color.G = 0;
    color.B = 0;

    m_session->FillRectangle(rect, color);

    m_session->Blend = blend;
}

void RenderingContext2D::fillRect(Rect& rect, Color& color)
{
    Color cm = color;
    cm.A *= m_globalOpacity;
    m_session->FillRectangle(rect, cm);
}

void RenderingContext2D::fillRectGradient(Rect& rect,
                                          float2& start,
                                          float2& end,
                                          Platform::Array<CanvasGradientStop> ^ stops)
{
    auto brush = ref new CanvasLinearGradientBrush(m_canvasRenderTarget, stops);
    brush->StartPoint = start;
    brush->EndPoint = end;
    m_session->FillRectangle(rect, brush);
}

void RenderingContext2D::fillText(std::wstring& text,
                                  float2& point,
                                  Color& color,
                                  int fontSize,
                                  std::wstring& fontFamily,
                                  std::wstring& fontWeight,
                                  std::wstring& fontStyle,
                                  int alignment)
{
    CanvasTextFormat ^ format = ref new CanvasTextFormat();
    format->FontFamily = ref new Platform::String(fontFamily.c_str());
    if (fontWeight == L"bold") {
        format->FontWeight = Windows::UI::Text::FontWeights::Bold;
    }
    if (fontStyle == L"italic") {
        format->FontStyle = Windows::UI::Text::FontStyle::Italic;
    }
    format->FontSize = fontSize;
    if (alignment == 0) {
        format->HorizontalAlignment = CanvasHorizontalAlignment::Left;
    }
    if (alignment == 1) {
        format->HorizontalAlignment = CanvasHorizontalAlignment::Center;
    }
    if (alignment == 2) {
        format->HorizontalAlignment = CanvasHorizontalAlignment::Right;
    }
    format->VerticalAlignment = CanvasVerticalAlignment::Bottom;
    format->LineSpacing = 1;

    Color cm = color;
    cm.A *= m_globalOpacity;
    m_session->DrawText(ref new Platform::String(text.c_str()), point, cm, format);
}

double RenderingContext2D::measureText(std::wstring& text, int fontSize, std::wstring& fontFamily)
{
    CanvasTextFormat ^ format = ref new CanvasTextFormat();
    format->FontFamily = ref new Platform::String(fontFamily.c_str());
    format->FontSize = fontSize;
    format->WordWrapping = Microsoft::Graphics::Canvas::Text::CanvasWordWrapping::NoWrap;
    CanvasTextLayout ^ layout =
        ref new CanvasTextLayout(m_session, ref new Platform::String(text.c_str()), format, 0, 0);
    return layout->DrawBounds.Width;
}

void RenderingContext2D::beginPath()
{
    m_pathBuilder = ref new Geometry::CanvasPathBuilder(m_session);
    m_figurePresent = false;
}

void RenderingContext2D::closePath()
{
    if (m_pathBuilder != nullptr && m_figurePresent) {
        m_pathBuilder->EndFigure(Geometry::CanvasFigureLoop::Closed);
        m_figurePresent = false;
    }
}

float2 transformPoint(float3x2& mat, float2 pt)
{
    return float2(mat.m11 * pt.x + mat.m21 * pt.y + mat.m31, mat.m12 * pt.x + mat.m22 * pt.y + mat.m32);
}

void RenderingContext2D::moveTo(double x, double y)
{
    if (m_pathBuilder == nullptr) beginPath();
    if (m_figurePresent) {
        m_pathBuilder->EndFigure(Geometry::CanvasFigureLoop::Open);
    }
    m_pathBuilder->BeginFigure(transformPoint(m_session->Transform, float2(x, y)));
    m_figurePresent = true;
}

void RenderingContext2D::lineTo(double x, double y)
{
    if (m_pathBuilder == nullptr) beginPath();
    if (!m_figurePresent) {
        moveTo(x, y);
    } else {
        m_pathBuilder->AddLine(transformPoint(m_session->Transform, float2(x, y)));
    }
}

void RenderingContext2D::quadraticCurveTo(double cx, double cy, double x, double y)
{
    if (m_pathBuilder == nullptr) beginPath();
    if (!m_figurePresent) {
        moveTo(x, y);
    } else {
        m_pathBuilder->AddQuadraticBezier(transformPoint(m_session->Transform, float2(cx, cy)),
                                          transformPoint(m_session->Transform, float2(x, y)));
    }
}

void RenderingContext2D::bezierCurveTo(double c1x, double c1y, double c2x, double c2y, double x, double y)
{
    if (m_pathBuilder == nullptr) beginPath();
    if (!m_figurePresent) {
        moveTo(x, y);
    } else {
        m_pathBuilder->AddCubicBezier(transformPoint(m_session->Transform, float2(c1x, c1y)),
                                      transformPoint(m_session->Transform, float2(c2x, c2y)),
                                      transformPoint(m_session->Transform, float2(x, y)));
    }
}

void RenderingContext2D::arc(double cx, double cy, double r, double startAngle, double endAngle, bool counterClockwise)
{
    if (m_pathBuilder == nullptr) beginPath();
    if (!m_figurePresent) {
        moveTo(cx, cy);
    }
    float tr = r * m_session->Transform.m11;
    m_pathBuilder->AddArc(
        transformPoint(m_session->Transform, float2(cx, cy)), tr, tr, startAngle, endAngle - startAngle);
}

void RenderingContext2D::fill(Color& color)
{
    if (m_pathBuilder == nullptr) return;
    if (m_figurePresent) {
        m_pathBuilder->EndFigure(Geometry::CanvasFigureLoop::Open);
        m_figurePresent = false;
    }

    float3x2 tmp = m_session->Transform;
    m_session->Transform = float3x2::identity();

    Color cm = color;
    cm.A *= m_globalOpacity;

    Geometry::CanvasGeometry ^ geometry = Geometry::CanvasGeometry::CreatePath(m_pathBuilder);
    m_session->FillGeometry(geometry, cm);

    m_session->Transform = tmp;

    beginPath();
    m_pathBuilder->AddGeometry(geometry);
}

void RenderingContext2D::fillGradient(float2& start, float2& end, Platform::Array<CanvasGradientStop> ^ stops)
{
    if (m_pathBuilder == nullptr) return;
    if (m_figurePresent) {
        m_pathBuilder->EndFigure(Geometry::CanvasFigureLoop::Open);
        m_figurePresent = false;
    }

    float3x2 tmp = m_session->Transform;
    m_session->Transform = float3x2::identity();

    Geometry::CanvasGeometry ^ geometry = Geometry::CanvasGeometry::CreatePath(m_pathBuilder);

    auto brush = ref new CanvasLinearGradientBrush(m_canvasRenderTarget, stops);
    brush->StartPoint = start;
    brush->EndPoint = end;
    m_session->FillGeometry(geometry, brush);

    m_session->Transform = tmp;

    beginPath();
    m_pathBuilder->AddGeometry(geometry);
}

void RenderingContext2D::stroke(Color& color)
{
    if (m_pathBuilder == nullptr) return;
    if (m_figurePresent) {
        m_pathBuilder->EndFigure(Geometry::CanvasFigureLoop::Open);
        m_figurePresent = false;
    }

    float3x2 tmp = m_session->Transform;
    m_session->Transform = float3x2::identity();

    Geometry::CanvasGeometry ^ geometry = Geometry::CanvasGeometry::CreatePath(m_pathBuilder);

    Geometry::CanvasStrokeStyle ^ style = ref new Geometry::CanvasStrokeStyle();
    style->StartCap = capStyle;
    style->EndCap = capStyle;
    style->LineJoin = joinStyle;

    Color cm = color;
    cm.A *= m_globalOpacity;

    m_session->DrawGeometry(geometry, cm, lineWidth * tmp.m11, style);

    m_session->Transform = tmp;

    beginPath();
    m_pathBuilder->AddGeometry(geometry);
}

void RenderingContext2D::strokeGradient(float2& start, float2& end, Platform::Array<CanvasGradientStop> ^ stops)
{
    if (m_pathBuilder == nullptr) return;
    if (m_figurePresent) {
        m_pathBuilder->EndFigure(Geometry::CanvasFigureLoop::Open);
        m_figurePresent = false;
    }

    float3x2 tmp = m_session->Transform;
    m_session->Transform = float3x2::identity();

    Geometry::CanvasGeometry ^ geometry = Geometry::CanvasGeometry::CreatePath(m_pathBuilder);

    auto brush = ref new CanvasLinearGradientBrush(m_canvasRenderTarget, stops);
    brush->StartPoint = start;
    brush->EndPoint = end;
    m_session->DrawGeometry(geometry, brush);

    m_session->Transform = tmp;

    beginPath();
    m_pathBuilder->AddGeometry(geometry);
}

void RenderingContext2D::setLineStyle(float _lineWidth,
                                      Geometry::CanvasCapStyle capMode,
                                      Geometry::CanvasLineJoin joinMode)
{
    lineWidth = _lineWidth;
    capStyle = capMode;
    joinStyle = joinMode;
}

void RenderingContext2D::setTransform(double a, double b, double c, double d, double e, double f)
{
    m_session->Transform = float3x2(a, b, c, d, e, f);
}

void RenderingContext2D::setGlobalOpacity(float opacity) { m_globalOpacity = opacity; }

Platform::Array<unsigned char> ^ RenderingContext2D::getImageData(Rect& rect, unsigned int* stride)
{
    m_session->Flush();
    if (!m_isOptimizedBitmap) {
        *stride = rect.Width * m_bpp;
        return m_canvasRenderTarget->GetPixelBytes(rect.X, rect.Y, rect.Width, rect.Height);
    } else {
        return nullptr;
    }
}

RenderingContext2D::EncodingType RenderingContext2D::getEncodingFromMimeType(const wstring& type)
{
    if (_wcsicmp(type.c_str(), L"image/png") == 0) {
        return EncodingType::PNG;
    } else if (_wcsicmp(type.c_str(), L"image/jpeg") == 0) {
        return EncodingType::JPEG;
    } else {
        return EncodingType::Unknown;
    }
}

void RenderingContext2D::getImageDataBGRFlipY(vector<byte>& bgrPixels)
{
    // Get the raw pixels from the underlying canvas
    auto canvasPixels = m_canvasRenderTarget->GetPixelBytes();
    auto canvasPixelsLength = canvasPixels->Length;
    auto canvasPixelData = canvasPixels->begin();

    const int dest_bpp = 4;
    bgrPixels.resize((canvasPixelsLength * dest_bpp) / m_bpp);

    // Convert from 32bpp RGBA to 24bpp BGR and flip vertical
    for (int row_index = 0; row_index < m_height; row_index++) {
        for (int column_index = 0; column_index < m_width; column_index++) {
            const auto destination_row_offset = row_index * m_width * dest_bpp;
            const auto source_row_offset = (row_index)*m_width * m_bpp;

            unsigned char alpha = canvasPixelData[source_row_offset + column_index * m_bpp + 3];
            // un-premultiply the alpha values, because the image encoders wants un-premultiplied alpha.
            float s = alpha > 0 ? 255.0 / alpha : 0;
            bgrPixels[destination_row_offset + column_index * dest_bpp + 0] =
                canvasPixelData[source_row_offset + column_index * m_bpp + 2] * s;
            bgrPixels[destination_row_offset + column_index * dest_bpp + 1] =
                canvasPixelData[source_row_offset + column_index * m_bpp + 1] * s;
            bgrPixels[destination_row_offset + column_index * dest_bpp + 2] =
                canvasPixelData[source_row_offset + column_index * m_bpp + 0] * s;
            bgrPixels[destination_row_offset + column_index * dest_bpp + 3] = alpha;
        }
    }
}

HRESULT RenderingContext2D::getDataFromStream(IWICImagingFactory* imagingFactory, IStream* stream, vector<byte>& data)
{
    // Figure out long is the encoded stream
    LARGE_INTEGER seekSize;
    seekSize.QuadPart = 0;
    ULARGE_INTEGER streamLength;
    RETURN_IF_FAILED(stream->Seek(seekSize, STREAM_SEEK_CUR, &streamLength));

    // Allocate a memory block to copy the encoded image stream to
    data.resize(streamLength.QuadPart);

    // Create a WIC stream over the memory block
    ComPtr<IWICStream> byteAccessWicStream = NULL;
    RETURN_IF_FAILED(imagingFactory->CreateStream(byteAccessWicStream.ReleaseAndGetAddressOf()));
    RETURN_IF_FAILED(byteAccessWicStream->InitializeFromMemory(data.data(), streamLength.QuadPart));

    // Get the vanilla stream from the WIC stream
    ComPtr<IStream> byteAccessStream;
    RETURN_IF_FAILED(byteAccessWicStream.As(&byteAccessStream));

    // Copy the encoded image stream to the byte accessible stream
    RETURN_IF_FAILED(stream->Seek(seekSize, STREAM_SEEK_SET, nullptr));
    RETURN_IF_FAILED(stream->CopyTo(byteAccessStream.Get(), streamLength, nullptr, nullptr));

    return S_OK;
}

HRESULT RenderingContext2D::getDataUrlFromEncodedImage(vector<byte>& imageData,
                                                       const wstring& mimeType,
                                                       wstring* encodedImage)
{
    // Create an IBuffer over the image memory block
    Microsoft::WRL::ComPtr<HologramJS::Utilities::BufferOnMemory> imageBuffer;
    Details::MakeAndInitialize<HologramJS::Utilities::BufferOnMemory>(
        &imageBuffer, imageData.data(), static_cast<unsigned int>(imageData.size()));
    auto iinspectable = (IInspectable*)reinterpret_cast<IInspectable*>(imageBuffer.Get());
    IBuffer ^ imageIBuffer = reinterpret_cast<IBuffer ^>(iinspectable);

    // base64 encode the resulting image
    auto encodedImagePlatString = CryptographicBuffer::EncodeToBase64String(imageIBuffer);

    // Create the dataURL
    encodedImage->reserve(encodedImagePlatString->Length() + 128);
    encodedImage->assign(L"data:");
    encodedImage->append(mimeType);
    encodedImage->append(L";base64,");
    encodedImage->append(encodedImagePlatString->Data());

    return S_OK;
}

HRESULT RenderingContext2D::initializeEncodingPropertyBag(IPropertyBag2* propertyBag,
                                                          EncodingType encodingType,
                                                          double encoderOptions)
{
    // If encoding to JPEG, set the image quality to what the caller requested;
    // PNG does not support image quality settings
    if (encodingType == EncodingType::JPEG) {
        PROPBAG2 qualityOption = {0};
        qualityOption.pstrName = L"ImageQuality";
        VARIANT qualityValue;
        VariantInit(&qualityValue);
        qualityValue.vt = VT_R4;
        qualityValue.fltVal = encoderOptions;
        RETURN_IF_FAILED(propertyBag->Write(1, &qualityOption, &qualityValue));
    }

    return S_OK;
}

bool RenderingContext2D::toDataURL(const std::wstring& type, double encoderOptions, std::wstring* encodedImage)
{
    m_session->Flush();

    RETURN_IF_TRUE(m_isOptimizedBitmap);

    const auto encodingType = getEncodingFromMimeType(type);

    // Only PNG and Jpeg are supported
    RETURN_IF_TRUE(encodingType == EncodingType::Unknown);

    // Convert from 32bpp RGBA to 24bpp BGR
    std::vector<byte> bgrPixels;
    getImageDataBGRFlipY(bgrPixels);

    ComPtr<IWICImagingFactory> imagingFactory = NULL;
    RETURN_IF_FAILED(CoCreateInstance(CLSID_WICImagingFactory,
                                      NULL,
                                      CLSCTX_INPROC_SERVER,
                                      IID_IWICImagingFactory,
                                      (LPVOID*)imagingFactory.ReleaseAndGetAddressOf()));

    // Create a memory stream to hold the encoded image, then wrap a WIC stream around it
    ComPtr<IStream> memoryStream;
    RETURN_IF_FAILED(
        ::CreateStreamOnHGlobal(nullptr, true /*delete on release*/, memoryStream.ReleaseAndGetAddressOf()));
    ComPtr<IWICStream> imageStream;
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

    RETURN_IF_FAILED(initializeEncodingPropertyBag(propertyBag.Get(), encodingType, encoderOptions));

    // Initialize the encoder
    RETURN_IF_FAILED(bitmapFrame->Initialize(propertyBag.Get()));
    RETURN_IF_FAILED(bitmapFrame->SetSize(m_canvasRenderTarget->Size.Width, m_canvasRenderTarget->Size.Height));

    // Set the input format to the encoder and make sure the format is acceptable
    WICPixelFormatGUID formatGUID = GUID_WICPixelFormat32bppBGRA;
    RETURN_IF_FAILED(bitmapFrame->SetPixelFormat(&formatGUID));
    RETURN_IF_FALSE(IsEqualGUID(formatGUID, GUID_WICPixelFormat32bppBGRA));

    // Write the canvas pixels to the encoder
    auto height = m_canvasRenderTarget->Size.Height;
    auto stride = bgrPixels.size() / m_canvasRenderTarget->Size.Height;
    RETURN_IF_FAILED(bitmapFrame->WritePixels(height, stride, bgrPixels.size(), bgrPixels.data()));

    // Finalize the encoding operation
    RETURN_IF_FAILED(bitmapFrame->Commit());
    RETURN_IF_FAILED(encoder->Commit());

    // Get the encoded memory block from the stream
    // TODO: when we can use GlobalLock/Unlock (RS2+), access the memoryStream directly
    vector<byte> encodedMemoryBlock;
    RETURN_IF_FAILED(getDataFromStream(imagingFactory.Get(), memoryStream.Get(), encodedMemoryBlock));

    RETURN_IF_FAILED(getDataUrlFromEncodedImage(encodedMemoryBlock, type, encodedImage));

    return true;
}
