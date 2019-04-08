#include "host-interfaces.h"
#include "include/holojs/private/script-host-utilities.h"

using namespace HoloJs::Interfaces;

JsValueRef Console::logStatic(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    return reinterpret_cast<Console*>(callbackData)->log(callee, arguments, argumentCount);
}

JsValueRef Console::debugStatic(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    return reinterpret_cast<Console*>(callbackData)->debug(callee, arguments, argumentCount);
}

HRESULT Console::initialize()
{
    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(L"log", L"system", logStatic, this, &m_logFunction));
    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(L"debug", L"system", debugStatic, this, &m_debugFunction));

    return S_OK;
}

JsValueRef Console::debug(JsValueRef callee, JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    const wchar_t* logString;
    size_t logStringLength;
    RETURN_INVALID_REF_IF_JS_ERROR(JsStringToPointer(arguments[1], &logString, &logStringLength));

    if (m_config == nullptr || m_config->callbackMode == ConsoleCallbackMode::RunDefaultBehavior) {
        OutputDebugString(logString);
    }

    if (m_config != nullptr) {
        m_config->onConsoleDebug(logString);
    }

    return JS_INVALID_REFERENCE;
}

JsValueRef Console::log(JsValueRef callee, JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    const wchar_t* logString;
    size_t logStringLength;
    RETURN_INVALID_REF_IF_JS_ERROR(JsStringToPointer(arguments[1], &logString, &logStringLength));

    if (m_config == nullptr || m_config->callbackMode == ConsoleCallbackMode::RunDefaultBehavior) {
        OutputDebugString(logString);
        wprintf(L"%s", logString);
    }

    if (m_config != nullptr) {
        m_config->onConsoleLog(logString);
    }

    return JS_INVALID_REFERENCE;
}