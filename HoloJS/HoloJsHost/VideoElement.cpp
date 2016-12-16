#include "pch.h"
#include "VideoElement.h"

using namespace HologramJS::API;
using namespace std;
using namespace HologramJS::Utilities;
using namespace concurrency;
using namespace Microsoft::WRL;

bool VideoElement::UseFileSystem = false;
wstring VideoElement::BaseUrl = L"";
wstring VideoElement::BasePath = L"";
JsValueRef VideoElement::m_createVideoFunction = JS_INVALID_REFERENCE;
JsValueRef VideoElement::m_setVideoSourceFunction = JS_INVALID_REFERENCE;

bool
VideoElement::Initialize()
{
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"createVideo", L"video", createVideo, nullptr, &m_createVideoFunction));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"setVideoSource", L"video", setVideoSource, nullptr, &m_setVideoSourceFunction));

	RETURN_IF_FAILED(MFStartup(MF_VERSION));

	return true;
}

_Use_decl_annotations_
JsValueRef
CHAKRA_CALLBACK
VideoElement::createVideo(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef *arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	ExternalObject* externalObject = new ExternalObject();
	RETURN_INVALID_REF_IF_FALSE(externalObject->Initialize(new VideoElement()));
	return ScriptResourceTracker::ObjectToDirectExternal(externalObject);
}

_Use_decl_annotations_
JsValueRef
CHAKRA_CALLBACK
VideoElement::setVideoSource(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef *arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);
	auto video = ScriptResourceTracker::ExternalToObject<VideoElement>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(video);

	RETURN_INVALID_REF_IF_FALSE(ScriptHostUtilities::GetString(arguments[2], video->m_source));

	video->LoadAsync();

	return JS_INVALID_REFERENCE;
}

task<bool>
VideoElement::LoadAsync()
{
	if ((_wcsnicmp(m_source.c_str(), L"http://", wcslen(L"http://")) == 0)
		|| (_wcsnicmp(m_source.c_str(), L"https://", wcslen(L"https://")) == 0)
		|| !UseFileSystem)
	{
        auto videoReaderConfigResult = await create_task([this]() -> bool { return ConfigureVideoReader(); });
		if (videoReaderConfigResult)
		{
			FireOnLoadEvent();

            create_task([this]() -> bool
            {
                m_stopStreamingRequested = false;
                m_isStreaming = true;
                return StreamAndDecode();
            });
		}
	}

	return false;
}

bool
VideoElement::ConfigureVideoReader()
{
	wstring source;

	if ((_wcsnicmp(m_source.c_str(), L"http://", wcslen(L"http://")) == 0)
		|| (_wcsnicmp(m_source.c_str(), L"https://", wcslen(L"https://")) == 0))
	{
		source = m_source;
	}
	else
	{
		source = BaseUrl + L"/" + m_source;
	}

    RETURN_IF_FALSE(InitializeHardwareDecoding());

	ComPtr<IMFAttributes> attributes;
	RETURN_IF_FAILED(MFCreateAttributes(&attributes, 1));
	RETURN_IF_FAILED(attributes->SetUINT32(MF_SOURCE_READER_ENABLE_ADVANCED_VIDEO_PROCESSING, 1));
    RETURN_IF_FAILED(attributes->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, 1));

    RETURN_IF_FAILED(MFCreateDXGIDeviceManager(&m_dxgiManagerResetToken, m_dxgiManager.ReleaseAndGetAddressOf()));

    ComPtr<IUnknown> deviceUnknown;
    RETURN_IF_FAILED(m_device.As(&deviceUnknown));
    RETURN_IF_FAILED(m_dxgiManager->ResetDevice(deviceUnknown.Get(), m_dxgiManagerResetToken));
    
    ComPtr<IUnknown> dxgiManagerUnknown;
    RETURN_IF_FAILED(m_dxgiManager.As(&dxgiManagerUnknown));
    RETURN_IF_FAILED(attributes->SetUnknown(MF_SOURCE_READER_D3D_MANAGER, dxgiManagerUnknown.Get()));

	RETURN_IF_FAILED(MFCreateSourceReaderFromURL(m_source.c_str(), attributes.Get(), m_videoReader.ReleaseAndGetAddressOf()));
	
	RETURN_IF_FAILED(ConfigureDecoder(m_videoReader.Get()));

	RETURN_IF_FAILED(m_videoReader->SetStreamSelection(MF_SOURCE_READER_FIRST_VIDEO_STREAM, true));

	ComPtr<IMFSourceReaderEx> readerEx;
	RETURN_IF_FAILED(m_videoReader.As(&readerEx));

	GUID transformType;
	DWORD transformIndex = 0;
	ComPtr<IMFVideoProcessorControl> videoProcessor;
	while (true)
	{
		ComPtr<IMFTransform> transform;
		const HRESULT hr = readerEx->GetTransformForStream(MF_SOURCE_READER_FIRST_VIDEO_STREAM, transformIndex, &transformType, &transform);
		RETURN_IF_FAILED(hr);
		if (transformType == MFT_CATEGORY_VIDEO_PROCESSOR)
		{
			RETURN_IF_FAILED(transform.As(&videoProcessor));
			break;
		}

		transformIndex++;
	}

	if (videoProcessor)
	{
		RETURN_IF_FAILED(videoProcessor->SetMirror(MIRROR_VERTICAL));
	}

	return true;
}

