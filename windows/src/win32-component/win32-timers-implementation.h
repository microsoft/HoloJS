#pragma once

#include "holojs/private/timers.h"
#include <agents.h>
#include <concrt.h>
#include <experimental\resumable>
#include <list>
#include <mutex>
#include <pplawait.h>

namespace HoloJs {
class Win32TimerDefinition;

// This class is not designed to be thread safe; it should only be used on the thread that host the JS runtime
class Win32Timers : public HoloJs::ITimersImplementation {
   public:
    Win32Timers(HWND window) : m_window(window) {}
    ~Win32Timers() {}

    void processTimer(Win32TimerDefinition* timer);

    HWND getWindow() const { return m_window; }

   private:
    // List of active timers
    // On timer clear or timer firing, it is removed from this list
    std::list<std::shared_ptr<Win32TimerDefinition>> m_timers;

    enum class ListOperationType { ProcessTick, Remove };
    void insertTimer(std::shared_ptr<Win32TimerDefinition> timer);

    virtual JsValueRef createTimer(HoloJs::TimerType type,
                                   JsValueRef callee,
                                   JsValueRef* arguments,
                                   unsigned short argumentCount);

    virtual JsValueRef clearTimer(int timerId);

    HWND m_window;
};

class Win32TimerDefinition {
   public:
    Win32TimerDefinition(HoloJs::TimerType type, Win32Timers* scheduler) : m_type(type), m_scheduler(scheduler) {}
    ~Win32TimerDefinition();

    // Captures the script callback and parameters to be passed to this callback
    HRESULT captureScriptResources(JsValueRef scriptCallback, const std::vector<JsValueRef>& scriptCallbackParameters);

    // Invokes the script callback
    HRESULT invokeScriptCallback();

    HoloJs::TimerType GetType() const { return m_type; }

    int getId() const { return m_id; }
    void setId(int id) { m_id = id; }

    void stop();
    HRESULT start(int timeout);

    Win32Timers* getScheduler() const { return m_scheduler; }

   private:
    // Release captured script resources
    HRESULT releaseScriptResources();

    // Timeout or Interval (periodic)
    HoloJs::TimerType m_type;

    // Script resources
    JsValueRef m_scriptCallback = JS_INVALID_REFERENCE;
    std::vector<JsValueRef> m_scriptCallbackParameters;

    Win32Timers* m_scheduler;

    bool m_active = false;
    int m_id;
};

}  // namespace HoloJs
