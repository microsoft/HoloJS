#pragma once

#include "include/holojs/private/chakra.h"
#include "include/holojs/private/event-target.h"
#include <map>
#include <string>
#include <vector>

namespace HoloJs {
namespace Interfaces {

class DOMEventRegistration {
   public:
    DOMEventRegistration() {}
    ~DOMEventRegistration() {}

    long initialize();

   private:
    JsValueRef m_addEventListenerFunction;
    static JsValueRef CHAKRA_CALLBACK staticAddEventListener(_In_ JsValueRef callee,
                                                             _In_ bool isConstructCall,
                                                             _In_ JsValueRef *arguments,
                                                             _In_ unsigned short argumentCount,
                                                             _In_ PVOID callbackData)
    {
        return reinterpret_cast<DOMEventRegistration *>(callbackData)
            ->addEventListener(callee, arguments, argumentCount);
    }
    JsValueRef addEventListener(_In_ JsValueRef callee, _In_ JsValueRef *arguments, _In_ unsigned short argumentCount);

    JsValueRef m_removeEventListenerFunction;
    static JsValueRef CHAKRA_CALLBACK staticRemoveEventListener(_In_ JsValueRef callee,
                                                                _In_ bool isConstructCall,
                                                                _In_ JsValueRef *arguments,
                                                                _In_ unsigned short argumentCount,
                                                                _In_ PVOID callbackData)
    {
        return reinterpret_cast<DOMEventRegistration *>(callbackData)->removeEventListener(arguments, argumentCount);
    }
    JsValueRef removeEventListener(_In_ JsValueRef *arguments, _In_ unsigned short argumentCount);
};

}  // namespace Interfaces
}  // namespace HoloJs