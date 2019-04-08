#pragma once

#include "stdafx.h"
#include "CppUnitTest.h"
#include "holojs/holojs.h"
#include "image-tests.h"
#include <agents.h>
#include <ppltasks.h>
#include <string.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace HoloJs;
using namespace std;
using namespace concurrency;
using namespace libholojsunittest;

TEST_CLASS(Image){public :

                              TEST_METHOD(LoadUrl){ImageTests::LoadImageTest();
}
TEST_METHOD(UrlNotFound) { ImageTests::LoadUrlNotFoundTest(); }
TEST_METHOD(InvalidUrl) { ImageTests::LoadInvalidUrlTest(); }
TEST_METHOD(DataUrl) { ImageTests::loadDataUrlImage(); }
TEST_METHOD(InvalidDataUrl) { ImageTests::loadInvalidDataUrlImage(); }
}
;

class ImageChecker : public IConsoleConfiguration {
   public:
    bool loaded = false;
    bool error = false;
    bool sizeMatch = false;

    virtual void onConsoleLog(const wchar_t* message)
    {
        if (_wcsicmp(message, L"loaded") == 0) {
            loaded = true;
        } else if (_wcsicmp(message, L"size match") == 0) {
            sizeMatch = true;
        } else if (_wcsicmp(message, L"error") == 0) {
            error = true;
        }
    }

    virtual void onConsoleDebug(const wchar_t* message) {}
};

void ImageTests::LoadImageTest()
{
    PCWSTR script =
        L"var img = new Image();\
        img.addEventListener('load', function() {\
            console.log('loaded');\
            if (img.width === 200 && img.height === 200) {\
                console.log('size match');\
            }\
        }, false);\
        img.src = 'https://upload.wikimedia.org/wikipedia/commons/thumb/4/44/Microsoft_logo.svg/200px-Microsoft_logo.svg.png';";

    std::shared_ptr<HoloJs::IHoloJsScriptHost> scriptHost(HoloJs::CreateHoloJsScriptHost(),
                                                          &HoloJs::DeleteHoloJsScriptHost);

    auto consoleConfig = make_shared<ImageChecker>();
    scriptHost->setConsoleConfig(static_cast<IConsoleConfiguration*>(consoleConfig.get()));

	auto configuration = HoloJs::ViewConfiguration();
    Assert::IsTrue(SUCCEEDED(scriptHost->initialize(configuration)));

    // Create a task that returns false after the specified timeout.
    auto failure_task = complete_after(5000).then([scriptHost] { scriptHost->stopExecution(); });

    Assert::IsTrue(SUCCEEDED(scriptHost->start(script)));
    Assert::IsTrue(consoleConfig->loaded);
    Assert::IsTrue(consoleConfig->sizeMatch);
}

void ImageTests::LoadUrlNotFoundTest()
{
    PCWSTR script =
        L"var img = new Image();\
        img.addEventListener('load', function() {\
            console.log('loaded');\
        }, false);\
        img.addEventListener('error', function() {\
            console.log('error');\
        }, false);\
        img.src = 'https://some.random.url.aaaaa12345.com/image.png';";

    std::shared_ptr<HoloJs::IHoloJsScriptHost> scriptHost(HoloJs::CreateHoloJsScriptHost(),
                                                          &HoloJs::DeleteHoloJsScriptHost);

    auto consoleConfig = make_shared<ImageChecker>();
    scriptHost->setConsoleConfig(static_cast<IConsoleConfiguration*>(consoleConfig.get()));

	auto configuration = HoloJs::ViewConfiguration();
    Assert::IsTrue(SUCCEEDED(scriptHost->initialize(configuration)));

    // Create a task that returns false after the specified timeout.
    auto failure_task = complete_after(5000).then([scriptHost] { scriptHost->stopExecution(); });

    Assert::IsTrue(SUCCEEDED(scriptHost->start(script)));
    Assert::IsTrue(consoleConfig->error);
    Assert::IsFalse(consoleConfig->loaded);
}
void ImageTests::LoadInvalidUrlTest() {
    PCWSTR script =
        L"var img = new Image();\
        img.addEventListener('load', function() {\
            console.log('loaded');\
        }, false);\
        img.addEventListener('error', function() {\
            console.log('error');\
        }, false);\
        img.src = 'foo.bar';";

    std::shared_ptr<HoloJs::IHoloJsScriptHost> scriptHost(HoloJs::CreateHoloJsScriptHost(),
                                                          &HoloJs::DeleteHoloJsScriptHost);

    auto consoleConfig = make_shared<ImageChecker>();
    scriptHost->setConsoleConfig(static_cast<IConsoleConfiguration*>(consoleConfig.get()));

	auto configuration = HoloJs::ViewConfiguration();
    Assert::IsTrue(SUCCEEDED(scriptHost->initialize(configuration)));

    // Create a task that returns false after the specified timeout.
    auto failure_task = complete_after(5000).then([scriptHost] { scriptHost->stopExecution(); });

    Assert::IsTrue(SUCCEEDED(scriptHost->start(script)));
    Assert::IsTrue(consoleConfig->error);
    Assert::IsFalse(consoleConfig->loaded);
}

