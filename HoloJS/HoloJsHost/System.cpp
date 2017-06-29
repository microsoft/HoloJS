#include "pch.h"
#include "System.h"


using namespace HologramJS::API;
using namespace HologramJS::Utilities;
using namespace concurrency;
using namespace std;

System::System()
{
}


System::~System()
{
}

bool
System::Initialize()
{
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"setTimeout", L"system", setTimeoutStatic, this, &m_setTimeoutFunction));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"clearTimeout", L"system", clearTimeoutStatic, this, &m_clearTimeoutFunction));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"log", L"system", logStatic, this, &m_logFunction));

	return true;
}

JsValueRef
System::log(
	JsValueRef callee,
	JsValueRef* arguments,
	unsigned short argumentCount
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

	const wchar_t* logString;
	size_t logStringLength;
	RETURN_INVALID_REF_IF_JS_ERROR(JsStringToPointer(arguments[1], &logString, &logStringLength));

	OutputDebugString(logString);

	return JS_INVALID_REFERENCE;
}

JsValueRef
System::clearTimeout(
	JsValueRef callee,
	JsValueRef* arguments,
	unsigned short argumentCount
)
{
	RETURN_INVALID_REF_IF_TRUE(argumentCount < 2);

	int timeoutId;
	RETURN_INVALID_REF_IF_JS_ERROR(JsNumberToInt(arguments[1], &timeoutId));

	shared_ptr<Timeout> timeout;
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

	timeout->Timer->stop();

	// Release JS resources
	unsigned int refCount;
	JsRelease(timeout->ScriptCallback, &refCount);
	for (const auto& parameter : timeout->ScriptCallbackParameters)
	{
		JsRelease(parameter, &refCount);
	}

	return JS_INVALID_REFERENCE;
}

JsValueRef
System::setTimeout(
	JsValueRef callee,
	JsValueRef* arguments,
	unsigned short argumentCount
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount >= 3);

	shared_ptr<Timeout> timeout = make_shared<Timeout>();

	timeout->ScriptCallback = arguments[1];

	int timeoutValue;
	RETURN_INVALID_REF_IF_JS_ERROR(JsNumberToInt(arguments[2], &timeoutValue));

	unsigned int refCount;
	RETURN_INVALID_REF_IF_JS_ERROR(JsAddRef(timeout->ScriptCallback, &refCount));

	// Capture the parameters intended for the callback function;
	// Increase the ref count to prevent garbage collection before the timer fires
	timeout->ScriptCallbackParameters.resize(argumentCount - 2);
	timeout->ScriptCallbackParameters[0] = callee;
	JsAddRef(callee, &refCount);
	for (int i = 3; i < argumentCount; i++)
	{
		JsAddRef(arguments[i], &refCount);
		timeout->ScriptCallbackParameters[i - 2] = arguments[i];
	}

	// Create a timer object to be inserted in our list of active timers
	timeout->Timer = make_unique<timer<int>>(timeoutValue, 0, nullptr, false);
	task_completion_event<void> tce;
	timeout->Callback = make_unique<call<int>>([tce](int)
	{
		tce.set();
	});

	// Connect the timer to the callback and start the timer.
	timeout->Timer->link_target(timeout->Callback.get());
	timeout->Timer->start();
	timeout->Continuation = make_unique<task<void>>(tce);
	timeout->ID = 0;

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
	timeout->Continuation->then([this, id]()
	{
		shared_ptr<Timeout> timeout;
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
		JsValueRef result;
		JsCallFunction(timeout->ScriptCallback, timeout->ScriptCallbackParameters.data(), static_cast<unsigned short>(timeout->ScriptCallbackParameters.size()), &result);

		// Release script resources
		unsigned int refCount;
		JsRelease(timeout->ScriptCallback, &refCount);
		for (const auto& parameter : timeout->ScriptCallbackParameters)
		{
			JsRelease(parameter, &refCount);
		}
	}, task_continuation_context::use_current());

	JsValueRef retValue;
	RETURN_INVALID_REF_IF_JS_ERROR(JsIntToNumber(timeout->ID, &retValue));
	return retValue;
}
