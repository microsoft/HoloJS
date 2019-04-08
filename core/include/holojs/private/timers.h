#pragma once

#include "chakra.h"
#include "error-handling.h"
#include <memory>

namespace HoloJs {

enum class TimerType { Timeout, Interval };

class ITimersImplementation {
   public:
    virtual ~ITimersImplementation() {}

    virtual JsValueRef createTimer(TimerType type,
                                   JsValueRef callee,
                                   JsValueRef* arguments,
                                   unsigned short argumentCount) = 0;
    virtual JsValueRef clearTimer(int timerId) = 0;
};

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

    JsValueRef m_clearIntervalFunction = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK clearIntervalStatic(JsValueRef callee,
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

}  // namespace HoloJs