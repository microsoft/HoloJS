#pragma once

#include "stdafx.h"
#include "CppUnitTest.h"
#include "holojs.h"
#include <string.h>


class ConsoleConfig : public IConsoleConfiguration {
   public:
    int logCount = 0;
    int debugCount = 0;
    const wchar_t* expectedMessage;

    virtual void onConsoleLog(const wchar_t* message)
    {
        Assert::IsTrue(wcscmp(expectedMessage, message) == 0);
        logCount++;
    }
    virtual void onConsoleDebug(const wchar_t* message) { debugCount++; }
};