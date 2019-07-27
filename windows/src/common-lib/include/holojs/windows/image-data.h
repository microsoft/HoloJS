#pragma once

#include "holojs/private/image-interface.h"
#include <wincodec.h>

namespace HoloJs {

class HoloJsScriptHost;

namespace Win32 {

class WindowsImageData : public HoloJs::IImageData {
   public:
    WindowsImageData(IWICBitmapLock* bitmapLock, IMAGE_FORMAT_GUID imageFormat)
    {
        m_bitmapLock.Attach(bitmapLock);
        m_bitmapLock->GetStride(&m_stride);
        m_bitmapLock->GetDataPointer(&m_pixelsSize, &m_pixels);
        m_decodedFormat = imageFormat;
    }

    virtual ~WindowsImageData() { m_bitmapLock.ReleaseAndGetAddressOf(); }

	private:
    Microsoft::WRL::ComPtr<IWICBitmapLock> m_bitmapLock;
};
}  // namespace Win32
}  // namespace HoloJs