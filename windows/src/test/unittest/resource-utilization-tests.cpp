#include "stdafx.h"
#include "holojs/holojs.h"
#include "resource-utilization-tests.h"
#include <windows.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace HoloJs;
using namespace std;
using namespace concurrency;
using namespace libholojsunittest;

TEST_CLASS(ResourceUtilization){
    public :

        TEST_METHOD(HostResourceRelease){ResourceUtilizationTests::ScriptHostResourceReleaseTest();
}
TEST_METHOD(Canvas2DResourceRelease) { ResourceUtilizationTests::Canvas2DResourceRelease(); }
TEST_METHOD(CanvasImageDataRelease) { ResourceUtilizationTests::CanvasImageDataRelease(); }
}
;

void ResourceUtilizationTests::ScriptHostResourceReleaseTest()
{
    PCWSTR script = L"console.log('loaded');";

    for (int i = 0; i < 100; i++) {
        std::shared_ptr<HoloJs::IHoloJsScriptHost> scriptHost(HoloJs::CreateHoloJsScriptHost(),
                                                              &HoloJs::DeleteHoloJsScriptHost);

		auto configuration = HoloJs::ViewConfiguration();
        Assert::IsTrue(SUCCEEDED(scriptHost->initialize(configuration)), L"Failed to initialize script host");

        auto failure_task = complete_after(2000).then([scriptHost] { scriptHost->stopExecution(); });

        scriptHost->start(script);
    }
}

// Create script host, canvas and getImageData, then release the script host. Repeat 100 times and make sure we're not
// running out of memory
void ResourceUtilizationTests::Canvas2DResourceRelease()
{
    PCWSTR script =
        L"let canvas = document.createElement('canvas');\
    canvas.width = 3000;\
    canvas.height = 3000;\
    let ctx = canvas.getContext('2d');\
    var image = ctx.getImageData(0, 0, 3000, 3000);";

    for (int i = 0; i < 100; i++) {
        std::shared_ptr<HoloJs::IHoloJsScriptHost> scriptHost(HoloJs::CreateHoloJsScriptHost(),
                                                              &HoloJs::DeleteHoloJsScriptHost);

		auto configuration = HoloJs::ViewConfiguration();
        Assert::IsTrue(SUCCEEDED(scriptHost->initialize(configuration)), L"Failed to initialize script host");

        auto failure_task = complete_after(2000).then([scriptHost] { scriptHost->stopExecution(); });

        scriptHost->start(script);
    }
}

// call canvas getImageData 1000 times and make sure we're not running out of memory
void ResourceUtilizationTests::CanvasImageDataRelease()
{
    PCWSTR script =
        L"let canvas = document.createElement('canvas');\
    canvas.width = 3000;\
    canvas.height = 3000;\
    let ctx = canvas.getContext('2d');\
    for (var i = 0; i < 1000; i++) {\
        let image = ctx.getImageData(0, 0, 3000, 3000);\
    };";

    std::shared_ptr<HoloJs::IHoloJsScriptHost> scriptHost(HoloJs::CreateHoloJsScriptHost(),
                                                          &HoloJs::DeleteHoloJsScriptHost);

	auto configuration = HoloJs::ViewConfiguration();
    Assert::IsTrue(SUCCEEDED(scriptHost->initialize(configuration)), L"Failed to initialize script host");

    auto failure_task = complete_after(2000).then([scriptHost] { scriptHost->stopExecution(); });

    scriptHost->start(script);
}