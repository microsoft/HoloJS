#pragma once

namespace HologramJS
{
	namespace API
	{
		class WindowElement
		{
		public:

			WindowElement();
			~WindowElement();

			void Close();

			bool Initialize();

			void Resize(int width, int height);
			void VSync(Windows::Foundation::Numerics::float4x4 viewMatrix);

			Input::KeyboardInput& KeyboardRouter() { return m_keyboardInput; }
			Input::MouseInput& MouseRouter() { return m_mouseInput; }

			void SetBaseUrl(const std::wstring& baseUrl) { m_baseUrl.assign(baseUrl); }

		private:

			std::wstring m_baseUrl;

			Input::KeyboardInput m_keyboardInput;
			Input::MouseInput m_mouseInput;

			int m_width;
			int m_height;

			JsValueRef m_callbackFunction = JS_INVALID_REFERENCE;

			JsValueRef m_getWidthFunction = JS_INVALID_REFERENCE;
			static JsValueRef CHAKRA_CALLBACK getWidthStatic(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			)
			{
				return reinterpret_cast<WindowElement*>(callbackData)->getWidth(arguments, argumentCount);
			}

			JsValueRef getWidth(
				JsValueRef* arguments,
				unsigned short argumentCount
			);

			JsValueRef m_getHeightFunction = JS_INVALID_REFERENCE;
			static JsValueRef CHAKRA_CALLBACK getHeightStatic(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			)
			{
				return reinterpret_cast<WindowElement*>(callbackData)->getHeight(arguments, argumentCount);
			}

			JsValueRef getHeight(
				JsValueRef* arguments,
				unsigned short argumentCount
			);

			JsValueRef m_getBaseUrlFunction = JS_INVALID_REFERENCE;
			static JsValueRef CHAKRA_CALLBACK getBaseUrlStatic(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			)
			{
				return reinterpret_cast<WindowElement*>(callbackData)->getBaseUrl(arguments, argumentCount);
			}

			JsValueRef getBaseUrl(
				JsValueRef* arguments,
				unsigned short argumentCount
			);

			JsValueRef m_setCallbackFunction = JS_INVALID_REFERENCE;
			static JsValueRef CHAKRA_CALLBACK setCallbackStatic(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			)
			{
				return reinterpret_cast<WindowElement*>(callbackData)->setCallback(arguments, argumentCount);
			}

			JsValueRef setCallback(
				JsValueRef* arguments,
				unsigned short argumentCount
			);

			float* m_viewMatrixStoragePointer = nullptr;
			JsValueRef m_viewMatrixScriptProjection = JS_INVALID_REFERENCE;

			Windows::Foundation::Numerics::float4x4 m_inverseViewMatrix;
			float* m_cameraPositionStoragePointer = nullptr;
			JsValueRef m_cameraPositionScriptProjection = JS_INVALID_REFERENCE;

			bool CreateViewMatrixStorageAndScriptProjection();
		};

	}
}
