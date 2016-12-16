#pragma once
namespace HologramJS
{
	namespace API
	{
		class ImageElement : public HologramJS::Utilities::ElementWithEvents, public HologramJS::Utilities::IRelease
		{
		public:
			ImageElement();
			virtual ~ImageElement();

			virtual void Release() { }

			static bool Initialize();

			static bool UseFileSystem;
			static std::wstring BaseUrl;
			static std::wstring BasePath;

			IWICBitmapSource* GetBitmapSource() { return m_bitmapSource.Get(); }
			IWICBitmap* GetBitmap() { return m_bitmap.Get(); }
			bool GetPixelsPointer(WICInProcPointer* pixels, unsigned int* pixelsSize, unsigned int* stride);

			int Width() { return m_width; }
			int Height() { return m_height; }

		private:

			std::wstring m_source;

			void LoadAsync();

			concurrency::task<void> ReadFromPackageAsync();
			concurrency::task<void> DownloadAsync();

			void LoadImageFromBuffer(Windows::Storage::Streams::IBuffer^ imageStream);

			void FireOnLoadEvent();

			Windows::Storage::Streams::IBuffer^ m_imageBuffer;
			Microsoft::WRL::ComPtr<IWICStream> m_imageStream;
			Microsoft::WRL::ComPtr<IWICBitmapSource> m_bitmapSource;
			Microsoft::WRL::ComPtr<IWICBitmapDecoder> m_decoder;
			Microsoft::WRL::ComPtr<IWICBitmap> m_bitmap;
			Microsoft::WRL::ComPtr<IWICBitmapLock> m_bitmapLock;

			WICInProcPointer m_pixels;
			unsigned int m_pixelsSize;
			unsigned int m_stride;

			unsigned int m_width;
			unsigned int m_height;

			static JsValueRef m_createImageFunction;
			static JsValueRef CHAKRA_CALLBACK createImage(
				_In_ JsValueRef callee,
				_In_ bool isConstructCall,
				_In_ JsValueRef *arguments,
				_In_ unsigned short argumentCount,
				_In_ PVOID callbackData
			);

			static JsValueRef m_setImageSourceFunction;
			static JsValueRef CHAKRA_CALLBACK setImageSource(
				_In_ JsValueRef callee,
				_In_ bool isConstructCall,
				_In_ JsValueRef *arguments,
				_In_ unsigned short argumentCount,
				_In_ PVOID callbackData
			);

			static JsValueRef m_setImageSourceFromBlobFunction;
			static JsValueRef CHAKRA_CALLBACK setImageSourceFromBlob(
				_In_ JsValueRef callee,
				_In_ bool isConstructCall,
				_In_ JsValueRef *arguments,
				_In_ unsigned short argumentCount,
				_In_ PVOID callbackData
			);

			static JsValueRef m_getImageDataFunction;
			static JsValueRef CHAKRA_CALLBACK getImageData(
				_In_ JsValueRef callee,
				_In_ bool isConstructCall,
				_In_ JsValueRef *arguments,
				_In_ unsigned short argumentCount,
				_In_ PVOID callbackData
			);
		};
	}
}


