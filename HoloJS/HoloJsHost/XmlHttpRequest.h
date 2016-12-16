#pragma once

namespace HologramJS
{
	namespace API
	{
		class XmlHttpRequest : public HologramJS::Utilities::ElementWithEvents, public HologramJS::Utilities::IRelease
		{
		public:
			XmlHttpRequest();
			virtual ~XmlHttpRequest() {}

			virtual void Release() { }

			static bool Initialize();

			void SendRequest(const std::wstring& method, const std::wstring& uri, const std::wstring type);

			static bool UseFileSystem;
			static std::wstring BaseUrl;
			static std::wstring BasePath;

		private:

			bool IsTextResponse()
			{
				return (m_responseType.empty() || (_wcsicmp(m_responseType.c_str(), L"text") == 0));
			}

			std::wstring m_url;
			std::wstring m_method;
			std::wstring m_responseType = L"";

			std::wstring m_responseText;
			Windows::Storage::Streams::IBuffer^ m_response;
			unsigned long m_responseLength;
			int m_status;
			std::wstring m_statusText = L"";

			enum class RequestState
			{
				DONE = 4,
				OPENED = 1,
				HEADERS_RECEIVED = 2
			};

			RequestState m_state;

			concurrency::task<void> ReadFromPackageAsync();
			concurrency::task<void> DownloadAsync();

			void FireStateChanged();

			static JsValueRef m_createXHRFunction;
			static JsValueRef CHAKRA_CALLBACK createXHR(
				_In_ JsValueRef callee,
				_In_ bool isConstructCall,
				_In_ JsValueRef *arguments,
				_In_ unsigned short argumentCount,
				_In_ PVOID callbackData
			);

			static JsValueRef m_sendXHRFunction;
			static JsValueRef CHAKRA_CALLBACK sendXHR(
				_In_ JsValueRef callee,
				_In_ bool isConstructCall,
				_In_ JsValueRef *arguments,
				_In_ unsigned short argumentCount,
				_In_ PVOID callbackData
			);

			static JsValueRef m_getResponseTextFunction;
			static JsValueRef CHAKRA_CALLBACK getResponseText(
				_In_ JsValueRef callee,
				_In_ bool isConstructCall,
				_In_ JsValueRef *arguments,
				_In_ unsigned short argumentCount,
				_In_ PVOID callbackData
			);

			static JsValueRef m_getResponseFunction;
			static JsValueRef CHAKRA_CALLBACK getResponse(
				_In_ JsValueRef callee,
				_In_ bool isConstructCall,
				_In_ JsValueRef *arguments,
				_In_ unsigned short argumentCount,
				_In_ PVOID callbackData
			);
		};
	}
}
