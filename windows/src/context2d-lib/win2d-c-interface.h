#pragma once
#include <windows.h>

STDAPI getRenderTarget(void* deviceUnknown,
                       float width,
                       float height,
                       float dpi,
                       Windows::Graphics::DirectX::DirectXPixelFormat pixelFormat,
                       Microsoft::Graphics::Canvas::CanvasAlphaMode alphaMode,
                       void** renderTargetUnknown);

STDAPI getSharedDevice(void** deviceUnknown);

STDAPI getCanvasTextFormat(void** canvasTextFormatUnknown);

STDAPI getCanvasStrokeStyle(void** canvasStrokeStyleUnknown);

STDAPI getCanvasLinearGradientBrush(void* rendererUnknown,
                                    Microsoft::Graphics::Canvas::Brushes::CanvasGradientStop* stops,
                                    unsigned int stopCount,
                                    void** canvasLinearGradientBrushUnknown);

STDAPI createCanvasBitmapFromBytes(void* rendererUnknown,
                                   unsigned int byteCount,
                                   unsigned char* bytes,
                                   int width,
                                   int height,
                                   Windows::Graphics::DirectX::DirectXPixelFormat pixelFormat,
                                   void** canvasBitmapUnknown);

STDAPI getCanvasPathBuilder(void* sessionUnknown, void** canvasPathBuilderUnknown);

STDAPI getCanvasTextLayout(void* sessionUnknown, const wchar_t* text, void* canvasTextFormatUnknown, float width, float height, void** canvasPathBuilderUnknown);

STDAPI canvasGeometryCreatePath(void* pathUnknown, void** canvasGeometryUnknown);