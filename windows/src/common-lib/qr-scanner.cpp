#include "stdafx.h"
#include "DecodeHints.h"
#include "GenericLuminanceSource.h"
#include "HybridBinarizer.h"
#include "MultiFormatReader.h"
#include "Result.h"
#include "holojs/private/error-handling.h"
#include "include/holojs/windows/qr-scanner.h"
#include <collection.h>
#include <functional>
#include <ppltasks.h>
#include <robuffer.h>
#include <windows.storage.streams.h>
#include <wrl.h>

using namespace concurrency;
using namespace HoloJs::Platforms::Win32;
using namespace std;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Media::Capture;
using namespace Windows::Media::MediaProperties;
using namespace Windows::Storage::Streams;

class scope_exit {
   public:
    scope_exit(const std::function<void()>& func) : m_func(func) {}

    void dismiss() {}

    virtual ~scope_exit()
    {
        try {
            if (!m_isDismissed) {
                m_func();
            }
        } catch (...) {
        }
    }

   private:
    bool m_isDismissed = false;
    std::function<void()> m_func;
};

ZXing::Result tryDecodeFromBuffer(unsigned int width,
                                  unsigned int height,
                                  unsigned char* buffer,
                                  int bpp,
                                  int redIndex,
                                  int greenIndex,
                                  int blueIndex)
{
    auto luminance = make_shared<ZXing::GenericLuminanceSource>(
        width, height, buffer, bpp * width, bpp, redIndex, greenIndex, blueIndex);

    auto binarizer = make_unique<ZXing::HybridBinarizer>(luminance);
    auto decodeHints = ZXing::DecodeHints();
    vector<ZXing::BarcodeFormat> formats = {ZXing::BarcodeFormat::QR_CODE};
    decodeHints.setPossibleFormats(formats);
    auto reader = make_unique<ZXing::MultiFormatReader>(decodeHints);
    return reader->read(*binarizer.get());
}

HRESULT QrScanner::tryDecode()
{
    RETURN_IF_TRUE(m_isBusy);
    m_isBusy = true;

    auto mediaCapture = ref new MediaCapture();

    auto initializeTask = create_task(mediaCapture->InitializeAsync());

    auto capturedMediaCapture = Platform::Agile<MediaCapture>(mediaCapture);

    initializeTask.then([this, capturedMediaCapture](task<void> previousTask) {
        auto autoClearIsBusy = scope_exit([this]() { m_isBusy = false; });
        auto autoFireFailedCallback = scope_exit([this]() { m_callback(false, L""); });

        try {
            previousTask.get();
        } catch (...) {
            return;
        }

        auto captureFormat = getEncodingProperties(capturedMediaCapture);
        EXIT_IF_TRUE(captureFormat == nullptr);

        const auto height = captureFormat->Height;
        const auto width = captureFormat->Width;

        auto captureStream = ref new InMemoryRandomAccessStream();
        IBuffer ^ imageBuffer = nullptr;

        for (int i = 0; i < 10; i++) {
            captureStream->Seek(0);

            // take photo
            auto captureTask =
                create_task(capturedMediaCapture->CapturePhotoToStreamAsync(captureFormat, captureStream));

            try {
                captureTask.wait();
            } catch (...) {
                return;
            }

            unsigned char* imageNativeBuffer;
            EXIT_IF_FAILED(readCaptureStreamAndGetBytes(captureStream, &imageBuffer, &imageNativeBuffer));

            auto result = tryDecodeFromBuffer(width,
                                              height,
                                              imageNativeBuffer,
                                              m_pixelFormatBpp,
                                              m_pixelFormatRedIndex,
                                              m_pixelFormatGreenIndex,
                                              m_pixelFormatBlueIndex);
            if (result.isValid()) {
                autoFireFailedCallback.dismiss();
                m_callback(true, result.text());
                break;
            }
        }
    });

    return S_OK;
}

ImageEncodingProperties ^ QrScanner::getEncodingProperties(Platform::Agile<MediaCapture> mediaCapture)
{
    auto captureFormat = ImageEncodingProperties::CreateUncompressed(m_pixelFormat);
    bool foundImageEncoding = false;

    auto streamProperties =
        mediaCapture->VideoDeviceController->GetAvailableMediaStreamProperties(MediaStreamType::Photo);
    for (unsigned int i = 0; i < streamProperties->Size; i++) {
        auto property = streamProperties->GetAt(i);
        if (_wcsicmp(property->Type->Data(), L"Image") == 0) {
            auto videoProperties = safe_cast<ImageEncodingProperties ^>(property);

            if (videoProperties->Height * videoProperties->Width > captureFormat->Height * captureFormat->Width) {
                captureFormat->Height = videoProperties->Height;
                captureFormat->Width = videoProperties->Width;
                foundImageEncoding = true;
            }
        }
    }

    return foundImageEncoding ? captureFormat : nullptr;
}

HRESULT QrScanner::readCaptureStreamAndGetBytes(InMemoryRandomAccessStream ^ captureStream,
                                                IBuffer ^* intermediateBuffer,
                                                unsigned char** captureBytes)
{
    captureStream->Seek(0);
    RETURN_IF_TRUE(captureStream->Size > UINT32_MAX);

    if (*intermediateBuffer == nullptr || (*intermediateBuffer)->Capacity < captureStream->Size) {
        *intermediateBuffer = ref new Buffer(static_cast<unsigned int>(captureStream->Size));
    }

    auto readAsync = captureStream->ReadAsync(
        *intermediateBuffer, static_cast<unsigned int>(captureStream->Size), InputStreamOptions::None);
    create_task(readAsync).wait();

    auto buffer = readAsync->GetResults();

    Microsoft::WRL::ComPtr<Windows::Storage::Streams::IBufferByteAccess> bufferByteAccess;
    RETURN_IF_FAILED(
        reinterpret_cast<IInspectable*>(*intermediateBuffer)->QueryInterface(IID_PPV_ARGS(&bufferByteAccess)));

    RETURN_IF_FAILED(bufferByteAccess->Buffer(captureBytes));

    return S_OK;
}