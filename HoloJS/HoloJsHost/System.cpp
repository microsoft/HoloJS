#include "pch.h"
#include "System.h"
#include <agents.h>

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

	return true;
}

JsValueRef
System::setTimeout(
	JsValueRef callee,
	JsValueRef* arguments,
	unsigned short argumentCount
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount >= 3);

	JsValueRef callback = arguments[1];

	int timeout;
	RETURN_INVALID_REF_IF_JS_ERROR(JsNumberToInt(arguments[2], &timeout));

	unsigned int refCount;
	RETURN_INVALID_REF_IF_JS_ERROR(JsAddRef(callback, &refCount));

	vector<JsValueRef> callbackParameters(argumentCount - 2);
	callbackParameters[0] = callee;
	JsAddRef(callee, &refCount);
	for (int i = 3; i < argumentCount; i++)
	{
		JsAddRef(arguments[i], &refCount);
		callbackParameters[i - 2] = arguments[i];
	}

	task_completion_event<void> tce;

	// Create a non-repeating timer.
	auto fire_once = new timer<int>(timeout, 0, nullptr, false);
	auto timerCallback = new call<int>([tce](int)
	{
		tce.set();
	});

	// Connect the timer to the callback and start the timer.
	fire_once->link_target(timerCallback);
	fire_once->start();

	task<void> event_set(tce);

	// Create a continuation task that cleans up resources and
	// and return that continuation task.
	event_set.then([timerCallback, fire_once, callback, callbackParameters]() mutable
	{
		JsValueRef result;
		JsCallFunction(callback, callbackParameters.data(), static_cast<unsigned short>(callbackParameters.size()), &result);

		unsigned int refCount;
		JsRelease(callback, &refCount);

		for (const auto& parameter : callbackParameters)
		{
			JsRelease(parameter, &refCount);
		}

		delete timerCallback;
		delete fire_once;
	}, task_continuation_context::use_current());

	JsValueRef retValue;
	RETURN_INVALID_REF_IF_JS_ERROR(JsIntToNumber(0, &retValue));
	return retValue;
}
