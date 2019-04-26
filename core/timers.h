#pragma once

#include "include/holojs/private/chakra.h"
#include "include/holojs/private/error-handling.h"
#include "include/holojs/private/timers.h"
#include <agents.h>
#include <mutex>
#include <list>
#include <experimental\resumable>
#include <pplawait.h>
#include <ppltasks.h>
#include "timers.h"

namespace HoloJs {
namespace Interfaces {
class Timers {
   public:
    Timers() {}
    ~Timers() {}

    HRESULT initialize();

    void setTimersImplementation(ITimersImplementation* timersImplementation)
    {
        m_timersImplementation.reset(timersImplementation);
    }

   private:

    JsValueRef m_setTimeoutFunction = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK setTimeoutStatic(JsValueRef callee,
                                                       bool isConstructCall,
                                                       JsValueRef* arguments,
                                                       unsigned short argumentCount,
                                                       PVOID callbackData)
    {
        return reinterpret_cast<Timers*>(callbackData)
            ->m_timersImplementation->createTimer(TimerType::Timeout, callee, arguments, argumentCount);
    }

    JsValueRef m_setIntervalFunction = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK setIntervalStatic(JsValueRef callee,
                                                        bool isConstructCall,
                                                        JsValueRef* arguments,
                                                        unsigned short argumentCount,
                                                        PVOID callbackData)
    {
        return reinterpret_cast<Timers*>(callbackData)
            ->m_timersImplementation->createTimer(TimerType::Interval, callee, arguments, argumentCount);
    }

    JsValueRef m_clearTimerFunction = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK clearTimerStatic(JsValueRef callee,
                                                       bool isConstructCall,
                                                       JsValueRef* arguments,
                                                       unsigned short argumentCount,
                                                       PVOID callbackData)
    {
        RETURN_INVALID_REF_IF_TRUE(argumentCount < 2);

        int timerId;
        RETURN_INVALID_REF_IF_JS_ERROR(JsNumberToInt(arguments[1], &timerId));

        return reinterpret_cast<Timers*>(callbackData)->m_timersImplementation->clearTimer(timerId);
    }

    std::shared_ptr<ITimersImplementation> m_timersImplementation;
};

}  // namespace Interfaces
}  // namespace HoloJs
