#include "stdafx.h"
#include "holojs/private/error-handling.h"
#include "win32-timers-implementation.h"
#include <ppl.h>

using namespace HoloJs;
using namespace concurrency;
using namespace std;

void CALLBACK Timerproc(HWND Arg1, UINT Arg2, UINT_PTR Arg3, DWORD Arg4)
{
    auto context = reinterpret_cast<Win32TimerDefinition*>(Arg3);
    EXIT_IF_TRUE(context == nullptr);

    context->getScheduler()->processTimer(context);
}

Win32TimerDefinition ::~Win32TimerDefinition()
{
    stop();
    releaseScriptResources();
}

void Win32TimerDefinition::stop()
{
    if (m_active) {
        KillTimer(m_scheduler->getWindow(), reinterpret_cast<UINT_PTR>(this));
        m_active = false;
    }
}

HRESULT Win32TimerDefinition::start(int timeout)
{
    auto result = SetTimer(m_scheduler->getWindow(), reinterpret_cast<UINT_PTR>(this), timeout, &Timerproc);
    RETURN_IF_TRUE(result == 0);

    m_active = true;

    return S_OK;
}

HRESULT Win32TimerDefinition::captureScriptResources(JsValueRef scriptCallback,
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

HRESULT Win32TimerDefinition::releaseScriptResources()
{
    // Release JS resources
    unsigned int refCount;

    if (m_scriptCallback != JS_INVALID_REFERENCE) {
        RETURN_IF_JS_ERROR(JsRelease(m_scriptCallback, &refCount));
        m_scriptCallback = JS_INVALID_REFERENCE;
    }

    for (const auto& parameter : m_scriptCallbackParameters) {
        RETURN_IF_JS_ERROR(JsRelease(parameter, &refCount));
    }

    m_scriptCallbackParameters.clear();

    return S_OK;
}

HRESULT Win32TimerDefinition::invokeScriptCallback()
{
    // Call the script

    RETURN_IF_TRUE(m_scriptCallback == JS_INVALID_REFERENCE);

    JsValueRef result;

    RETURN_ON_SCRIPT_ERROR(JsCallFunction(m_scriptCallback,
                                          m_scriptCallbackParameters.data(),
                                          static_cast<unsigned short>(m_scriptCallbackParameters.size()),
                                          &result));

    return S_OK;
}

JsValueRef Win32Timers::clearTimer(int timerId)
{
    // Lookup timer in list;
    for (auto& timerInstance : m_timers) {
        if (timerId == timerInstance->getId()) {
            timerInstance->stop();
            m_timers.remove(timerInstance);

            break;
        }
    }

    return JS_INVALID_REFERENCE;
}

JsValueRef Win32Timers::createTimer(TimerType type,
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

    shared_ptr<Win32TimerDefinition> timer = make_shared<Win32TimerDefinition>(type, this);

    RETURN_INVALID_REF_IF_FAILED(timer->captureScriptResources(scriptCallback, capturedParameters));

    insertTimer(timer);
    timer->start(timeoutValue);

    JsValueRef retValue;
    RETURN_INVALID_REF_IF_JS_ERROR(JsIntToNumber(timer->getId(), &retValue));

    return retValue;
}

void Win32Timers::processTimer(Win32TimerDefinition* timer)
{
    EXIT_IF_TRUE(timer == nullptr);

    // Lookup timer in list;
    // If the timer is not found, the script cleared it while the WM_TIMER message was in the queue
    shared_ptr<Win32TimerDefinition> timerInList;
    for (auto timeoutInstance : m_timers) {
        if (timer->getId() == timeoutInstance->getId()) {
            timerInList = timeoutInstance;
            break;
        }
    }

    if (timerInList) {
        if (timer->GetType() != TimerType::Interval) {
            timer->stop();
        }

        timerInList->invokeScriptCallback();
    }
}

void Win32Timers::insertTimer(shared_ptr<Win32TimerDefinition> timer)
{
    timer->setId(0);

    // Determine a unique ID: largest ID + 1
    for (const auto& timerInstance : m_timers) {
        if (timer->getId() <= timerInstance->getId()) {
            timer->setId(timerInstance->getId() + 1);
        }
    }

    m_timers.push_back(timer);
}
