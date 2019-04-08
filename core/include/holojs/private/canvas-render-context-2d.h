#pragma once

#include "chakra.h"
#include "event-target.h"
#include "holojs-view.h"
#include "image-interface.h"
#include "object-lifetime.h"

namespace HoloJs {

class ICanvasRenderContext2D : public HoloJs::ResourceManagement::IRelease {
   public:
    virtual ~ICanvasRenderContext2D() {}

    virtual JsValueRef drawImage(HoloJs::IIMage* image, JsValueRef* arguments, unsigned short argumentCount) = 0;

    virtual JsValueRef clearRect(JsValueRef* arguments, unsigned short argumentCount) = 0;

    virtual JsValueRef fillRect(JsValueRef* arguments, unsigned short argumentCount) = 0;

    virtual JsValueRef fillRectGradient(JsValueRef* arguments, unsigned short argumentCount) = 0;

    virtual JsValueRef fillText(JsValueRef* arguments, unsigned short argumentCount) = 0;

    virtual JsValueRef measureText(JsValueRef* arguments, unsigned short argumentCount) = 0;

    virtual JsValueRef getImageData(JsValueRef* arguments, unsigned short argumentCount) = 0;

    virtual JsValueRef getWidth(JsValueRef* arguments, unsigned short argumentCount) = 0;

    virtual JsValueRef setWidth(JsValueRef* arguments, unsigned short argumentCount) = 0;

    virtual JsValueRef getHeight(JsValueRef* arguments, unsigned short argumentCount) = 0;

    virtual JsValueRef setHeight(JsValueRef* arguments, unsigned short argumentCount) = 0;

    virtual JsValueRef toDataUrl(JsValueRef* arguments, unsigned short argumentCount) = 0;

    virtual JsValueRef beginPath() = 0;

    virtual JsValueRef closePath() = 0;

    virtual JsValueRef moveTo(JsValueRef* arguments, unsigned short argumentCount) = 0;

    virtual JsValueRef lineTo(JsValueRef* arguments, unsigned short argumentCount) = 0;

    virtual JsValueRef bezierCurveTo(JsValueRef* arguments, unsigned short argumentCount) = 0;

    virtual JsValueRef quadraticCurveTo(JsValueRef* arguments, unsigned short argumentCount) = 0;

    virtual JsValueRef arc(JsValueRef* arguments, unsigned short argumentCount) = 0;

    virtual JsValueRef fill(JsValueRef* arguments, unsigned short argumentCount) = 0;

    virtual JsValueRef fillGradient(JsValueRef* arguments, unsigned short argumentCount) = 0;

    virtual JsValueRef stroke(JsValueRef* arguments, unsigned short argumentCount) = 0;

    virtual JsValueRef strokeGradient(JsValueRef* arguments, unsigned short argumentCount) = 0;

    virtual JsValueRef setTransform(JsValueRef* arguments, unsigned short argumentCount) = 0;

    virtual JsValueRef setLineStyle(JsValueRef* arguments, unsigned short argumentCount) = 0;

    virtual JsValueRef setGlobalOpacity(JsValueRef* arguments, unsigned short argumentCount) = 0;

    virtual long getImageDataNative(unsigned int x,
                                    unsigned int y,
                                    unsigned int width,
                                    unsigned int height,
                                    unsigned int& stride,
                                    std::vector<unsigned char>& data) = 0;
};

}  // namespace HoloJs