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

				bool CaptureScriptResources(JsValueRef scriptCallback, const std::vector<JsValueRef>& scriptCallbackParameters);
				bool ReleaseScriptResources();
				bool InvokeScriptCallback();

				TimerType GetType() const { return Type; }

				void Continue();

				void SetCallback(std::function<void()> lambda)
				{
					UserLambda = lambda;
					Continuation->then(lambda, concurrency::task_continuation_context::use_current());
				}

				int ID;
			private:

				TimerType Type;
				JsValueRef ScriptCallback = JS_INVALID_REFERENCE;
				std::vector<JsValueRef> ScriptCallbackParameters;

				std::unique_ptr<concurrency::timer<int>> Timer;
				std::unique_ptr<concurrency::call<int>> Callback;
				std::unique_ptr<concurrency::task<void>> Continuation;

				std::function<void()> UserLambda;
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
				return reinterpret_cast<Timers*>(callbackData)->CreateTimer(TimerType::Timeout, callee, arguments, argumentCount);
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
				return reinterpret_cast<Timers*>(callbackData)->CreateTimer(TimerType::Interval, callee, arguments, argumentCount);
			}

			JsValueRef m_clearTimerFunction = JS_INVALID_REFERENCE;
			static JsValueRef CHAKRA_CALLBACK clearTimerStatic(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			)
			{
				return reinterpret_cast<Timers*>(callbackData)->ClearTimer(callee, arguments, argumentCount);
			}

			JsValueRef CreateTimer(
				TimerType type,
				JsValueRef callee,
				JsValueRef* arguments,
				unsigned short argumentCount
			);

			JsValueRef ClearTimer(
				JsValueRef callee,
				JsValueRef* arguments,
				unsigned short argumentCount
			);
		};

	}
}