void ImageTests::loadDataUrlImage()
{
    PCWSTR encodedImage =
        L"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAMgAAADICAMAAACahl6sAAAABGdBTUEAALGPC/"
        L"xhBQAAACBjSFJNAAB6JgAAgIQAAPoAAACA6AAAdTAAAOpgAAA6mAAAF3CculE8AAAATlBMVEXz8/"
        L"Pz6ebzyr7z7uzv8evW5bfs8OTzUyXz39nl7NWBvAbz8PDx8u/r8PPV6fPv8vPz8u/17NXz8eu23/IFpvD/ugj25Lfk7vP08OT///"
        L"++hpO+AAAAAWJLR0QZ7G61iAAAAAd0SU1FB+IECxQNICA2ZMEAAAD3SURBVHja7dxLDoIAEERB5KOAggKC3v+"
        L"kLmbHfkIk9Q7QSV2gi0KSdGCXMqkq9usmqesOUt6SamO/"
        L"65NqQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQM4Kqdqk7rH/"
        L"6JKqnfNI+uuGMaln7L+mpOYdZHknNcb+"
        L"tCa1gYCAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgJwV8lmSGmJ/"
        L"3pL6OueRpCP7ATrZHaLnWCQ+"
        L"AAAAJXRFWHRkYXRlOmNyZWF0ZQAyMDE4LTA0LTExVDIwOjEzOjMyKzAwOjAw0VuYWwAAACV0RVh0ZGF0ZTptb2RpZnkAMjAxOC0wNC0xMVQyM"
        L"DoxMzozMiswMDowMKAGIOcAAAAASUVORK5CYII=";

    PCWSTR scriptBase =
        L"var img = new Image();\
        img.addEventListener('load', function() {\
            console.log('loaded');\
            if (img.width === 200 && img.height === 200) {\
                console.log('size match');\
            }\
        }, false);\
        img.src = '";

    wstring script = scriptBase;
    script += encodedImage;
    script += L"';";

    std::shared_ptr<HoloJs::IHoloJsScriptHost> scriptHost(HoloJs::CreateHoloJsScriptHost(),
                                                          &HoloJs::DeleteHoloJsScriptHost);

    auto consoleConfig = make_shared<ImageChecker>();
    scriptHost->setConsoleConfig(static_cast<IConsoleConfiguration*>(consoleConfig.get()));

	auto configuration = HoloJs::ViewConfiguration();
    Assert::IsTrue(SUCCEEDED(scriptHost->initialize(configuration)));

    // Create a task that returns false after the specified timeout.
    auto failure_task = complete_after(5000).then([scriptHost] { scriptHost->stopExecution(); });

    Assert::IsTrue(SUCCEEDED(scriptHost->start(script.c_str())));
    Assert::IsTrue(consoleConfig->loaded);
    Assert::IsTrue(consoleConfig->sizeMatch);
}
void ImageTests::loadInvalidDataUrlImage()
{
    PCWSTR encodedImage =
        L"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAMgAAADICAMAAACahl6sAAAABGdBTUEAALGPC/"
        L"xhBQAAACBjSFJNAAB6JgAAgIQAAPoAAACA6AAAdTAAAOpgAAA6mAAAF3CculE8AAAATlBMVEXz8/"
        L"Pz6ebzyr7z7<some_random_text_in_here_to_break_base_64_or_image_format>/17NXz8eu23/IFpvD/ugj25Lfk7vP08OT///"
        L"++hpO+AAAAAWJLR0QZ7G61iAAAAAd0SU1FB+IECxQNICA2ZMEAAAD3SURBVHja7dxLDoIAEERB5KOAggKC3v+"
        L"kLmbHfkIk9Q7QSV2gi0KSdGCXMqkq9usmqesOUt6SamO/"
        L"65NqQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQM4Kqdqk7rH/"
        L"6JKqnfNI+uuGMaln7L+mpOYdZHknNcb+"
        L"tCa1gYCAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgJwV8lmSGmJ/"
        L"3pL6OueRpCP7ATrZHaLnWCQ+"
        L"AAAAJXRFWHRkYXRlOmNyZWF0ZQAyMDE4LTA0LTExVDIwOjEzOjMyKzAwOjAw0VuYWwAAACV0RVh0ZGF0ZTptb2RpZnkAMjAxOC0wNC0xMVQyM"
        L"DoxMzozMiswMDowMKAGIOcAAAAASUVORK5CYII=";

    PCWSTR scriptBase =
        L"var img = new Image();\
        img.addEventListener('load', function() {\
            console.log('loaded');\
        }, false);\
        img.addEventListener('error', function() {\
            console.log('error');\
        }, false);\
        img.src = '";

    wstring script = scriptBase;
    script += encodedImage;
    script += L"';";

    std::shared_ptr<HoloJs::IHoloJsScriptHost> scriptHost(HoloJs::CreateHoloJsScriptHost(),
                                                          &HoloJs::DeleteHoloJsScriptHost);

    auto consoleConfig = make_shared<ImageChecker>();
    scriptHost->setConsoleConfig(static_cast<IConsoleConfiguration*>(consoleConfig.get()));

	auto configuration = HoloJs::ViewConfiguration();
    Assert::IsTrue(SUCCEEDED(scriptHost->initialize(configuration)));

    // Create a task that returns false after the specified timeout.
    auto failure_task = complete_after(5000).then([scriptHost] { scriptHost->stopExecution(); });

    Assert::IsTrue(SUCCEEDED(scriptHost->start(script.c_str())));
    Assert::IsFalse(consoleConfig->loaded);
    Assert::IsTrue(consoleConfig->error);
}