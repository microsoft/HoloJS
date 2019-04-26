#pragma once

#include "holojs/private/canvas-render-context-2d.h"
#include "holojs/private/image-interface.h"
#include <vector>
#include <wincodec.h>

namespace HoloJs {
namespace WindowsPlatform {

enum class EncodingType { PNG, JPEG, Unknown };

class RenderContext2D : public HoloJs::ICanvasRenderContext2D {
   public:
    RenderContext2D() {}
    virtual ~RenderContext2D() { cleanup(); }

    virtual void Release() { cleanup(); }

   private:
    HRESULT initialize();
    Microsoft::Graphics::Canvas::CanvasDevice ^ m_device;
    Microsoft::Graphics::Canvas::CanvasRenderTarget ^ m_renderer;
    void* m_rendererPtr;
    Microsoft::Graphics::Canvas::CanvasDrawingSession ^ m_session;

    unsigned int m_width = 0;
    unsigned int m_height = 0;
    float m_dpi = 96.0;
    unsigned int m_bpp = 4;
    bool m_isInitialized = false;
    Windows::Graphics::DirectX::DirectXPixelFormat m_nativePixelFormat =
        Windows::Graphics::DirectX::DirectXPixelFormat::R8G8B8A8UIntNormalized;

    float m_globalOpacity = 1.0;
    float m_lineWidth = 1;

    Microsoft::Graphics::Canvas::Geometry::CanvasPathBuilder ^ m_pathBuilder = nullptr;
    bool m_figurePresent = false;

    Microsoft::Graphics::Canvas::Geometry::CanvasCapStyle m_capStyle =
        Microsoft::Graphics::Canvas::Geometry::CanvasCapStyle::Round;
    Microsoft::Graphics::Canvas::Geometry::CanvasLineJoin m_joinStyle =
        Microsoft::Graphics::Canvas::Geometry::CanvasLineJoin::Round;

    void cleanup();

    void getImageDataBGRFlipY(std::vector<byte>& bgrPixels);
    void getImageDataBGRAUnPremultiplyFlipY(std::vector<byte>& bgrPixels);

    EncodingType getEncodingFromMimeType(const std::wstring& type);

    HRESULT getDataFromStream(IWICImagingFactory* imagingFactory, IStream* stream, std::vector<byte>& data);
    HRESULT getDataUrlFromEncodedImage(std::vector<byte>& imageData,
                                       const std::wstring& mimeType,
                                       std::wstring& encodedImage);

    HRESULT initializeEncodingPropertyBag(IPropertyBag2* propertyBag, EncodingType encodingType, float encoderOptions);

    virtual JsValueRef drawImage(HoloJs::IIMage* image, JsValueRef* arguments, unsigned short argumentCount);

    virtual JsValueRef clearRect(JsValueRef* arguments, unsigned short argumentCount);

    virtual JsValueRef fillRect(JsValueRef* arguments, unsigned short argumentCount);

    virtual JsValueRef fillRectGradient(JsValueRef* arguments, unsigned short argumentCount);

    virtual JsValueRef fillText(JsValueRef* arguments, unsigned short argumentCount);

    virtual JsValueRef measureText(JsValueRef* arguments, unsigned short argumentCount);

    virtual JsValueRef getImageData(JsValueRef* arguments, unsigned short argumentCount);

    virtual JsValueRef getWidth(JsValueRef* arguments, unsigned short argumentCount);

    virtual JsValueRef setWidth(JsValueRef* arguments, unsigned short argumentCount);

    virtual JsValueRef getHeight(JsValueRef* arguments, unsigned short argumentCount);

    virtual JsValueRef setHeight(JsValueRef* arguments, unsigned short argumentCount);

    virtual JsValueRef toDataUrl(JsValueRef* arguments, unsigned short argumentCount);

    virtual JsValueRef beginPath();

    virtual JsValueRef closePath();

    virtual JsValueRef moveTo(JsValueRef* arguments, unsigned short argumentCount);

    virtual JsValueRef lineTo(JsValueRef* arguments, unsigned short argumentCount);

    virtual JsValueRef bezierCurveTo(JsValueRef* arguments, unsigned short argumentCount);

    virtual JsValueRef quadraticCurveTo(JsValueRef* arguments, unsigned short argumentCount);

    virtual JsValueRef arc(JsValueRef* arguments, unsigned short argumentCount);

    virtual JsValueRef fill(JsValueRef* arguments, unsigned short argumentCount);

    virtual JsValueRef fillGradient(JsValueRef* arguments, unsigned short argumentCount);

    virtual JsValueRef stroke(JsValueRef* arguments, unsigned short argumentCount);

    virtual JsValueRef strokeGradient(JsValueRef* arguments, unsigned short argumentCount);

    virtual JsValueRef setTransform(JsValueRef* arguments, unsigned short argumentCount);

    virtual JsValueRef setLineStyle(JsValueRef* arguments, unsigned short argumentCount);

    virtual JsValueRef setGlobalOpacity(JsValueRef* arguments, unsigned short argumentCount);

    void moveToInternal(float x, float y);
    virtual long getImageDataNative(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned int& stride, std::vector<unsigned char>& data);
};
}  // namespace WindowsPlatform
}  // namespace HoloJs