#pragma once

#include "stdafx.h"
#include "CppUnitTest.h"
#include "console-tests.h"
#include "holojs/holojs.h"
#include <string.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace HoloJs;
using namespace std;

namespace libholojsunittest {
class ConsoleTestConfig : public IConsoleConfiguration {
   public:
    int logCount = 0;
    int debugCount = 0;
    const wchar_t* expectedLogMessage;
    const wchar_t* expectedDebugMessage;

    virtual void onConsoleLog(const wchar_t* message)
    {
        // Check that we got the expected message
        Assert::IsTrue(wcscmp(expectedLogMessage, message) == 0);
        logCount++;
    }

    virtual void onConsoleDebug(const wchar_t* message)
    {  // Check that we got the expected message
        Assert::IsTrue(wcscmp(expectedDebugMessage, message) == 0);
        debugCount++;
    }
};

void ConsoleTestsClass::RunConsoleLogTest()
{
    shared_ptr<ConsoleTestConfig> consoleConfig = make_shared<ConsoleTestConfig>();
    consoleConfig->expectedLogMessage = L"log something";
    shared_ptr<IHoloJsScriptHost> scriptHost(HoloJs::CreateHoloJsScriptHost(), &DeleteHoloJsScriptHost);

    scriptHost->setConsoleConfig(static_cast<IConsoleConfiguration*>(consoleConfig.get()));

	auto configuration = HoloJs::ViewConfiguration();
    Assert::IsTrue(SUCCEEDED(scriptHost->initialize(configuration)));

    complete_after(1000).then([scriptHost, consoleConfig] {
        scriptHost->execute(L"for (var i = 0; i < 25; i++) console.log('log something');");
        scriptHost->execute(L"for (var i = 0; i < 25; i++) console.log('log something');");
    });

    complete_after(1500).then([scriptHost, consoleConfig] {
        scriptHost->stopExecution();
    });

    Assert::IsTrue(SUCCEEDED(scriptHost->startWithEmptyApp()));

    Assert::AreEqual(50, consoleConfig->logCount);
}

void ConsoleTestsClass::RunConsoleDebugTest()
{
    shared_ptr<ConsoleTestConfig> consoleConfig = make_unique<ConsoleTestConfig>();
    consoleConfig->expectedDebugMessage = L"debug something";
    consoleConfig->expectedLogMessage = L"log something";
    shared_ptr<IHoloJsScriptHost> scriptHost(HoloJs::CreateHoloJsScriptHost(), &DeleteHoloJsScriptHost);

    scriptHost->setConsoleConfig(static_cast<IConsoleConfiguration*>(consoleConfig.get()));

	auto configuration = HoloJs::ViewConfiguration();
    Assert::IsTrue(SUCCEEDED(scriptHost->initialize(configuration)));

    complete_after(1000).then([scriptHost, consoleConfig] {
        scriptHost->execute(L"for (var i = 0; i < 25; i++) console.debug('debug something');");
        scriptHost->execute(L"console.log('log something');");
        scriptHost->execute(L"for (var i = 0; i < 25; i++) console.debug('debug something');");
    });

    complete_after(1500).then([scriptHost, consoleConfig] {
        scriptHost->stopExecution();
    });

    Assert::IsTrue(SUCCEEDED(scriptHost->startWithEmptyApp()));
    Assert::AreEqual(1, consoleConfig->logCount);
    Assert::AreEqual(50, consoleConfig->debugCount);
}
}  // namespace libholojsunittest
