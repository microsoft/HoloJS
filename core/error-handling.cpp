#include "include/holojs/private/error-handling.h"
#include "include/holojs/private/chakra.h"
#include "include/holojs/private/platform-interfaces.h"
#include <string>

long HoloJs::Error = -1;
long HoloJs::Success = 0;

void Log(const char* file, int line)
{
    std::string completeOutput = "Failure in file ";
    completeOutput += file;
    completeOutput += ", line ";
    completeOutput += std::to_string(line);
    completeOutput += "\r\n";

    HoloJs::getPlatform()->debugLog(completeOutput.c_str());
}

void PrintException()
{
    // Get error message
    JsValueRef exception;
    EXIT_IF_JS_ERROR(JsGetAndClearException(&exception));

    JsPropertyIdRef messageName;
    EXIT_IF_JS_ERROR(JsGetPropertyIdFromName(L"stack", &messageName));

    JsValueRef messageValue;
    EXIT_IF_JS_ERROR(JsGetProperty(exception, messageName, &messageValue));

    const wchar_t* message;
    size_t length;
    EXIT_IF_JS_ERROR(JsStringToPointer(messageValue, &message, &length));

    std::wstring output = (message != nullptr ? message : L"No exception message available");
    output += L"\r\n";

    HoloJs::getPlatform()->debugLog(output.c_str());
}