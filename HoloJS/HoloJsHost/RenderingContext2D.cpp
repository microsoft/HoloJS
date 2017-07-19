#include "pch.h"
#include "RenderingContext2D.h"
#include "ImageElement.h"

#include "IBufferOnMemory.h"

using namespace Microsoft::Graphics::Canvas;
using namespace Windows::Graphics::DirectX;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Numerics;
using namespace Windows::Storage::Streams;
using namespace HologramJS::WebGL;

void RenderingContext2D::drawImage3(HologramJS::API::ImageElement *imageElement,
                                    GLsizei imageWidth,
                                    GLsizei imageHeight,
                                    unsigned int sx,
                                    unsigned int sy,
                                    unsigned int sWidth,
                                    unsigned int sHeight,
                                    unsigned int dx,
                                    unsigned int dy,
                                    unsigned int dWidth,
                                    unsigned int dHeight)
{
    m_isOptimizedBitmap = false;

    m_canvasRenderTarget = ref new CanvasRenderTarget(CanvasDevice::GetSharedDevice(),
                                                      static_cast<float>(m_width),
                                                      static_cast<float>(m_height),
                                                      96,
                                                      DirectXPixelFormat::R8G8B8A8UIntNormalized,
                                                      CanvasAlphaMode::Ignore);

    unsigned int imageBufferSize = 0;
    WICInProcPointer imageMemory = nullptr;
    unsigned int stride;
    EXIT_IF_FALSE(imageElement->GetPixelsPointer(&imageMemory, &imageBufferSize, &stride));

    Microsoft::WRL::ComPtr<HologramJS::Utilities::BufferOnMemory> imageBuffer;
    Microsoft::WRL::Details::MakeAndInitialize<HologramJS::Utilities::BufferOnMemory>(
        &imageBuffer, imageMemory, imageBufferSize);
    auto iinspectable = (IInspectable *)reinterpret_cast<IInspectable *>(imageBuffer.Get());
    IBuffer ^ imageIBuffer = reinterpret_cast<IBuffer ^>(iinspectable);

    auto canvasBitmap = CanvasBitmap::CreateFromBytes(CanvasDevice::GetSharedDevice(),
                                                      imageIBuffer,
                                                      imageElement->Width(),
                                                      imageElement->Height(),
                                                      DirectXPixelFormat::R8G8B8A8UIntNormalized);

    CanvasDrawingSession ^ session = m_canvasRenderTarget->CreateDrawingSession();

    Rect source(
        static_cast<float>(sx), static_cast<float>(sy), static_cast<float>(sWidth), static_cast<float>(sHeight));
    Rect dest(static_cast<float>(dx), static_cast<float>(dy), static_cast<float>(dWidth), static_cast<float>(dHeight));
    session->DrawImage(canvasBitmap, dest, source);
    session = nullptr;
}

void RenderingContext2D::drawImage1(HologramJS::API::ImageElement *imageElement,
                                    GLsizei imageWidth,
                                    GLsizei imageHeight,
                                    unsigned int dx,
                                    unsigned int dy)
{
    EXIT_IF_FALSE((imageWidth + dx <= m_width) && (imageHeight + dy <= m_height));

    unsigned int imageBufferSize = 0;
    WICInProcPointer imageMemory = nullptr;
    unsigned int stride;
    EXIT_IF_FALSE(imageElement->GetPixelsPointer(&imageMemory, &imageBufferSize, &stride));

    m_optimizedBitmap.resize(m_width * m_height * 4);
    m_isOptimizedBitmap = true;

    if (dx == 0 && dy == 0 && imageWidth == m_width && imageHeight == m_height && stride == (m_width * 4)) {
        CopyMemory(m_optimizedBitmap.data(), imageMemory, imageBufferSize);
    } else {
        unsigned int sourceLine = 0;
        unsigned int destinationLine = dy;
        const unsigned int destinationStride = m_width * 4;
        for (int i = 0; i < imageHeight; i++) {
            CopyMemory(m_optimizedBitmap.data() + destinationLine * destinationStride + dx,
                       imageMemory + sourceLine * stride,
                       stride);
            sourceLine++;
            destinationLine++;
        }
    }
}

void RenderingContext2D::drawImage2(HologramJS::API::ImageElement *imageElement,
                                    GLsizei imageWidth,
                                    GLsizei imageHeight,
                                    unsigned int dx,
                                    unsigned int dy,
                                    unsigned int dWidth,
                                    unsigned int dHeight)
{
    m_isOptimizedBitmap = false;

    m_canvasRenderTarget = ref new CanvasRenderTarget(CanvasDevice::GetSharedDevice(),
                                                      static_cast<float>(m_width),
                                                      static_cast<float>(m_height),
                                                      96,
                                                      DirectXPixelFormat::R8G8B8A8UIntNormalized,
                                                      CanvasAlphaMode::Ignore);

    unsigned int imageBufferSize = 0;
    WICInProcPointer imageMemory = nullptr;
    unsigned int stride;
    EXIT_IF_FALSE(imageElement->GetPixelsPointer(&imageMemory, &imageBufferSize, &stride));

    Microsoft::WRL::ComPtr<HologramJS::Utilities::BufferOnMemory> imageBuffer;
    Microsoft::WRL::Details::MakeAndInitialize<HologramJS::Utilities::BufferOnMemory>(
        &imageBuffer, imageMemory, imageBufferSize);
    auto iinspectable = (IInspectable *)reinterpret_cast<IInspectable *>(imageBuffer.Get());
    IBuffer ^ imageIBuffer = reinterpret_cast<IBuffer ^>(iinspectable);

    auto canvasBitmap = CanvasBitmap::CreateFromBytes(CanvasDevice::GetSharedDevice(),
                                                      imageIBuffer,
                                                      imageElement->Width(),
                                                      imageElement->Height(),
                                                      DirectXPixelFormat::R8G8B8A8UIntNormalized);

    auto session = m_canvasRenderTarget->CreateDrawingSession();

    Rect dest(static_cast<float>(dx), static_cast<float>(dy), static_cast<float>(dWidth), static_cast<float>(dHeight));
    session->DrawImage(canvasBitmap, dest);
    session = nullptr;
}

Platform::Array<unsigned char> ^ RenderingContext2D::getImageData(int sx, int sy, int sw, int sh)
{
    if (!m_isOptimizedBitmap) {
        return m_canvasRenderTarget->GetPixelBytes(sx, sy, sw, sh);
    } else {
        return nullptr;
    }
}
