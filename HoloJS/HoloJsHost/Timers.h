#pragma once

#include <agents.h>

namespace HologramJS
{
	namespace API
	{
		class Timers
		{
		public:
			Timers() {}
			~Timers() {}

			bool Initialize();

		private:

			enum class TimerType
			{
				Timeout,
				Interval
			};

			class TimerDefinition
			{
			public:

				TimerDefinition(TimerType type, int duration);
				~TimerDefinition() { ReleaseScriptResources(); }

				void Stop() { Timer->stop(); }
				concurrency::task<void>* GetContinuation() { return Continuation.get(); }

				bool CaptureScriptResources(JsValueRef scriptCallback, const std::vector<JsValueRef>& scriptCallbackParameters);
				bool ReleaseScriptResources();
				bool InvokeScriptCallback();

				int ID;
			private:

				TimerType Type;
				JsValueRef ScriptCallback = JS_INVALID_REFERENCE;
				std::vector<JsValueRef> ScriptCallbackParameters;

				std::unique_ptr<concurrency::timer<int>> Timer;
				std::unique_ptr<concurrency::call<int>> Callback;
				std::unique_ptr<concurrency::task<void>> Continuation;
			};

			// Lock for the list of active timers
			std::mutex m_timeoutsLock;

			// List of active timers
			// On timer clear or timer firing, it is removed from this list
			std::list<std::shared_ptr<TimerDefinition>> m_timeouts;

			JsValueRef m_setTimeoutFunction = JS_INVALID_REFERENCE;
			static JsValueRef CHAKRA_CALLBACK setTimeoutStatic(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			)
			{
				return reinterpret_cast<Timers*>(callbackData)->setTimeout(callee, arguments, argumentCount);
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
				return reinterpret_cast<Timers*>(callbackData)->clearTimeout(callee, arguments, argumentCount);
			}

			JsValueRef m_setIntervalFunction = JS_INVALID_REFERENCE;
			static JsValueRef CHAKRA_CALLBACK setIntervalStatic(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			)
			{
				return reinterpret_cast<Timers*>(callbackData)->setInterval(callee, arguments, argumentCount);
			}

			JsValueRef m_clearIntervalFunction = JS_INVALID_REFERENCE;
			static JsValueRef CHAKRA_CALLBACK clearIntervalStatic(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			)
			{
				return reinterpret_cast<Timers*>(callbackData)->clearInterval(callee, arguments, argumentCount);
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

			JsValueRef clearInterval(
				JsValueRef callee,
				JsValueRef* arguments,
				unsigned short argumentCount
			);

			JsValueRef setInterval(
				JsValueRef callee,
				JsValueRef* arguments,
				unsigned short argumentCount
			);

		};

	}
}

