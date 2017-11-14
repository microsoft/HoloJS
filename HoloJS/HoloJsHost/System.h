#pragma once

#include "ChakraForHoloJS.h"

namespace HologramJS {
namespace API {
class System {
   public:
    System() {}
    ~System() {}

    bool Initialize();

   private:
    JsValueRef m_logFunction = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK logStatic(JsValueRef callee,
                                                bool isConstructCall,
                                                JsValueRef* arguments,
                                                unsigned short argumentCount,
                                                PVOID callbackData)
    {
        return reinterpret_cast<System*>(callbackData)->log(callee, arguments, argumentCount);
    }

    static void CALLBACK StaticPromiseContinuationCallback(JsValueRef task, void* callbackState)
    {
        reinterpret_cast<System*>(callbackState)->PromiseContinuationCallback(task);
    }

    void PromiseContinuationCallback(JsValueRef task);

    JsValueRef log(JsValueRef callee, JsValueRef* arguments, unsigned short argumentCount);
};

}  // namespace API
}  // namespace HologramJS
