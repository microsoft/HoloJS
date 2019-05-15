#include "dom-events.h"
#include "host-interfaces.h"
#include "include/holojs/private/chakra.h"
#include "include/holojs/private/script-host-utilities.h"
#include "resource-management/resource-manager.h"

using namespace HoloJs::Interfaces;
using namespace HoloJs::ResourceManagement;
using namespace std;

long DOMEventRegistration::initialize()
{
    JS_PROJECTION_REGISTER(L"eventRegistration", L"addEventListener", addEventListener);
    JS_PROJECTION_REGISTER(L"eventRegistration", L"removeEventListener", removeEventListener);
    JS_PROJECTION_REGISTER(L"eventRegistration", L"dispatchEvent", dispatchEvent);

    return S_OK;
}

JsValueRef DOMEventRegistration::_addEventListener(_In_ JsValueRef* arguments,
                                                   _In_ unsigned short argumentCount,
                                                   _In_ JsValueRef callee)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    auto element = ResourceManager::externalToEventsInterface(arguments[1]);
    RETURN_INVALID_REF_IF_NULL(element);

    wstring eventName;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetString(arguments[2], eventName));

    JsValueRef handler = arguments[3];
    element->addEventListener(eventName, arguments[3], callee);

    return JS_INVALID_REFERENCE;
}

JsValueRef DOMEventRegistration::_removeEventListener(_In_ JsValueRef* arguments,
                                                      _In_ unsigned short argumentCount,
                                                      _In_ JsValueRef callee)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    auto element = ResourceManager::externalToEventsInterface(arguments[1]);
    RETURN_INVALID_REF_IF_NULL(element);

    wstring eventName;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetString(arguments[2], eventName));

    JsValueRef handler = arguments[3];
    element->removeEventListener(eventName, handler);

    return nullptr;
}

JsValueRef DOMEventRegistration::_dispatchEvent(_In_ JsValueRef* arguments,
                                                _In_ unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    auto element = ResourceManager::externalToEventsInterface(arguments[1]);
    RETURN_INVALID_REF_IF_NULL(element);

    wstring eventName;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetString(arguments[2], eventName));

    JsValueRef customEvent = arguments[3];
    element->invokeEventListeners(eventName, customEvent);

    return nullptr;
}
