#include "pch.h"
#include "Timers.h"

using namespace HologramJS::API;
using namespace concurrency;
using namespace std;
using namespace HologramJS::Utilities;

bool
Timers::Initialize()
{
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"setTimeout", L"timers", setTimeoutStatic, this, &m_setTimeoutFunction));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"clearTimeout", L"timers", clearTimeoutStatic, this, &m_clearTimeoutFunction));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"setInterval", L"timers", setIntervalStatic, this, &m_setIntervalFunction));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"clearInterval", L"timers", clearIntervalStatic, this, &m_clearIntervalFunction));

	return true;
}

Timers::TimerDefinition::TimerDefinition(TimerType type, int duration)
	: Type(type)
{
	Timer = make_unique<timer<int>>(duration, 0, nullptr, (type == TimerType::Timeout ? false : true));
	
	task_completion_event<void> tce;
	Callback = make_unique<call<int>>([tce](int)
	{
		tce.set();
	});

	// Connect the timer to the callback and start the timer.
	Timer->link_target(Callback.get());
	Timer->start();
	Continuation = make_unique<task<void>>(tce);
	ID = 0;
}

bool
Timers::TimerDefinition::CaptureScriptResources(JsValueRef scriptCallback, const std::vector<JsValueRef>& scriptCallbackParameters)
{
	ScriptCallback = scriptCallback;
	ScriptCallbackParameters = scriptCallbackParameters;

	// Increase the ref count to prevent garbage collection before the timer fires
	unsigned int refCount;
	RETURN_IF_JS_ERROR(JsAddRef(ScriptCallback, &refCount));

	for (auto& parameter : ScriptCallbackParameters)
	{
		RETURN_IF_JS_ERROR(JsAddRef(parameter, &refCount));
	}

	return true;
}

bool
Timers::TimerDefinition::ReleaseScriptResources()
{
	// Release JS resources
	unsigned int refCount;

	if (ScriptCallback != JS_INVALID_REFERENCE)
	{
		RETURN_IF_JS_ERROR(JsRelease(ScriptCallback, &refCount));
	}
	
	for (const auto& parameter : ScriptCallbackParameters)
	{
		RETURN_IF_JS_ERROR(JsRelease(parameter, &refCount));
	}

	return true;
}

bool
Timers::TimerDefinition::InvokeScriptCallback()
{
	// Call the script
	
	RETURN_IF_TRUE(ScriptCallback == JS_INVALID_REFERENCE);

	JsValueRef result;
	RETURN_IF_JS_ERROR(
		JsCallFunction(
			ScriptCallback,
			ScriptCallbackParameters.data(),
			static_cast<unsigned short>(ScriptCallbackParameters.size()),
			&result));

	return true;
}

JsValueRef
Timers::clearTimeout(
	JsValueRef callee,
	JsValueRef* arguments,
	unsigned short argumentCount
)
{
	RETURN_INVALID_REF_IF_TRUE(argumentCount < 2);

	int timeoutId;
	RETURN_INVALID_REF_IF_JS_ERROR(JsNumberToInt(arguments[1], &timeoutId));

	shared_ptr<TimerDefinition> timeout;
	// Lookup and remove the timer from the list
	{
		// Lock the timeouts list
		std::lock_guard<std::mutex> guard(m_timeoutsLock);

		// Lookup the ID in the timeouts list
		for (auto& timeoutInstance : m_timeouts)
		{
			if (timeoutId == timeoutInstance->ID)
			{
				// Found it; remove it
				timeout = timeoutInstance;
				m_timeouts.remove(timeout);
				break;
			}
		}
	}

	if (!timeout)
	{
		// The timeout must have fired already; return
		return JS_INVALID_REFERENCE;
	}

	timeout->Stop();

	return JS_INVALID_REFERENCE;
}

JsValueRef
Timers::setTimeout(
	JsValueRef callee,
	JsValueRef* arguments,
	unsigned short argumentCount
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount >= 3);

	int timeoutValue;
	RETURN_INVALID_REF_IF_JS_ERROR(JsNumberToInt(arguments[2], &timeoutValue));

	// Capture the parameters intended for the callback function;
	const JsValueRef scriptCallback = arguments[1];
	vector<JsValueRef> capturedParameters(argumentCount - 2);
	capturedParameters[0] = callee;
	for (int i = 3; i < argumentCount; i++)
	{
		capturedParameters[i - 2] = arguments[i];
	}

	shared_ptr<TimerDefinition> timeout = make_shared<TimerDefinition>(TimerType::Timeout, timeoutValue);
	RETURN_INVALID_REF_IF_FALSE(timeout->CaptureScriptResources(scriptCallback, capturedParameters));

	// Insert the timeout in the list and assign it an ID
	{
		std::lock_guard<std::mutex> guard(m_timeoutsLock);

		for (const auto& timeoutInstance : m_timeouts)
		{
			if (timeout->ID <= timeoutInstance->ID)
			{
				timeout->ID = timeoutInstance->ID + 1;
			}
		}
		m_timeouts.push_back(timeout);
	}

	int id = timeout->ID;

	// Declare the code to be executed when the timer fires
	timeout->GetContinuation()->then([this, id]()
	{
		shared_ptr<TimerDefinition> timeout;
		// Lookup and remove the firing timer from the list of timers
		{
			// Lock the list of timeouts
			std::lock_guard<std::mutex> guard(m_timeoutsLock);

			// Lookup the ID in the timeouts list
			for (auto& timeoutInstance : m_timeouts)
			{
				if (id == timeoutInstance->ID)
				{
					// Found it; remove it
					timeout = timeoutInstance;
					m_timeouts.remove(timeout);
					break;
				}
			}
		}

		if (!timeout)
		{
			// The timeout must have been cleared; return
			return;
		}

		// Call the script
		timeout->InvokeScriptCallback();
	}, task_continuation_context::use_current());

	JsValueRef retValue;
	RETURN_INVALID_REF_IF_JS_ERROR(JsIntToNumber(timeout->ID, &retValue));
	return retValue;
}

JsValueRef
Timers::setInterval(
	JsValueRef callee,
	JsValueRef* arguments,
	unsigned short argumentCount
)
{
	return JS_INVALID_REFERENCE;
}

JsValueRef
Timers::clearInterval(
	JsValueRef callee,
	JsValueRef* arguments,
	unsigned short argumentCount
)
{
	return JS_INVALID_REFERENCE;
}

