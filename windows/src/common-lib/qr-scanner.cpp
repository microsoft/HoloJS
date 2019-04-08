#include "stdafx.h"
#include "DecodeHints.h"
#include "GenericLuminanceSource.h"
#include "HybridBinarizer.h"
#include "MultiFormatReader.h"
#include "Result.h"
#include "holojs/private/error-handling.h"
#include "include/holojs/windows/qr-scanner.h"
#include <collection.h>
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

HRESULT QrScanner::tryDecode()
{
    RETURN_IF_TRUE(m_isBusy);
    m_isBusy = true;

    auto mediaCapture = ref new MediaCapture();

    auto initializeTask = create_task(mediaCapture->InitializeAsync());

    auto capturedMediaCapture = Platform::Agile<MediaCapture>(mediaCapture);

    initializeTask.then([this, capturedMediaCapture](task<void> previousTask) {
        try {
            previousTask.get();
            auto captureFormat = ImageEncodingProperties::CreateUncompressed(MediaPixelFormat::Bgra8);
            auto captureStream = ref new InMemoryRandomAccessStream();

            auto streamProperties =
                capturedMediaCapture->VideoDeviceController->GetAvailableMediaStreamProperties(MediaStreamType::Photo);
            for (unsigned int i = 0; i < streamProperties->Size; i++) {
                auto property = streamProperties->GetAt(i);
                if (_wcsicmp(property->Type->Data(), L"Image") == 0) {
                    auto videoProperties = safe_cast<ImageEncodingProperties ^>(property);

                    if (videoProperties->Height * videoProperties->Width >
                        captureFormat->Height * captureFormat->Width) {
                        captureFormat->Height = videoProperties->Height;
                        captureFormat->Width = videoProperties->Width;
                    }
                }
            }

            // take photo
            auto captureTask =
                create_task(capturedMediaCapture->CapturePhotoToStreamAsync(captureFormat, captureStream));

            captureTask.then([this, captureStream, captureFormat](task<void> previousTask) {
                try {
                    previousTask.get();
                    captureStream->Seek(0);
                    EXIT_IF_TRUE(captureStream->Size > UINT32_MAX);

                    IBuffer ^ imageBuffer = ref new Buffer(static_cast<unsigned int>(captureStream->Size));
                    create_task(captureStream->ReadAsync(imageBuffer,
                                                         static_cast<unsigned int>(captureStream->Size),
                                                         InputStreamOptions::None))
                        .then([this, imageBuffer, captureFormat](IBuffer ^ buffer) {
                            auto height = captureFormat->Height;
                            auto width = captureFormat->Width;
                            Microsoft::WRL::ComPtr<Windows::Storage::Streams::IBufferByteAccess> bufferByteAccess;
                            EXIT_IF_FAILED(reinterpret_cast<IInspectable*>(imageBuffer)
                                               ->QueryInterface(IID_PPV_ARGS(&bufferByteAccess)));

                            unsigned char* imageNativeBuffer;
                            auto length = imageBuffer->Length;
                            EXIT_IF_FAILED(bufferByteAccess->Buffer(&imageNativeBuffer));

                            auto luminance = make_shared<ZXing::GenericLuminanceSource>(
                                width, height, imageNativeBuffer, 4 * width, 4, 2, 1, 0);
                            auto binarizer = make_unique<ZXing::HybridBinarizer>(luminance);
                            auto decodeHints = ZXing::DecodeHints();
                            decodeHints.setShouldTryHarder(true);
                            auto reader = make_unique<ZXing::MultiFormatReader>(decodeHints);
                            auto result = reader->read(*binarizer.get());
                            if (result.isValid()) {
                                m_callback(result.text());
                            }

                            m_isBusy = false;
                        });
                } catch (...) {
                }
            });
        } catch (...) {
        }
    });

    return S_OK;
}