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
