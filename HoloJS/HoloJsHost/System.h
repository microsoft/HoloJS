#pragma once

namespace HologramJS
{
	namespace API
	{
		class System
		{
		public:
			System();
			~System();

			bool Initialize();

		private:

			JsValueRef m_setTimeoutFunction = JS_INVALID_REFERENCE;
			static JsValueRef CHAKRA_CALLBACK setTimeoutStatic(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			)
			{
				return reinterpret_cast<System*>(callbackData)->setTimeout(callee, arguments, argumentCount);
			}

			JsValueRef setTimeout(
				JsValueRef callee,
				JsValueRef* arguments,
				unsigned short argumentCount
			);
		};

	}
}