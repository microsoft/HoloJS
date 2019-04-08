#include "stdafx.h"
#include "CppUnitTest.h"
#include "timer-tests.h"
#include "console-tests.h"
#include "web-loader-tests.h"
#include "xhr-tests.h"
#include "event-target-tests.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
// using namespace HoloJs;
using namespace std;
using namespace libholojsunittest;

TEST_CLASS(BasicTests){public : TEST_METHOD(ConsoleLogTest){ConsoleTestsClass::RunConsoleLogTest();
}  // namespace libholojsunittest

TEST_METHOD(ConsoleDebugTest) { ConsoleTestsClass::RunConsoleDebugTest(); }
TEST_METHOD(TimeoutTest) { TimersTest::TimeoutTest(); }
TEST_METHOD(IntervalTest) { TimersTest::IntervalTest(); }
TEST_METHOD(ClearTimersTest) { TimersTest::ClearTimerTest(); }
TEST_METHOD(WebLoadSuccessTest) { WebLoaderTests::WebLoadAppTest(); }
TEST_METHOD(WebLoadFailTest) { WebLoaderTests::FailWebLoadAppTest(); }

TEST_METHOD(EventListenerTests) { EventTargetTests::AddRemoveEventListenerTest(); }
}
;