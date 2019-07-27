#pragma once

#include "blob-interface.h"
#include "chakra.h"
#include "event-target.h"
#include "holojs-view.h"
#include "object-lifetime.h"

namespace HoloJs {

typedef struct _IMAGE_FORMAT_GUID {
    DWORD Data1;
    WORD Data2;
    WORD Data3;
    BYTE Data4[8];
} IMAGE_FORMAT_GUID;

enum class ImageFlipRotation { None, FlipY };

class IImageData {
   public:
    IMAGE_FORMAT_GUID m_decodedFormat;

    byte* m_pixels;
    unsigned int m_pixelsSize;
    unsigned int m_stride;

	virtual ~IImageData() {};
};

class IIMage : public HoloJs::EventTarget, public HoloJs::ResourceManagement::IRelease {
   public:
    virtual ~IIMage() {}

    virtual long setSource(const std::wstring& source, JsValueRef imageRef) = 0;
    virtual long setSourceFromBlob(JsValueRef blobRef, IBlob* blob, JsValueRef imageRef) = 0;

    virtual long getImageData(const IMAGE_FORMAT_GUID& imageFormat,
                              IImageData** imageData,
                              ImageFlipRotation flipOperation) = 0;

    virtual unsigned int getWidth() = 0;
    virtual unsigned int getHeight() = 0;
};

}  // namespace HoloJs