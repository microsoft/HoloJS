#include "stdafx.h"
#include <windows.h>
#include "event-target-tests.h"
#include "holojs/holojs.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace HoloJs;
using namespace std;
using namespace concurrency;
using namespace libholojsunittest;

class EventTestsChecker : public IConsoleConfiguration {
   public:
	   bool textLoaded;

    virtual void onConsoleLog(const wchar_t* message)
    {
		if (wcscmp(message, L"loaded text") == 0)
		{
			textLoaded = true;
		}
    }

    virtual void onConsoleDebug(const wchar_t* message) {}
};


void EventTargetTests::AddRemoveEventListenerTest()
{
	PCWSTR script =
		L"function listener1 () {								\
		}														\
		function listener2 () {  								\
		}														\
		function listener3 () {  								\
		}														\
																\
		var oReq = new XMLHttpRequest();						\
		oReq.addEventListener('load', listener1);				\
		oReq.addEventListener('load', listener1);				\
        oReq.addEventListener('load', listener2);				\
        oReq.addEventListener('error', listener3);				\
		oReq.removeEventListener('error', listener2);			\
		oReq.removeEventListener('load', listener2);			";
		//oReq.open('GET', 'http:/localhost:8000/vr-cubes.json');	\
		//oReq.send();";

	std::shared_ptr<HoloJs::IHoloJsScriptHost> scriptHost(
        HoloJs::CreateHoloJsScriptHost(), &HoloJs::DeleteHoloJsScriptHost);

    unique_ptr<EventTestsChecker> consoleConfig = make_unique<EventTestsChecker>();
    scriptHost->setConsoleConfig(static_cast<IConsoleConfiguration*>(consoleConfig.get()));

	auto configuration = HoloJs::ViewConfiguration();
    Assert::IsTrue(SUCCEEDED(scriptHost->initialize(configuration)), L"Failed to initialize script host");

	auto failure_task = complete_after(10000).then([scriptHost] { scriptHost->stopExecution(); });

	scriptHost->start(script);
}
