#include "pch.h"
#include "System.h"
#include "ScriptErrorHandling.h"
#include "ScriptHostUtilities.h"

using namespace HologramJS::API;
using namespace HologramJS::Utilities;
using namespace concurrency;
using namespace std;

void System::PromiseContinuationCallback(JsValueRef task)
{
    EXIT_IF_JS_ERROR(JsAddRef(task, nullptr));

    JsValueRef result;
    JsValueRef global;
    EXIT_IF_JS_ERROR(JsGetGlobalObject(&global));

    HANDLE_EXCEPTION_IF_JS_ERROR(JsCallFunction(task, &global, 1, &result));
    EXIT_IF_JS_ERROR(JsRelease(task, nullptr));
}

bool System::Initialize()
{
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"log", L"system", logStatic, this, &m_logFunction));

    JsSetPromiseContinuationCallback(StaticPromiseContinuationCallback, this);

    return true;
}

JsValueRef System::log(JsValueRef callee, JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    const wchar_t* logString;
    size_t logStringLength;
    RETURN_INVALID_REF_IF_JS_ERROR(JsStringToPointer(arguments[1], &logString, &logStringLength));

    OutputDebugString(logString);

    return JS_INVALID_REFERENCE;
}
