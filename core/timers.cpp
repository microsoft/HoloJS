#include "include/holojs/private/error-handling.h"
#include "include/holojs/private/script-host-utilities.h"
#include "include/holojs/private/timers.h"
#include "host-interfaces.h"

using namespace HoloJs;

HRESULT Timers::initialize()
{
    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"setTimeout", L"timers", setTimeoutStatic, this, &m_setTimeoutFunction));
    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"clearTimer", L"timers", clearTimerStatic, this, &m_clearTimerFunction));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"setInterval", L"timers", setIntervalStatic, this, &m_setIntervalFunction));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"clearInterval", L"timers", clearIntervalStatic, this, &m_clearIntervalFunction));

    return S_OK;
}