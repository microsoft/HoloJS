#pragma once

#include "ChakraForHoloJS.h"
#include <agents.h>

namespace HologramJS {
namespace API {
class Timers {
   public:
    Timers() {}
    ~Timers() {}

    bool Initialize();

   private:
    enum class TimerType { Timeout, Interval };

    class TimerDefinition {
       public:
        TimerDefinition(TimerType type, int duration);
        ~TimerDefinition() { ReleaseScriptResources(); }

        void Stop() { Timer->stop(); }

        // Captures the script callback and parameters to be passed to this callback
        bool CaptureScriptResources(JsValueRef scriptCallback, const std::vector<JsValueRef>& scriptCallbackParameters);

        // Release captured script resources
        bool ReleaseScriptResources();

        // Invokes the script callback
        bool InvokeScriptCallback();

        TimerType GetType() const { return Type; }

        // For periodic timers, to keep the timer firing, call this method each time the timer fires
        void Continue();

        void SetNativeCallback(std::function<void()> lambda)
        {
            NativeCallback = lambda;
            Continuation->then(lambda, concurrency::task_continuation_context::use_current());
        }

        int ID;

       private:
        // Timeout or Interval (periodic)
        TimerType Type;

        // Script resources
        JsValueRef ScriptCallback = JS_INVALID_REFERENCE;
        std::vector<JsValueRef> ScriptCallbackParameters;

        // Internal timer and task structures
        std::unique_ptr<concurrency::timer<int>> Timer;
        std::unique_ptr<concurrency::call<int>> InternalCallback;
        std::unique_ptr<concurrency::task<void>> Continuation;

        // C++ callback invoked when the timer fires
        std::function<void()> NativeCallback;
    };

    // Lock for the list of active timers
    std::mutex m_timersLock;

    // List of active timers
    // On timer clear or timer firing, it is removed from this list
    std::list<std::shared_ptr<TimerDefinition>> m_timers;

    enum class ListOperationType { ProcessTick, Remove };

    std::shared_ptr<TimerDefinition> ProcessTimersList(ListOperationType operationType, int id);
    int InsertInTimersList(std::shared_ptr<TimerDefinition> timer);

    JsValueRef m_setTimeoutFunction = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK setTimeoutStatic(JsValueRef callee,
                                                       bool isConstructCall,
                                                       JsValueRef* arguments,
                                                       unsigned short argumentCount,
                                                       PVOID callbackData)
    {
        return reinterpret_cast<Timers*>(callbackData)
            ->CreateTimer(TimerType::Timeout, callee, arguments, argumentCount);
    }

    JsValueRef m_setIntervalFunction = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK setIntervalStatic(JsValueRef callee,
                                                        bool isConstructCall,
                                                        JsValueRef* arguments,
                                                        unsigned short argumentCount,
                                                        PVOID callbackData)
    {
        return reinterpret_cast<Timers*>(callbackData)
            ->CreateTimer(TimerType::Interval, callee, arguments, argumentCount);
    }

    JsValueRef m_clearTimerFunction = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK clearTimerStatic(JsValueRef callee,
                                                       bool isConstructCall,
                                                       JsValueRef* arguments,
                                                       unsigned short argumentCount,
                                                       PVOID callbackData)
    {
        return reinterpret_cast<Timers*>(callbackData)->ClearTimer(callee, arguments, argumentCount);
    }

    JsValueRef CreateTimer(TimerType type, JsValueRef callee, JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef ClearTimer(JsValueRef callee, JsValueRef* arguments, unsigned short argumentCount);
};

}  // namespace API
}  // namespace HologramJS
