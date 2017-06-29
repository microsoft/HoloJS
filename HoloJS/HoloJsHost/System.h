#pragma once

#include <agents.h>

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

			class Timeout
			{
			public:
				std::unique_ptr<concurrency::timer<int>> Timer;
				std::unique_ptr<concurrency::call<int>> Callback;
				std::unique_ptr<concurrency::task<void>> Continuation;
				JsValueRef ScriptCallback;
				std::vector<JsValueRef> ScriptCallbackParameters;
				int ID;
			};

			// Lock for the list of active timers
			std::mutex m_timeoutsLock;
			
			// List of active timers
			// On timer clear or timer firing, it is removed from this list
			std::list<std::shared_ptr<Timeout>> m_timeouts;

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

			JsValueRef m_clearTimeoutFunction = JS_INVALID_REFERENCE;
			static JsValueRef CHAKRA_CALLBACK clearTimeoutStatic(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			)
			{
				return reinterpret_cast<System*>(callbackData)->clearTimeout(callee, arguments, argumentCount);
			}

			JsValueRef m_logFunction = JS_INVALID_REFERENCE;
			static JsValueRef CHAKRA_CALLBACK logStatic(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			)
			{
				return reinterpret_cast<System*>(callbackData)->log(callee, arguments, argumentCount);
			}

			JsValueRef setTimeout(
				JsValueRef callee,
				JsValueRef* arguments,
				unsigned short argumentCount
			);

			JsValueRef clearTimeout(
				JsValueRef callee,
				JsValueRef* arguments,
				unsigned short argumentCount
			);

			JsValueRef log(
				JsValueRef callee,
				JsValueRef* arguments,
				unsigned short argumentCount
			);
		};

	}
}