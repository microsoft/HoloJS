#pragma once

#include "stdafx.h"
#include "CppUnitTest.h"
#include "holojs/holojs.h"
#include "timer-tests.h"
#include <Windows.h>
#include <agents.h>
#include <ppltasks.h>
#include <string.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace HoloJs;
using namespace std;
using namespace concurrency;
using namespace libholojsunittest;

class TimeoutChecker : public IConsoleConfiguration {
   public:
    int expectedTimeout = 0;
    bool isStarted = false;
    unsigned long long startTime;
    int intervalCount = 0;

    virtual void onConsoleLog(const wchar_t* message)
    {
        if (!isStarted) {
            startTime = GetTickCount64();
            isStarted = true;
        } else {
            auto now = GetTickCount64();
            auto elapsed = now - startTime;
            Assert::IsTrue(elapsed > (expectedTimeout - 100) && elapsed < (expectedTimeout + 100));
            intervalCount++;
        }
    }

    virtual void onConsoleDebug(const wchar_t* message) {}
};

class IntervalChecker : public IConsoleConfiguration {
   public:
    int expectedInterval = 0;
    bool isStarted = false;
    unsigned long long lastTime;
    int intervalCount = 0;

    virtual void onConsoleLog(const wchar_t* message)
    {
        if (!isStarted) {
            lastTime = GetTickCount64();
            isStarted = true;
        } else {
            auto now = GetTickCount64();
            auto elapsed = now - lastTime;
            Assert::IsTrue(elapsed > (expectedInterval - 100) && elapsed < (expectedInterval + 100),
                           L"Interval out of bounds");
            lastTime = now;
            intervalCount++;
        }
    }

    virtual void onConsoleDebug(const wchar_t* message) {}
};

class ClearChecker : public IConsoleConfiguration {
   public:
    int fireCount = 0;

    virtual void onConsoleLog(const wchar_t* message) { fireCount++; }

    virtual void onConsoleDebug(const wchar_t* message) {}
};

void TimersTest::TimeoutTest()
{
    std::shared_ptr<HoloJs::IHoloJsScriptHost> scriptHost(
        HoloJs::CreateHoloJsScriptHost(), &HoloJs::DeleteHoloJsScriptHost);

    shared_ptr<TimeoutChecker> consoleConfig = make_shared<TimeoutChecker>();
    consoleConfig->expectedTimeout = 1000;
    scriptHost->setConsoleConfig(static_cast<IConsoleConfiguration*>(consoleConfig.get()));

	auto configuration = HoloJs::ViewConfiguration();
    Assert::IsTrue(SUCCEEDED(scriptHost->initialize(configuration)));

    // Create a task that returns false after the specified timeout.
    auto failure_task = complete_after(5000).then([scriptHost] { scriptHost->stopExecution(); });

    Assert::IsTrue(SUCCEEDED(
        scriptHost->start(L"console.log('start'); setTimeout(function(){console.log('timeout');}, 1000);")));

    Assert::IsTrue(consoleConfig->isStarted);
    Assert::IsTrue(consoleConfig->intervalCount == 1);
}

void TimersTest::IntervalTest()
{
    std::shared_ptr<HoloJs::IHoloJsScriptHost> scriptHost(
        HoloJs::CreateHoloJsScriptHost(), &HoloJs::DeleteHoloJsScriptHost);

    unique_ptr<IntervalChecker> consoleConfig = make_unique<IntervalChecker>();
    consoleConfig->expectedInterval = 1000;
    scriptHost->setConsoleConfig(static_cast<IConsoleConfiguration*>(consoleConfig.get()));

	auto configuration = HoloJs::ViewConfiguration();
    Assert::IsTrue(SUCCEEDED(scriptHost->initialize(configuration)), L"Failed to initialize script host");

    // Create a task that returns false after the specified timeout.

    auto failure_task = complete_after(10000).then([scriptHost] { scriptHost->stopExecution(); });

    scriptHost->start(
        L"console.log('start');\
          var maxCount = 5;\
          var counter = 0;\
          var timer = setInterval(function() {\
              counter++;\
              console.log('interval');\
              if (counter >= maxCount) \
                 clearInterval(timer);\
          }, 1000); ");

    Assert::IsTrue(consoleConfig->isStarted);
    Assert::IsTrue(consoleConfig->intervalCount >= 4 && consoleConfig->intervalCount <= 6);
}

void TimersTest::ClearTimerTest()
{
    std::shared_ptr<HoloJs::IHoloJsScriptHost> scriptHost(
        HoloJs::CreateHoloJsScriptHost(), &HoloJs::DeleteHoloJsScriptHost);

    unique_ptr<ClearChecker> consoleConfig = make_unique<ClearChecker>();
    scriptHost->setConsoleConfig(static_cast<IConsoleConfiguration*>(consoleConfig.get()));

	auto configuration = HoloJs::ViewConfiguration();
    Assert::IsTrue(SUCCEEDED(scriptHost->initialize(configuration)));

    // Create a task that returns false after the specified timeout.
    auto failure_task = complete_after(10000).then([scriptHost] { scriptHost->stopExecution(); });

    scriptHost->start(
        L"var timer1 = setTimeout(function() { console.log('fired'); }, 2000);\
          var interval1 = setInterval(function() { clearTimeout(timer1); }, 100);");

    Assert::IsTrue(consoleConfig->fireCount == 0);
}
