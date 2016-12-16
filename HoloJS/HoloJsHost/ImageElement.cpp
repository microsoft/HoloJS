#include "pch.h"
#include "ScriptsLoader.h"
#include "ImageElement.h"

using namespace HologramJS::Utilities;
using namespace HologramJS::API;
using namespace std;
using namespace concurrency;
using namespace Windows::Graphics::Imaging;

bool ImageElement::UseFileSystem = false;
wstring ImageElement::BaseUrl = L"";
wstring ImageElement::BasePath = L"";
JsValueRef ImageElement::m_createImageFunction = JS_INVALID_REFERENCE;
JsValueRef ImageElement::m_setImageSourceFunction = JS_INVALID_REFERENCE;
JsValueRef ImageElement::m_setImageSourceFromBlobFunction = JS_INVALID_REFERENCE;
JsValueRef ImageElement::m_getImageDataFunction = JS_INVALID_REFERENCE;


ImageElement::ImageElement()
{
}


ImageElement::~ImageElement()
{
}

bool
ImageElement::Initialize()
{
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"createImage", L"image", createImage, nullptr, &m_createImageFunction));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"setImageSource", L"image", setImageSource, nullptr, &m_setImageSourceFunction));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"setImageSourceFromBlob", L"image", setImageSourceFromBlob, nullptr, &m_setImageSourceFromBlobFunction));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"getImageData", L"image", getImageData, nullptr, &m_getImageDataFunction));

	return true;
}

_Use_decl_annotations_
JsValueRef
CHAKRA_CALLBACK
ImageElement::createImage(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef *arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	ExternalObject* externalObject = new ExternalObject();
	RETURN_INVALID_REF_IF_FALSE(externalObject->Initialize(new ImageElement()));
	return ScriptResourceTracker::ObjectToDirectExternal(externalObject);
}

_Use_decl_annotations_
JsValueRef
CHAKRA_CALLBACK
ImageElement::setImageSource(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef *arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);
	auto image = ScriptResourceTracker::ExternalToObject<ImageElement>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(image);

	RETURN_INVALID_REF_IF_FALSE(ScriptHostUtilities::GetString(arguments[2], image->m_source));

	image->LoadAsync();

	return JS_INVALID_REFERENCE;
}

_Use_decl_annotations_
JsValueRef
CHAKRA_CALLBACK
ImageElement::setImageSourceFromBlob(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef *arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	return JS_INVALID_REFERENCE;
}

_Use_decl_annotations_
JsValueRef
CHAKRA_CALLBACK
ImageElement::getImageData(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef *arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	return JS_INVALID_REFERENCE;
}

void
ImageElement::LoadAsync()
{
	if ((_wcsnicmp(m_source.c_str(), L"http://", wcslen(L"http://")) == 0)
		|| (_wcsnicmp(m_source.c_str(), L"https://", wcslen(L"https://")) == 0)
		|| !UseFileSystem)
	{
		DownloadAsync();
	}
	else
	{
		ReadFromPackageAsync();
	}
}

task<void>
ImageElement::ReadFromPackageAsync()
{
	wstring completePath = BasePath + m_source;
	auto mainPathElements = HologramJS::ScriptsLoader::SplitPath(completePath);

	if (mainPathElements.size() == 0)
	{
		return;
	}

	auto fileName = mainPathElements.front();
	mainPathElements.erase(mainPathElements.begin());

	auto imageBuffer = await HologramJS::ScriptsLoader::ReadBinaryFromFile(
		Windows::ApplicationModel::Package::Current->InstalledLocation,
		mainPathElements,
		fileName);

	LoadImageFromBuffer(imageBuffer);
}

task<void>
ImageElement::DownloadAsync()
{
	Windows::Foundation::Uri^ uri;

	if ((_wcsnicmp(m_source.c_str(), L"http://", wcslen(L"http://")) == 0)
		|| (_wcsnicmp(m_source.c_str(), L"https://", wcslen(L"https://")) == 0))
	{
		uri = ref new Windows::Foundation::Uri(Platform::StringReference(m_source.c_str()));
	}
	else
	{
		wstring completeUrl = BaseUrl + L"/" + m_source;
		uri = ref new Windows::Foundation::Uri(Platform::StringReference(completeUrl.c_str()));
	}

	Windows::Web::Http::HttpClient^ httpClient = ref new  Windows::Web::Http::HttpClient();

	auto responseMessage = await httpClient->GetAsync(uri);
	if (responseMessage->IsSuccessStatusCode)
	{
		Windows::Storage::Streams::IBuffer^ responseBuffer = await responseMessage->Content->ReadAsBufferAsync();
		LoadImageFromBuffer(responseBuffer);
	}

	return;
}

