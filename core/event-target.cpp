#include "include/holojs/private/chakra.h"
#include "include/holojs/private/event-target.h"
#include "include/holojs/private/error-handling.h"
#include <array>

using namespace HoloJs;
using namespace std;

EventTarget::~EventTarget()
{
    for (auto& eventHandlers : m_handlerMap) {
        for (auto& handler : eventHandlers.second) {
            JsRelease(handler.first, nullptr);
            JsRelease(handler.second, nullptr);
        }
    }
}

long EventTarget::addEventListener(const std::wstring& eventName, JsValueRef handler, JsValueRef handlerContext)
{
    auto handlersForEventName = m_handlerMap.find(eventName);
    if (handlersForEventName == m_handlerMap.end()) {
        auto eventMap = map<JsValueRef, JsValueRef>();
        eventMap.emplace(handler, handlerContext);
        m_handlerMap.emplace(eventName, eventMap);

        RETURN_IF_JS_ERROR(JsAddRef(handler, nullptr));
        RETURN_IF_JS_ERROR(JsAddRef(handlerContext, nullptr));
    } else {
        auto registeredHandler = handlersForEventName->second.find(handler);
        if (registeredHandler == handlersForEventName->second.end()) {
            handlersForEventName->second.emplace(handler, handlerContext);
            RETURN_IF_JS_ERROR(JsAddRef(handler, nullptr));
            RETURN_IF_JS_ERROR(JsAddRef(handlerContext, nullptr));
        }
    }

    return S_OK;
}

long EventTarget::removeEventListener(const std::wstring& eventName, JsValueRef handler)
{
    auto handlersForEventName = m_handlerMap.find(eventName);
    if (handlersForEventName != m_handlerMap.end()) {
        auto registeredHandler = handlersForEventName->second.find(handler);
        if (registeredHandler != handlersForEventName->second.end()) {
            RETURN_IF_JS_ERROR(JsRelease(registeredHandler->first, nullptr));
            RETURN_IF_JS_ERROR(JsRelease(registeredHandler->second, nullptr));

            handlersForEventName->second.erase(handler);
        }
    }

    return S_OK;
}

long EventTarget::invokeEventListeners(const wstring& eventName)
{
    JsValueRef result;

    auto handlersForEventName = m_handlerMap.find(eventName);
    if (handlersForEventName != m_handlerMap.end()) {
        for (auto& handler : handlersForEventName->second) {
            HANDLE_EXCEPTION_IF_JS_ERROR(JsCallFunction(handler.first, &handler.second, 1, &result));
        }
    }

    return S_OK;
}

long EventTarget::invokeEventListeners(const std::wstring& eventName, JsValueRef argument) {
    JsValueRef result;

    auto handlersForEventName = m_handlerMap.find(eventName);
    if (handlersForEventName != m_handlerMap.end()) {
        for (auto& handler : handlersForEventName->second) {
            array<JsValueRef, 2> allArguments;
            allArguments[0] = handler.second;
            allArguments[1] = argument;
            HANDLE_EXCEPTION_IF_JS_ERROR(JsCallFunction(handler.first, allArguments.data(), static_cast<unsigned short>(allArguments.size()), &result));
        }
    }

    return S_OK;
}

long EventTarget::invokeEventListeners(const std::wstring& eventName,
                                          const vector<JsValueRef>& arguments)
{
    JsValueRef result;

    auto handlersForEventName = m_handlerMap.find(eventName);
    if (handlersForEventName != m_handlerMap.end()) {
        for (auto& handler : handlersForEventName->second) {
            vector<JsValueRef> allArguments;
            allArguments.push_back(handler.second);
            allArguments.insert(allArguments.end(), arguments.begin(), arguments.end());
            HANDLE_EXCEPTION_IF_JS_ERROR(
                JsCallFunction(handler.first, allArguments.data(), static_cast<unsigned short>(allArguments.size()), &result));
        }
    }

    return S_OK;
}
