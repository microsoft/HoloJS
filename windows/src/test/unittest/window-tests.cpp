#include "stdafx.h"
#include "holojs/holojs.h"
#include "window-tests.h"
#include <windows.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace HoloJs;
using namespace std;
using namespace concurrency;
using namespace libholojsunittest;

TEST_CLASS(WindowTest){public :

                                   TEST_METHOD(CheckWindowElements){WindowTests::CheckWindowElements();
}
}
;

class WindowChecker : public IConsoleConfiguration {
   public:
    bool hasWindow;
    bool hasWidth;
    bool hasHeight;
    bool vSyncCalled;

    virtual void onConsoleLog(const wchar_t* message)
    {
        if (wcscmp(message, L"window") == 0) {
            hasWindow = true;
        } else if (wcscmp(message, L"width") == 0) {
            hasWidth = true;
        } else if (wcscmp(message, L"height") == 0) {
            hasHeight = true;
        } else if (wcscmp(message, L"vsync") == 0) {
            vSyncCalled = true;
        } 
    }

    virtual void onConsoleDebug(const wchar_t* message) {}
};

void WindowTests::CheckWindowElements()
{
    PCWSTR script =
        L"if (typeof window !== undefined) { console.log('window');}\
          if(typeof window.width !== undefined && typeof window.innerWidth !== undefined)  { console.log('width');}\
          if(typeof window.height !== undefined && typeof window.innerHeight !== undefined)  { console.log('height');}\
          window.addEventListener('resize', function () { console.log('resized'); }); \
          window.requestAnimationFrame(function () { console.log('vsync'); }); \
        ";

    std::shared_ptr<HoloJs::IHoloJsScriptHost> scriptHost(HoloJs::CreateHoloJsScriptHost(),
                                                          &HoloJs::DeleteHoloJsScriptHost);

    unique_ptr<WindowChecker> consoleConfig = make_unique<WindowChecker>();
    scriptHost->setConsoleConfig(static_cast<IConsoleConfiguration*>(consoleConfig.get()));

	auto configuration = HoloJs::ViewConfiguration();
    Assert::IsTrue(SUCCEEDED(scriptHost->initialize(configuration)), L"Failed to initialize script host");

    auto failure_task = complete_after(3000).then([scriptHost] { scriptHost->stopExecution(); });

    scriptHost->start(script);

    Assert::IsTrue(consoleConfig->hasWidth);
    Assert::IsTrue(consoleConfig->hasHeight);
    Assert::IsTrue(consoleConfig->hasWindow);
    Assert::IsTrue(consoleConfig->vSyncCalled);
}