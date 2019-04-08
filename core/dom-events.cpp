#include "dom-events.h"
#include "host-interfaces.h"
#include "include/holojs/private/chakra.h"
#include "resource-management/resource-manager.h"
#include "include/holojs/private/script-host-utilities.h"

using namespace HoloJs::Interfaces;
using namespace HoloJs::ResourceManagement;
using namespace std;

long DOMEventRegistration::initialize()
{
    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"addEventListener", L"eventRegistration", staticAddEventListener, this, &m_addEventListenerFunction));
    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"removeEventListener", L"eventRegistration", staticRemoveEventListener, this, &m_removeEventListenerFunction));

    return S_OK;
}

JsValueRef DOMEventRegistration::addEventListener(_In_ JsValueRef callee,
                                                  _In_ JsValueRef* arguments,
                                                  _In_ unsigned short argumentCount)
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

JsValueRef DOMEventRegistration::removeEventListener(_In_ JsValueRef* arguments, _In_ unsigned short argumentCount)
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
