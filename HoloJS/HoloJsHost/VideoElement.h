#pragma once

namespace HologramJS
{
	namespace API
	{
		class VideoElement : public HologramJS::Utilities::ElementWithEvents, public HologramJS::Utilities::IRelease
		{
		public:
			VideoElement() {}

			virtual ~VideoElement()
			{
				Release();
			}

			virtual void Release()
			{
				UnlockFrame();
				m_stopStreamingRequested = true;
				while (m_isStreaming)
				{
					Sleep(100);
				}
			}

			static bool Initialize();

			static bool UseFileSystem;
			static std::wstring BaseUrl;
			static std::wstring BasePath;

			int Width() { return m_width; }
			int Height() { return m_height; }

            bool IsNewFrameAvailable() { return m_newFrameAvailable; };
			bool LockNextFrame(void** frameData, unsigned int* frameDataLength);
			bool UnlockFrame();

		private:

			std::wstring m_source;

			concurrency::task<bool> LoadAsync();
			bool ConfigureVideoReader();
			bool StreamAndDecode();
			bool m_isStreaming;
			bool m_stopStreamingRequested = false;

			void FireOnLoadEvent();

			unsigned int m_width;
			unsigned int m_height;

			static JsValueRef m_createVideoFunction;
			static JsValueRef CHAKRA_CALLBACK createVideo(
				_In_ JsValueRef callee,
				_In_ bool isConstructCall,
				_In_ JsValueRef *arguments,
				_In_ unsigned short argumentCount,
				_In_ PVOID callbackData
			);

			static JsValueRef m_setVideoSourceFunction;
			static JsValueRef CHAKRA_CALLBACK setVideoSource(
				_In_ JsValueRef callee,
				_In_ bool isConstructCall,
				_In_ JsValueRef *arguments,
				_In_ unsigned short argumentCount,
				_In_ PVOID callbackData
			);

			bool ConfigureDecoder(IMFSourceReader* reader);

			Microsoft::WRL::ComPtr<IMFSourceReader> m_videoReader;
			Microsoft::WRL::ComPtr<IMFSample> m_frontSample;
			Microsoft::WRL::ComPtr<IMFSample> m_backSample;
			Microsoft::WRL::ComPtr<IMFMediaBuffer> m_backMediaBuffer;
            Microsoft::WRL::ComPtr<IMFMediaBuffer> m_frontMediaBuffer;

			std::mutex m_frameBufferLock;
			bool m_newFrameAvailable = false;

            bool InitializeHardwareDecoding();

            Microsoft::WRL::ComPtr<ID3D11Device> m_device;
            Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deviceContext;
            Microsoft::WRL::ComPtr<IMFDXGIDeviceManager> m_dxgiManager;
            UINT m_dxgiManagerResetToken;

            byte* m_frontBufferMemory = nullptr;
            byte* m_backBufferMemory = nullptr;
            DWORD m_frontBufferMemorySize = 0;
            DWORD m_backBufferMemorySize = 0;

            void SwapBuffers();
		};
	}
}


