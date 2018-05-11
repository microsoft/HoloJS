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

    bool toDataURL(const std::wstring& type, double encoderOptions, std::wstring* encodedImage);

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
                  std::wstring& fontFamily,
                  std::wstring& fontWeight,
                  std::wstring& fontStyle,
                  int alignment

    );

    double measureText(std::wstring& text, int fontSize, std::wstring& fontFamily);

    void beginPath();
    void closePath();
    void moveTo(double x, double y);
    void lineTo(double x, double y);
    void bezierCurveTo(double c1x, double c1y, double c2x, double c2y, double x, double y);
    void quadraticCurveTo(double cx, double cy, double x, double y);
    void arc(double cx, double cy, double r, double startAngle, double endAngle, bool counterClockwise);

    void fill(Windows::UI::Color& color);
    void fillGradient(Windows::Foundation::Numerics::float2& start,
                      Windows::Foundation::Numerics::float2& end,
                      Platform::Array<Microsoft::Graphics::Canvas::Brushes::CanvasGradientStop> ^ stops);
    void stroke(Windows::UI::Color& color);
    void strokeGradient(Windows::Foundation::Numerics::float2& start,
                        Windows::Foundation::Numerics::float2& end,
                        Platform::Array<Microsoft::Graphics::Canvas::Brushes::CanvasGradientStop> ^ stops);

    void setTransform(double a, double b, double c, double d, double e, double f);
    void setGlobalOpacity(float opacity);
    void setLineStyle(float lineWidth,
                      Microsoft::Graphics::Canvas::Geometry::CanvasCapStyle capMode,
                      Microsoft::Graphics::Canvas::Geometry::CanvasLineJoin joinMode);

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
    Microsoft::Graphics::Canvas::CanvasDrawingSession ^ m_session = nullptr;

    float m_globalOpacity = 1.0;

    Microsoft::Graphics::Canvas::Geometry::CanvasPathBuilder ^ m_pathBuilder = nullptr;
    bool m_figurePresent = false;

    float lineWidth = 1;
    Microsoft::Graphics::Canvas::Geometry::CanvasCapStyle capStyle =
        Microsoft::Graphics::Canvas::Geometry::CanvasCapStyle::Round;
    Microsoft::Graphics::Canvas::Geometry::CanvasLineJoin joinStyle =
        Microsoft::Graphics::Canvas::Geometry::CanvasLineJoin::Round;

    enum class EncodingType { PNG, JPEG, Unknown };

    EncodingType getEncodingFromMimeType(const std::wstring& type);

    void getImageDataBGRFlipY(std::vector<byte>& bgrPixels);

    HRESULT getDataFromStream(IWICImagingFactory* imagingFactory, IStream* stream, std::vector<byte>& data);

    HRESULT initializeEncodingPropertyBag(IPropertyBag2* propertyBag, EncodingType encodingType, double encoderOptions);

    HRESULT getDataUrlFromEncodedImage(std::vector<byte>& imageData,
                                       const std::wstring& mimeType,
                                       std::wstring* encodedImage);
};
}  // namespace Canvas
}  // namespace HologramJS
