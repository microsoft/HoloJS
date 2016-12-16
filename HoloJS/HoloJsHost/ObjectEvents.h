#pragma once

namespace HologramJS
{
	namespace Utilities
	{
		class ElementWithEvents
		{
		public:

			bool SetCallback(JsValueRef scriptCallback, JsValueRef scriptCallbackContext)
			{
				if (m_scriptCallback == JS_INVALID_REFERENCE)
				{
					m_scriptCallback = scriptCallback;
					m_scriptCallbackContext = scriptCallbackContext;

					unsigned int refCount;
					RETURN_IF_JS_ERROR(JsAddRef(m_scriptCallback, &refCount));
					RETURN_IF_JS_ERROR(JsAddRef(m_scriptCallbackContext, &refCount));
				}

				return true;
			}

			void ReleaseCallback()
			{
				if (m_scriptCallback != JS_INVALID_REFERENCE)
				{
					unsigned int refCount;
					JsRelease(m_scriptCallback, &refCount);
					m_scriptCallback = JS_INVALID_REFERENCE;
				}

				if (m_scriptCallbackContext != JS_INVALID_REFERENCE)
				{
					unsigned int refCount;
					JsRelease(m_scriptCallbackContext, &refCount);
					m_scriptCallbackContext = JS_INVALID_REFERENCE;
				}
			}

			bool InvokeCallback(std::vector<JsValueRef>& parameters)
			{
				JsValueRef result;
				parameters.insert(parameters.begin(), m_scriptCallbackContext);
				RETURN_IF_JS_ERROR(
					JsCallFunction(
						m_scriptCallback,
						parameters.data(),
						static_cast<unsigned short>(parameters.size()),
						&result));

				return true;
			}

			bool HasCallback() { return m_scriptCallback != JS_INVALID_REFERENCE; }

		private:
			JsValueRef m_scriptCallback = JS_INVALID_REFERENCE;
			JsValueRef m_scriptCallbackContext = JS_INVALID_REFERENCE;
		};

		class EventsManager
		{
		public:
			EventsManager();
			~EventsManager();

			bool Initialize();

		private:
			JsValueRef m_setCallbackFunction;
			static JsValueRef CHAKRA_CALLBACK setCallback(
				_In_ JsValueRef callee,
				_In_ bool isConstructCall,
				_In_ JsValueRef *arguments,
				_In_ unsigned short argumentCount,
				_In_ PVOID callbackData
			);

			JsValueRef m_removeCallbackFunction;
			static JsValueRef CHAKRA_CALLBACK removeCallback(
				_In_ JsValueRef callee,
				_In_ bool isConstructCall,
				_In_ JsValueRef *arguments,
				_In_ unsigned short argumentCount,
				_In_ PVOID callbackData
			);

		};


	}
}

