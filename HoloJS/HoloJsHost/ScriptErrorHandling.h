#pragma once

#include "ChakraForHoloJS.h"

namespace HologramJS {
class ScriptErrorHandling {
   public:
    static void PrintException()
    {
        // Get error message
        JsValueRef exception;
        EXIT_IF_JS_ERROR(JsGetAndClearException(&exception));

        JsPropertyIdRef messageName;
        EXIT_IF_JS_ERROR(JsGetPropertyIdFromName(L"stack", &messageName));

        JsValueRef messageValue;
        EXIT_IF_JS_ERROR(JsGetProperty(exception, messageName, &messageValue));

        const wchar_t *message;
        size_t length;
        EXIT_IF_JS_ERROR(JsStringToPointer(messageValue, &message, &length));

        std::wstring output = (message != nullptr ? message : L"No exception message available");
        output += L"\r\n";
        OutputDebugString(output.c_str());
    }
};
}  // namespace HologramJS