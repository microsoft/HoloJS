#pragma once

#include "ChakraForHoloJS.h"

namespace HologramJS {
class ScriptErrorHandling {
   public:
    static void PrintException()
    {
        // Get error message
        JsValueRef exception;
        JsGetAndClearException(&exception);

        JsPropertyIdRef messageName;
        JsGetPropertyIdFromName(L"stack", &messageName);

        JsValueRef messageValue;
        JsGetProperty(exception, messageName, &messageValue);

        const wchar_t *message;
        size_t length;
        JsStringToPointer(messageValue, &message, &length);

        std::wstring output = L"";
        output += message;
        output += L"\r\n";
        OutputDebugString(output.c_str());
    }
};
}  // namespace HologramJS