#include "stdafx.h"
#include "CppUnitTest.h"
#include "holojs/holojs.h"
#include "web-loader-tests.h"
#include <Windows.h>
#include <agents.h>
#include <ppltasks.h>
#include <string.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace HoloJs;
using namespace std;
using namespace concurrency;
using namespace libholojsunittest;

class WebLoaderChecker : public IConsoleConfiguration {
   public:
	   int loadCount = 0;

    virtual void onConsoleLog(const wchar_t* message)
    {
		if (wcscmp(message, L"loaded") == 0)
		{
			loadCount++;
		}
    }

    virtual void onConsoleDebug(const wchar_t* message) {}
};

void WebLoaderTests::WebLoadAppTest()
{
    std::shared_ptr<HoloJs::IHoloJsScriptHost> scriptHost(
        HoloJs::CreateHoloJsScriptHost(), &HoloJs::DeleteHoloJsScriptHost);

    shared_ptr<WebLoaderChecker> consoleConfig = make_shared<WebLoaderChecker>();
    scriptHost->setConsoleConfig(static_cast<IConsoleConfiguration*>(consoleConfig.get()));

	auto configuration = HoloJs::ViewConfiguration();
    Assert::IsTrue(SUCCEEDED(scriptHost->initialize(configuration)));

    // Create a task that returns false after the specified timeout.
    auto failure_task = complete_after(5000).then([scriptHost] { scriptHost->stopExecution(); });

    Assert::IsTrue(SUCCEEDED(
        scriptHost->startUri(L"http://localhost:8001/test-content/ballshooter.xrs")));

    scriptHost->stopExecution();

    Assert::IsTrue(consoleConfig->loadCount == 1);
}

void WebLoaderTests::FailWebLoadAppTest()
{
    std::shared_ptr<HoloJs::IHoloJsScriptHost> scriptHost(
        HoloJs::CreateHoloJsScriptHost(), &HoloJs::DeleteHoloJsScriptHost);

    shared_ptr<WebLoaderChecker> consoleConfig = make_shared<WebLoaderChecker>();
    scriptHost->setConsoleConfig(static_cast<IConsoleConfiguration*>(consoleConfig.get()));

	auto configuration = HoloJs::ViewConfiguration();
    Assert::IsTrue(SUCCEEDED(scriptHost->initialize(configuration)));

    auto failure_task = complete_after(5000).then([scriptHost] { scriptHost->stopExecution(); });

    Assert::IsTrue(SUCCEEDED(
        scriptHost->startUri(L"http://localhost:8000/spinning-cube1.xrs")));

    Assert::IsTrue(consoleConfig->loadCount == 0);
}