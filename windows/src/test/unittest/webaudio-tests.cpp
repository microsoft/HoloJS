#include "stdafx.h"
#include "holojs/holojs.h"
#include "webaudio-tests.h"
#include <windows.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace HoloJs;
using namespace std;
using namespace concurrency;
using namespace libholojsunittest;

TEST_CLASS(WebAudio){public :

                                   TEST_METHOD(PlayOggFile){WebAudioTests::PlayOggFile();
}
}
;

class WebAudioChecker : public IConsoleConfiguration {
   public:
    bool decoded;
    bool error;
    bool ended;

    virtual void onConsoleLog(const wchar_t* message)
    {
        if (wcscmp(message, L"decoded") == 0) {
            decoded = true;
        } else if (wcscmp(message, L"error") == 0) {
            error = true;
        } else if (wcscmp(message, L"ended") == 0) {
            ended = true;
        } 
    }

    virtual void onConsoleDebug(const wchar_t* message) {}
};

void WebAudioTests::PlayOggFile()
{
    std::shared_ptr<HoloJs::IHoloJsScriptHost> scriptHost(HoloJs::CreateHoloJsScriptHost(),
                                                          &HoloJs::DeleteHoloJsScriptHost);

    unique_ptr<WebAudioChecker> consoleConfig = make_unique<WebAudioChecker>();
    scriptHost->setConsoleConfig(static_cast<IConsoleConfiguration*>(consoleConfig.get()));

	auto configuration = HoloJs::ViewConfiguration();
    Assert::IsTrue(SUCCEEDED(scriptHost->initialize(configuration)), L"Failed to initialize script host");

    auto failure_task = complete_after(10000).then([scriptHost] { scriptHost->stopExecution(); });

    scriptHost->startUri(L"http://localhost:8001/test-content/bouncing-sound.xrs");

    Assert::IsTrue(consoleConfig->decoded);
    Assert::IsTrue(consoleConfig->ended);
    Assert::IsFalse(consoleConfig->error);
}