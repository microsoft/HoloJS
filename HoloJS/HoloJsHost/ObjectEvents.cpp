#include "pch.h"
#include "ObjectEvents.h"

using namespace HologramJS::Utilities;

EventsManager::EventsManager()
{
}

EventsManager::~EventsManager()
{
}

bool
EventsManager::Initialize()
{
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"setCallback", L"eventing", setCallback, nullptr, &m_setCallbackFunction));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"removeCallback", L"eventing", removeCallback, nullptr, &m_removeCallbackFunction));

	return true;
}

JsValueRef
CHAKRA_CALLBACK
EventsManager::setCallback(
	_In_ JsValueRef callee,
	_In_ bool isConstructCall,
	_In_ JsValueRef *arguments,
	_In_ unsigned short argumentCount,
	_In_ PVOID callbackData
)
{
	RETURN_IF_FALSE(argumentCount == 3);

	ElementWithEvents* element = ScriptResourceTracker::ExternalToEventsInterface(arguments[1]);
	RETURN_IF_NULL(element);

	RETURN_IF_FALSE(element->SetCallback(arguments[2], callee));

	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
EventsManager::removeCallback(
	_In_ JsValueRef callee,
	_In_ bool isConstructCall,
	_In_ JsValueRef *arguments,
	_In_ unsigned short argumentCount,
	_In_ PVOID callbackData
)
{
	RETURN_IF_FALSE(argumentCount == 2);

	ElementWithEvents* element = ScriptResourceTracker::ExternalToEventsInterface(arguments[1]);
	RETURN_IF_NULL(element);

	element->ReleaseCallback();

	return nullptr;
}
