#pragma once

#include "IRelease.h"
#include "ImageElement.h"

namespace HologramJS {
namespace Canvas {
class RenderingContext2D : public HologramJS::Utilities::IRelease {
   public:
    RenderingContext2D();

    virtual ~RenderingContext2D() {}

    void Release() {}

    bool OptimizedBufferAvailable() { return m_isOptimizedBitmap; }
    size_t GetOptimizedBufferSize() { return m_optimizedBitmap.size(); }
    void CopyOptimizedBitmapToBuffer(byte* buffer)
    {
        CopyMemory(buffer, m_optimizedBitmap.data(), m_optimizedBitmap.size());
    }

    void drawImage(HologramJS::API::ImageElement* imageElement,
                   Windows::Foundation::Rect& srcRect,
                   Windows::Foundation::Rect& destRect);

    void clearRect(Windows::Foundation::Rect& rect);

    void fillRect(Windows::Foundation::Rect& rect, Windows::UI::Color& color);

    void fillRectGradient(Windows::Foundation::Rect& rect,
                          Windows::Foundation::Numerics::float2& start,
                          Windows::Foundation::Numerics::float2& end,
                          Platform::Array<Microsoft::Graphics::Canvas::Brushes::CanvasGradientStop> ^ stops);

    void fillText(std::wstring& text,
                  Windows::Foundation::Numerics::float2& point,
                  Windows::UI::Color& color,
                  int fontSize,
                  std::wstring& fontFamily);

    Platform::Array<unsigned char> ^ getImageData(Windows::Foundation::Rect& rect, unsigned int* stride);

    void setWidth(int value)
    {
        m_width = value;
        this->createRenderTarget();
    }

    int getWidth() { return m_width; }

    void setHeight(int value)
    {
        m_height = value;
        this->createRenderTarget();
    }

    int getHeight() { return m_height; }

    void createRenderTarget();

   private:
    unsigned int m_height = 150;
    unsigned int m_width = 300;

    std::vector<byte> m_optimizedBitmap;
    bool m_isOptimizedBitmap = false;

    Windows::Graphics::DirectX::DirectXPixelFormat m_nativePixelFormat =
        Windows::Graphics::DirectX::DirectXPixelFormat::R8G8B8A8UIntNormalized;
    unsigned int m_bpp = 4;

    Microsoft::Graphics::Canvas::CanvasRenderTarget ^ m_canvasRenderTarget = nullptr;
};
}  // namespace Canvas
}  // namespace HologramJS
