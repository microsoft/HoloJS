#include "host-interfaces.h"

using namespace HoloJs::Interfaces;

void PromiseContinuation::staticPromiseContinuationCallback(JsValueRef task, void* callbackState)
{
    reinterpret_cast<PromiseContinuation*>(callbackState)->promiseContinuationCallback(task);
}

void PromiseContinuation::promiseContinuationCallback(JsValueRef task)
{
    EXIT_IF_JS_ERROR(JsAddRef(task, nullptr));
    auto autoReleaseTask = JsRefReleaseAtScopeExit(task);

    JsValueRef result;
    JsValueRef global;
    EXIT_IF_JS_ERROR(JsGetGlobalObject(&global));

    HANDLE_EXCEPTION_IF_JS_ERROR(JsCallFunction(task, &global, 1, &result));
}

HRESULT PromiseContinuation::initialize()
{
    RETURN_IF_JS_ERROR(JsSetPromiseContinuationCallback(staticPromiseContinuationCallback, this));
    return S_OK;
}