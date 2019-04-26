#pragma once

#include "holojs/private/holojs-script-host-internal.h"
#include <functional>
#include <string>
#include <vector>

namespace HoloJs {
namespace Platforms {
namespace Win32 {
class QrScanner {
   public:
    QrScanner(HoloJs::IHoloJsScriptHostInternal* host) : m_host(host) {}
    ~QrScanner()
    {
        // TODO: better way of cancelling an asynchronously running scan task
        while (m_isBusy) Sleep(15);
    }

    HRESULT tryDecode();

    void setOnResultsCallback(std::function<void(bool success, std::wstring text)> callback) { m_callback = callback; }

   private:
    std::function<void(bool success, std::wstring text)> m_callback;
    bool m_isBusy = false;

    HoloJs::IHoloJsScriptHostInternal* m_host;

    Windows::Media::MediaProperties::ImageEncodingProperties ^
        getEncodingProperties(Platform::Agile<Windows::Media::Capture::MediaCapture> mediaCapture);

    HRESULT readCaptureStreamAndGetBytes(Windows::Storage::Streams::InMemoryRandomAccessStream ^ captureStream,
                                         Windows::Storage::Streams::IBuffer ^* intermediateBuffer,
                                         unsigned char** captureBytes);

    Windows::Media::MediaProperties::MediaPixelFormat m_pixelFormat =
        Windows::Media::MediaProperties::MediaPixelFormat::Bgra8;

    int m_pixelFormatBpp = 4;
    int m_pixelFormatRedIndex = 2;
    int m_pixelFormatGreenIndex = 1;
    int m_pixelFormatBlueIndex = 0;
};

}  // namespace Win32
}  // namespace Platforms
}  // namespace HoloJs
