#pragma once

#include "holojs/private/timers.h"
#include <agents.h>
#include <concrt.h>
#include <experimental\resumable>
#include <list>
#include <mutex>
#include <pplawait.h>
#include <ppltasks.h>

namespace HoloJs {
namespace UWP {

class WinRTTimers : public HoloJs::ITimersImplementation {
   public:
    WinRTTimers() { }
    ~WinRTTimers() {
		for (auto& timer : m_timers) {
			timer->m_thredpoolTimer->Cancel();
		}
	}

   private:
    class TimerDefinition {
       public:
        TimerDefinition(int duration);
		~TimerDefinition() { m_thredpoolTimer->Cancel();  releaseScriptResources(); }

        // Captures the script callback and parameters to be passed to this callback
        HRESULT captureScriptResources(JsValueRef scriptCallback,
                                       const std::vector<JsValueRef>& scriptCallbackParameters);

        // Release captured script resources
        HRESULT releaseScriptResources();

        // Invokes the script callback
        HRESULT invokeScriptCallback();

        Windows::System::Threading::ThreadPoolTimer ^ m_thredpoolTimer;

        int m_id;

       private:
        // Script resources
        JsValueRef m_scriptCallback = JS_INVALID_REFERENCE;
        std::vector<JsValueRef> m_scriptCallbackParameters;
    };

    // Lock for the list of active timers
    std::mutex m_timersLock;

    // List of active timers
    // On timer clear or timer firing, it is removed from this list
    std::list<std::shared_ptr<TimerDefinition>> m_timers;

    enum class ListOperationType { ProcessTick, Remove };

    int insertInTimersList(std::shared_ptr<TimerDefinition> timer);

    virtual JsValueRef createTimer(HoloJs::TimerType type,
                                   JsValueRef callee,
                                   JsValueRef* arguments,
                                   unsigned short argumentCount);

    virtual JsValueRef clearTimer(int timerId);
};

}  // namespace UWP
}  // namespace HoloJs
