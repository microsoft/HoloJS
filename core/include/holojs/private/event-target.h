#pragma once

#include "chakra.h"
#include <string>
#include <vector>
#include <map>

namespace HoloJs {

class EventTarget {
   public:
    virtual long addEventListener(const std::wstring &eventName, JsValueRef handler, JsValueRef handlerContext);
    virtual long removeEventListener(const std::wstring &eventName, JsValueRef handler);
    virtual long invokeEventListeners(const std::wstring &eventName);
    virtual long invokeEventListeners(const std::wstring &eventName, JsValueRef argument);
    virtual long invokeEventListeners(const std::wstring &eventName, const std::vector<JsValueRef> &arguments);

    ~EventTarget();

   protected:
    JsValueRef m_scriptCallback = JS_INVALID_REFERENCE;
    JsValueRef m_scriptCallbackContext = JS_INVALID_REFERENCE;

    std::map<std::wstring, std::map<JsValueRef, JsValueRef>> m_handlerMap;
};

}  // namespace HoloJs