bool
VideoElement::ConfigureDecoder(
	IMFSourceReader* reader
)
{
	// Find the native format of the stream.
	ComPtr<IMFMediaType> nativeType;
	RETURN_IF_FAILED(reader->GetNativeMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, &nativeType));

	// Get the media type from the stream.

	RETURN_IF_FAILED(MFGetAttributeSize(nativeType.Get(), MF_MT_FRAME_SIZE, &m_width, &m_height));

	GUID majorType;
	RETURN_IF_FAILED(nativeType->GetGUID(MF_MT_MAJOR_TYPE, &majorType));
	RETURN_IF_FALSE(majorType == MFMediaType_Video);	

	// Create an uncompressed video output type
	ComPtr<IMFMediaType> desiredType;
	RETURN_IF_FAILED(MFCreateMediaType(&desiredType));
	RETURN_IF_FAILED(desiredType->SetGUID(MF_MT_MAJOR_TYPE, majorType));
	RETURN_IF_FAILED(desiredType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32));

	// Set the uncompressed format for source output
	RETURN_IF_FAILED(reader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, desiredType.Get()));

	return true;
}

bool
VideoElement::StreamAndDecode()
{
	std::function<void()> autoStopStreaming = [this]()  { m_isStreaming = false; };

	LONGLONG timestampMs;
	LONGLONG previousTimestampMs = 0;
	LONGLONG lastFrameTickCount = 0;

	DWORD streamIndex;
	DWORD streamFlags;
	LONGLONG timestamp100Ns;

	RETURN_IF_FAILED(
		m_videoReader->ReadSample(
			MF_SOURCE_READER_FIRST_VIDEO_STREAM,
			0,
			&streamIndex,
			&streamFlags,
			&timestamp100Ns,
			m_backSample.ReleaseAndGetAddressOf()));

    SwapBuffers();

	while (!m_stopStreamingRequested)
	{
		timestampMs = timestamp100Ns / 10000;
		LONGLONG waitTime = (timestampMs - previousTimestampMs) - (GetTickCount64() - lastFrameTickCount);
		if (waitTime > 0)
		{
			Sleep(static_cast<DWORD>(waitTime));
		}

		previousTimestampMs = timestampMs;
		lastFrameTickCount = GetTickCount64();

		RETURN_IF_FAILED(
			m_videoReader->ReadSample(
				MF_SOURCE_READER_FIRST_VIDEO_STREAM,
				0,
				&streamIndex,
				&streamFlags,
				&timestamp100Ns,
				m_backSample.ReleaseAndGetAddressOf()));

        SwapBuffers();
	}

	return true;
}

void
VideoElement::SwapBuffers()
{
    DWORD maxLength;
    if (SUCCEEDED(m_backSample->ConvertToContiguousBuffer(m_backMediaBuffer.ReleaseAndGetAddressOf()))
		&& m_backMediaBuffer
        && SUCCEEDED(m_backMediaBuffer->Lock(&m_backBufferMemory, &maxLength, &m_backBufferMemorySize)))
    {
        m_frameBufferLock.lock();

        m_newFrameAvailable = true;

        if (m_frontBufferMemory != nullptr)
        {
            m_frontMediaBuffer->Unlock();
            m_frontBufferMemory = nullptr;
            m_frontBufferMemorySize = 0;
        }

        m_frontSample.Swap(m_backSample);
        m_frontMediaBuffer.Swap(m_backMediaBuffer);

        byte* pointerTemp = m_backBufferMemory;
        m_backBufferMemory = m_frontBufferMemory;
        m_frontBufferMemory = pointerTemp;

        DWORD intTemp = m_backBufferMemorySize;
        m_backBufferMemorySize = m_frontBufferMemorySize;
        m_frontBufferMemorySize = intTemp;

        m_frameBufferLock.unlock();
    }
}

void
VideoElement::FireOnLoadEvent()
{
	if (HasCallback())
	{
		vector<JsValueRef> parameters(3);

		JsValueRef* typeParam = &parameters[0];
		EXIT_IF_JS_ERROR(JsPointerToString(L"load", wcslen(L"load"), typeParam));

		JsValueRef* widthParam = &parameters[1];
		EXIT_IF_JS_ERROR(JsIntToNumber(static_cast<int>(m_width), widthParam));

		JsValueRef* heightParam = &parameters[2];
		EXIT_IF_JS_ERROR(JsIntToNumber(static_cast<int>(m_height), heightParam));

		(void)InvokeCallback(parameters);
	}
}

bool
VideoElement::LockNextFrame(
	void** frameData,
	unsigned int* frameDataLength
)
{
	if (!m_newFrameAvailable)
	{
		return true;
	}

	m_frameBufferLock.lock();

	*frameData = m_frontBufferMemory;
	*frameDataLength = m_frontBufferMemorySize;

	return true;
}

bool
VideoElement::UnlockFrame()
{
	if (m_frontBufferMemory != nullptr)
	{
		m_newFrameAvailable = false;
		m_frameBufferLock.unlock();
	}

	return true;
}

bool
VideoElement::InitializeHardwareDecoding(
    )
{
    static const D3D_FEATURE_LEVEL levels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
    };

    D3D_FEATURE_LEVEL featureLevel;
    RETURN_IF_FAILED(
        D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            D3D11_CREATE_DEVICE_VIDEO_SUPPORT,
            levels,
            ARRAYSIZE(levels),
            D3D11_SDK_VERSION,
            m_device.ReleaseAndGetAddressOf(),
            &featureLevel,
            m_deviceContext.ReleaseAndGetAddressOf()
    ));

    ComPtr<ID3D10Multithread> multithread;
    RETURN_IF_FAILED(m_device.As(&multithread));
    multithread->SetMultithreadProtected(TRUE);

    return true;
}
