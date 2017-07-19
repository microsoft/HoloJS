#include "pch.h"
#include "Timers.h"
#include "ScriptErrorHandling.h"
#include "ScriptHostUtilities.h"
#include <concrt.h>

using namespace HologramJS::API;
using namespace concurrency;
using namespace std;
using namespace HologramJS::Utilities;

bool Timers::Initialize()
{
    RETURN_IF_FALSE(
        ScriptHostUtilities::ProjectFunction(L"setTimeout", L"timers", setTimeoutStatic, this, &m_setTimeoutFunction));
    RETURN_IF_FALSE(
        ScriptHostUtilities::ProjectFunction(L"clearTimer", L"timers", clearTimerStatic, this, &m_clearTimerFunction));
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(
        L"setInterval", L"timers", setIntervalStatic, this, &m_setIntervalFunction));

    return true;
}

Timers::TimerDefinition::TimerDefinition(TimerType type, int duration) : Type(type)
{
    Timer = make_unique<timer<int>>(duration, 0, nullptr, (type == TimerType::Timeout ? false : true));

    concurrency::task_completion_event<void> completionEvent;
    InternalCallback = make_unique<call<int>>([completionEvent](int) { completionEvent.set(); });

    // Connect the timer to the callback and start the timer.
    Timer->link_target(InternalCallback.get());
    Timer->start();
    Continuation = make_unique<task<void>>(completionEvent);

    ID = 0;
}

void Timers::TimerDefinition::Continue()
{
    // Create a new completion event;
    // For a periodic timer, Continue must be called every interval otherwise the task_completion_event won't
    // fire again
    concurrency::task_completion_event<void> completionEvent;
    InternalCallback = make_unique<call<int>>([completionEvent](int) { completionEvent.set(); });

    // Connect the timer to the callback and start the timer.
    Timer->link_target(InternalCallback.get());
    Continuation = make_unique<task<void>>(completionEvent);
    Continuation->then(NativeCallback, concurrency::task_continuation_context::use_current());
}

bool Timers::TimerDefinition::CaptureScriptResources(JsValueRef scriptCallback,
                                                     const std::vector<JsValueRef>& scriptCallbackParameters)
{
    ScriptCallback = scriptCallback;
    ScriptCallbackParameters = scriptCallbackParameters;

    // Increase the ref count to prevent garbage collection before the timer fires
    unsigned int refCount;
    RETURN_IF_JS_ERROR(JsAddRef(ScriptCallback, &refCount));

    for (auto& parameter : ScriptCallbackParameters) {
        RETURN_IF_JS_ERROR(JsAddRef(parameter, &refCount));
    }

    return true;
}

bool Timers::TimerDefinition::ReleaseScriptResources()
{
    // Release JS resources
    unsigned int refCount;

    if (ScriptCallback != JS_INVALID_REFERENCE) {
        RETURN_IF_JS_ERROR(JsRelease(ScriptCallback, &refCount));
    }

    for (const auto& parameter : ScriptCallbackParameters) {
        RETURN_IF_JS_ERROR(JsRelease(parameter, &refCount));
    }

    return true;
}

bool Timers::TimerDefinition::InvokeScriptCallback()
{
    // Call the script

    RETURN_IF_TRUE(ScriptCallback == JS_INVALID_REFERENCE);

    JsValueRef result;
    RETURN_ON_SCRIPT_ERROR(JsCallFunction(ScriptCallback,
                                          ScriptCallbackParameters.data(),
                                          static_cast<unsigned short>(ScriptCallbackParameters.size()),
                                          &result));

    return true;
}

JsValueRef Timers::ClearTimer(JsValueRef callee, JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_TRUE(argumentCount < 2);

    int timerId;
    RETURN_INVALID_REF_IF_JS_ERROR(JsNumberToInt(arguments[1], &timerId));

    shared_ptr<TimerDefinition> timer = ProcessTimersList(ListOperationType::Remove, timerId);
    if (!timer) {
        // The timeout must have fired already; return
        return JS_INVALID_REFERENCE;
    }

    timer->Stop();

    return JS_INVALID_REFERENCE;
}

JsValueRef Timers::CreateTimer(TimerType type, JsValueRef callee, JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount >= 3);

    int timeoutValue;
    RETURN_INVALID_REF_IF_JS_ERROR(JsNumberToInt(arguments[2], &timeoutValue));

    // Capture the parameters intended for the callback function;
    const JsValueRef scriptCallback = arguments[1];
    vector<JsValueRef> capturedParameters(argumentCount - 2);
    capturedParameters[0] = callee;
    for (int i = 3; i < argumentCount; i++) {
        capturedParameters[i - 2] = arguments[i];
    }

    shared_ptr<TimerDefinition> timer = make_shared<TimerDefinition>(type, timeoutValue);

    // Declare the script code to execute when the timer fires and what parameters to pass to it
    RETURN_INVALID_REF_IF_FALSE(timer->CaptureScriptResources(scriptCallback, capturedParameters));

    const int id = InsertInTimersList(timer);

    // Declare the native code to be executed when the timer fires
    timer->SetNativeCallback([this, id]() {
        shared_ptr<TimerDefinition> timer = ProcessTimersList(ListOperationType::ProcessTick, id);
        if (!timer) {
            // The timer must have been cleared; return
            return;
        }

        // Call the script
        timer->InvokeScriptCallback();

        if (timer->GetType() == TimerType::Interval) {
            timer->Continue();
        }
    });

    JsValueRef retValue;
    RETURN_INVALID_REF_IF_JS_ERROR(JsIntToNumber(timer->ID, &retValue));
    return retValue;
}

shared_ptr<Timers::TimerDefinition> Timers::ProcessTimersList(ListOperationType operationType, int id)
{
    shared_ptr<TimerDefinition> timer;

    // Lock the list of timers
    std::lock_guard<std::mutex> guard(m_timersLock);

    // Lookup timer in list;
    for (auto& timeoutInstance : m_timers) {
        if (id == timeoutInstance->ID) {
            // Found it
            timer = timeoutInstance;

            if ((operationType == ListOperationType::ProcessTick && timer->GetType() == TimerType::Timeout) ||
                operationType == ListOperationType::Remove) {
                m_timers.remove(timer);
            }

            break;
        }
    }

    return timer;
}

int Timers::InsertInTimersList(std::shared_ptr<TimerDefinition> timer)
{
    std::lock_guard<std::mutex> guard(m_timersLock);

    timer->ID = 0;

    // Determine a unique ID: largest ID + 1
    for (const auto& timerInstance : m_timers) {
        if (timer->ID <= timerInstance->ID) {
            timer->ID = timerInstance->ID + 1;
        }
    }

    m_timers.push_back(timer);

    return timer->ID;
}
