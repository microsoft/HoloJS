#include "stdafx.h"
#include "winrt-timers-implementation.h"
#include <ppl.h>

using namespace HoloJs::UWP;
using namespace concurrency;
using namespace Windows::System::Threading;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::UI::Core;
using namespace std;

WinRTTimers::TimerDefinition::TimerDefinition(int duration) : m_id(0) {}

HRESULT WinRTTimers::TimerDefinition::captureScriptResources(JsValueRef scriptCallback,
                                                             const std::vector<JsValueRef>& scriptCallbackParameters)
{
    m_scriptCallback = scriptCallback;
    m_scriptCallbackParameters = scriptCallbackParameters;

    // Increase the ref count to prevent garbage collection before the timer fires
    unsigned int refCount;
    RETURN_IF_JS_ERROR(JsAddRef(m_scriptCallback, &refCount));

    for (auto& parameter : m_scriptCallbackParameters) {
        RETURN_IF_JS_ERROR(JsAddRef(parameter, &refCount));
    }

    return S_OK;
}

HRESULT WinRTTimers::TimerDefinition::releaseScriptResources()
{
    // Release JS resources
    unsigned int refCount;

    if (m_scriptCallback != JS_INVALID_REFERENCE) {
        RETURN_IF_JS_ERROR(JsRelease(m_scriptCallback, &refCount));
    }

    for (const auto& parameter : m_scriptCallbackParameters) {
        RETURN_IF_JS_ERROR(JsRelease(parameter, &refCount));
    }

    return S_OK;
}

HRESULT WinRTTimers::TimerDefinition::invokeScriptCallback()
{
    // Call the script
    std::lock_guard<std::mutex> guard(m_timerLock);

    if (isCancelled()) {
        return S_OK;
    }

    RETURN_IF_TRUE(m_scriptCallback == JS_INVALID_REFERENCE);

    JsValueRef result;
    RETURN_ON_SCRIPT_ERROR(JsCallFunction(m_scriptCallback,
                                          m_scriptCallbackParameters.data(),
                                          static_cast<unsigned short>(m_scriptCallbackParameters.size()),
                                          &result));

    return S_OK;
}

void WinRTTimers::TimerDefinition::cancel()
{
    std::lock_guard<std::mutex> guard(m_timerLock);

    if (m_threadpoolTimer != nullptr) {
        m_threadpoolTimer->Cancel();
        m_threadpoolTimer = nullptr;
    }

    releaseScriptResources();

    m_isCancelled = true;
}

JsValueRef WinRTTimers::clearTimer(int timerId)
{
    for (const auto& timerInstance : m_timers) {
        if (timerId == timerInstance->m_id) {
            timerInstance->cancel();
            m_timers.remove(timerInstance);

            break;
        }
    }

    return JS_INVALID_REFERENCE;
}

JsValueRef WinRTTimers::createTimer(HoloJs::TimerType type,
                                    JsValueRef callee,
                                    JsValueRef* arguments,
                                    unsigned short argumentCount)
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

    shared_ptr<TimerDefinition> timer = make_shared<TimerDefinition>(timeoutValue);

    // Declare the script code to execute when the timer fires and what parameters to pass to it
    RETURN_INVALID_REF_IF_FAILED(timer->captureScriptResources(scriptCallback, capturedParameters));

    insertInTimersList(timer);

    ThreadPoolTimer ^ tpTimer;
    Windows::Foundation::TimeSpan period;
    period.Duration = timeoutValue * 1000 * 10;

    std::weak_ptr<TimerDefinition> timerWeakReference = timer;
    if (type == HoloJs::TimerType::Interval) {
        timer->setTimer(ThreadPoolTimer::CreatePeriodicTimer(
            ref new TimerElapsedHandler([timerWeakReference](ThreadPoolTimer ^ source) {
                CoreApplication::MainView->Dispatcher->RunAsync(CoreDispatcherPriority::Normal,
                                                                ref new DispatchedHandler([timerWeakReference]() {
                                                                    if (auto timer = timerWeakReference.lock()) {
                                                                        timer->invokeScriptCallback();
                                                                    }
                                                                }));
            }),
            period));
    } else {
        timer->setTimer(ThreadPoolTimer::CreateTimer(
            ref new TimerElapsedHandler([timerWeakReference](ThreadPoolTimer ^ source) {
                CoreApplication::MainView->Dispatcher->RunAsync(CoreDispatcherPriority::Normal,
                                                                ref new DispatchedHandler([timerWeakReference]() {
                                                                    if (auto timer = timerWeakReference.lock()) {
                                                                        timer->invokeScriptCallback();
                                                                    }
                                                                }));
            }),
            period));
    }

    JsValueRef retValue;
    RETURN_INVALID_REF_IF_JS_ERROR(JsIntToNumber(timer->m_id, &retValue));

    return retValue;
}

int WinRTTimers::insertInTimersList(std::shared_ptr<TimerDefinition> timer)
{
    timer->m_id = 0;

    // Determine a unique ID: largest ID + 1
    for (const auto& timerInstance : m_timers) {
        if (timer->m_id <= timerInstance->m_id) {
            timer->m_id = timerInstance->m_id + 1;
        }
    }

    m_timers.push_back(timer);

    return timer->m_id;
}