void
ImageElement::LoadImageFromBuffer(
	Windows::Storage::Streams::IBuffer^ imageBuffer
)
{
	Microsoft::WRL::ComPtr<IWICImagingFactory> factory;
	EXIT_IF_FAILED(
		CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_IWICImagingFactory,
			&factory));

	EXIT_IF_FAILED(factory->CreateStream(&m_imageStream));

	Microsoft::WRL::ComPtr<Windows::Storage::Streams::IBufferByteAccess> bufferByteAccess;
	EXIT_IF_FAILED(reinterpret_cast<IInspectable*>(imageBuffer)->QueryInterface(IID_PPV_ARGS(&bufferByteAccess)));

	byte* imageNativeBuffer;
	EXIT_IF_FAILED(bufferByteAccess->Buffer(&imageNativeBuffer));

	EXIT_IF_FAILED(m_imageStream->InitializeFromMemory(imageNativeBuffer, imageBuffer->Length));

	EXIT_IF_FAILED(factory->CreateDecoderFromStream(m_imageStream.Get(), nullptr, WICDecodeMetadataCacheOnDemand, &m_decoder));

	unsigned int frameCount;
	EXIT_IF_FAILED(m_decoder->GetFrameCount(&frameCount));
	EXIT_IF_FALSE(frameCount > 0);

	Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> bitmapFrameDecode;

	EXIT_IF_FAILED(m_decoder->GetFrame(0, &bitmapFrameDecode));

	Microsoft::WRL::ComPtr<IWICBitmapSource> source;
	EXIT_IF_FAILED(bitmapFrameDecode.As(&source));

	EXIT_IF_FAILED(source->GetSize(&m_width, &m_height));

	WICPixelFormatGUID pixelFormat;
	EXIT_IF_FAILED(source->GetPixelFormat(&pixelFormat));

	Microsoft::WRL::ComPtr<IWICBitmapSource> converter;
	if (!IsEqualGUID(pixelFormat, GUID_WICPixelFormat32bppRGBA))
	{
		EXIT_IF_FAILED(WICConvertBitmapSource(GUID_WICPixelFormat32bppRGBA, source.Get(), &converter));
		EXIT_IF_FAILED(converter.As(&m_bitmapSource));
	}

	EXIT_IF_FAILED(factory->CreateBitmapFromSource(m_bitmapSource.Get(), WICBitmapNoCache, &m_bitmap));

	FireOnLoadEvent();
}

void
ImageElement::FireOnLoadEvent()
{
	if (HasCallback())
	{
		vector<JsValueRef> parameters(3);

		JsValueRef* typeParam = &parameters[0];
		EXIT_IF_JS_ERROR(JsPointerToString(L"load", wcslen(L"load"), typeParam));
		
		JsValueRef* widthParam= &parameters[1];
		EXIT_IF_JS_ERROR(JsIntToNumber(static_cast<int>(m_width), widthParam));

		JsValueRef* heightParam = &parameters[2];
		EXIT_IF_JS_ERROR(JsIntToNumber(static_cast<int>(m_height), heightParam));

		(void)InvokeCallback(parameters);
	}
}

bool
ImageElement::GetPixelsPointer(WICInProcPointer* pixels, unsigned int* pixelsSize, unsigned int* stride)
{
	if (!m_bitmapLock)
	{
		WICRect allBitmapRect;
		allBitmapRect.X = 0; allBitmapRect.Y = 0; allBitmapRect.Width = m_width; allBitmapRect.Height = m_height;
		RETURN_IF_FAILED(m_bitmap->Lock(&allBitmapRect, WICBitmapLockRead, &m_bitmapLock));

		WICInProcPointer localPixels;
		unsigned int localPixelsSize;
		unsigned int localStride;

		RETURN_IF_FAILED(m_bitmapLock->GetStride(&localStride));

		RETURN_IF_FAILED(m_bitmapLock->GetDataPointer(&localPixelsSize, &localPixels));

		*pixels = localPixels;
		*pixelsSize = localPixelsSize;
		*stride = localStride;
	}
	else
	{
		*pixels = m_pixels;
		*pixelsSize = m_pixelsSize;
		*stride = m_stride;
	}
	
	return true;
	
}