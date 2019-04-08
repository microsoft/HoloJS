#include "stdafx.h"
#include "holojs/holojs.h"
#include "xhr-tests.h"
#include <windows.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace HoloJs;
using namespace std;
using namespace concurrency;
using namespace libholojsunittest;

TEST_CLASS(XmlHttpRequest){public :

                               TEST_METHOD(XHRLoadBinaryTest){XHRTests::DownloadBinaryTest();
}
TEST_METHOD(XHRNotFoundTest) { XHRTests::DownloadNotFoundResourceTest(); }
TEST_METHOD(XHRLoadTextTest) { XHRTests::DownloadTextTest(); }
TEST_METHOD(XHROnReadyStateChangeTest) { XHRTests::OnReadyStateChangeTest(); }
TEST_METHOD(XHRGetResponseHeadersTest) { XHRTests::GetResponseHeadersTest(); }
TEST_METHOD(XHRPostTextTest) { XHRTests::PostTextTest(); }
TEST_METHOD(XHRPostBinaryTest) { XHRTests::PostBinaryTest(); }
}
;

class XHRChecker : public IConsoleConfiguration {
   public:
    bool loaded;
    bool textLoaded;
    bool binaryLoaded;
    bool notFound;
    bool onLoad;
    bool onError;
    bool onLoadEventHandler;
    bool onReadyStateChangeEventHandler;
    bool onErrorEventHandler;
    bool failureCase;

    virtual void onConsoleLog(const wchar_t* message)
    {
        if (wcscmp(message, L"loaded text") == 0) {
            textLoaded = true;
        } else if (wcscmp(message, L"loaded") == 0) {
            loaded = true;
        } else if (wcscmp(message, L"not found") == 0) {
            notFound = true;
        } else if (wcscmp(message, L"onload") == 0) {
            onLoad = true;
        } else if (wcscmp(message, L"onerror") == 0) {
            onError = true;
        } else if (wcscmp(message, L"binary loaded") == 0) {
            binaryLoaded = true;
        } else if (wcscmp(message, L"onload event handler") == 0) {
            onLoadEventHandler = true;
        } else if (wcscmp(message, L"onreadystatechange event handler") == 0) {
            onReadyStateChangeEventHandler = true;
        } else if (wcscmp(message, L"onerror event handler") == 0) {
            onErrorEventHandler = true;
        } else if (wcscmp(message, L"failure") == 0) {
            failureCase = true;
        }
    }

    virtual void onConsoleDebug(const wchar_t* message) {}
};

void XHRTests::DownloadTextTest()
{
    PCWSTR script =
        L"function reqListener () {								\
            console.log('onload');  		                    \
            if (this.response) {\
                console.log('binary loaded');\
            }\
			if (this.responseText) {    						\
                console.log(this.responseText);			        \
				console.log('loaded text');						\
			}													\
		}														\
        function onload() {console.log('onload event handler'); }\
        function onreadystatechange() {console.log('onreadystatechange event handler'); }\
																\
		var oReq = new XMLHttpRequest();						\
		oReq.addEventListener('load', reqListener);				\
        oReq.onload = onload; \
        oReq.onreadystatechange = onreadystatechange; \
		oReq.open('GET', 'http://localhost:8001/test-content/ballshooter.xrs');	\
		oReq.send();";

    std::shared_ptr<HoloJs::IHoloJsScriptHost> scriptHost(HoloJs::CreateHoloJsScriptHost(),
                                                          &HoloJs::DeleteHoloJsScriptHost);

    unique_ptr<XHRChecker> consoleConfig = make_unique<XHRChecker>();
    scriptHost->setConsoleConfig(static_cast<IConsoleConfiguration*>(consoleConfig.get()));

    auto configuration = HoloJs::ViewConfiguration();
    Assert::IsTrue(SUCCEEDED(scriptHost->initialize(configuration)), L"Failed to initialize script host");

    auto failure_task = complete_after(3000).then([scriptHost] { scriptHost->stopExecution(); });

    scriptHost->start(script);

    Assert::IsTrue(consoleConfig->textLoaded);
    Assert::IsTrue(consoleConfig->binaryLoaded);
    Assert::IsTrue(consoleConfig->onLoadEventHandler);
    Assert::IsTrue(consoleConfig->onReadyStateChangeEventHandler);
}

void XHRTests::DownloadBinaryTest()
{
    PCWSTR script =
        L"function onloaded () {\
            if (this.response) {\
                console.log('binary loaded');\
            }\
			if (this.responseText) {\
				console.log('loaded text');\
			}\
		}\
        function onerror() \
        { \
            console.log('onerror'); \
        } \
		var oReq = new XMLHttpRequest();\
        oReq.responseType = 'arraybuffer';\
		oReq.addEventListener('load', onloaded);\
        oReq.addEventListener('error', onerror);\
		oReq.open('GET', 'http://localhost:8001/test-content/ballshooter.xrs');\
		oReq.send();";

    std::shared_ptr<HoloJs::IHoloJsScriptHost> scriptHost(HoloJs::CreateHoloJsScriptHost(),
                                                          &HoloJs::DeleteHoloJsScriptHost);

    unique_ptr<XHRChecker> consoleConfig = make_unique<XHRChecker>();
    scriptHost->setConsoleConfig(static_cast<IConsoleConfiguration*>(consoleConfig.get()));

	auto configuration = HoloJs::ViewConfiguration();
    Assert::IsTrue(SUCCEEDED(scriptHost->initialize(configuration)), L"Failed to initialize script host");

    auto failure_task = complete_after(3000).then([scriptHost] { scriptHost->stopExecution(); });

    scriptHost->start(script);

    Assert::IsFalse(consoleConfig->textLoaded);
    Assert::IsTrue(consoleConfig->binaryLoaded);
    Assert::IsFalse(consoleConfig->onError);
}

void XHRTests::DownloadNotFoundResourceTest()
{
    PCWSTR script =
        L"function onloaded () {\
			console.log('onload');\
		} \
        function onerror() \
        { \
            console.log('onerror'); \
            if (this.status === 404) { \
                console.log('not found'); \
            } \
        } \
        function onerrorhandler() {console.log('onerror event handler'); }\
		var oReq = new XMLHttpRequest(); \
        oReq.responseType = 'arraybuffer'; \
		oReq.addEventListener('load', onloaded); \
        oReq.addEventListener('error', onerror); \
        oReq.onerror = onerrorhandler;\
		oReq.open('GET', 'http://localhost:8001/test-content/ballshooter1.xrs');\
		oReq.send();";

    std::shared_ptr<HoloJs::IHoloJsScriptHost> scriptHost(HoloJs::CreateHoloJsScriptHost(),
                                                          &HoloJs::DeleteHoloJsScriptHost);

    unique_ptr<XHRChecker> consoleConfig = make_unique<XHRChecker>();
    scriptHost->setConsoleConfig(static_cast<IConsoleConfiguration*>(consoleConfig.get()));

	auto configuration = HoloJs::ViewConfiguration();
    Assert::IsTrue(SUCCEEDED(scriptHost->initialize(configuration)), L"Failed to initialize script host");

    auto failure_task = complete_after(3000).then([scriptHost] { scriptHost->stopExecution(); });

    scriptHost->start(script);

    Assert::IsFalse(consoleConfig->onLoad);
    Assert::IsTrue(consoleConfig->onError);
    Assert::IsTrue(consoleConfig->notFound);
    Assert::IsTrue(consoleConfig->onErrorEventHandler);
}

void XHRTests::OnReadyStateChangeTest()
{
    PCWSTR script =
        L"function reqListener () { \
            console.log('onload'); \
            if (this.response) {\
                console.log('binary loaded');\
            }\
			if (this.responseText) { \
                console.log(this.responseText); \
				console.log('loaded text'); \
			} \
		} \
 \
		var oReq = new XMLHttpRequest(); \
		oReq.onreadystatechange = reqListener; \
		oReq.open('GET', 'http://localhost:8001/test-content/ballshooter.xrs');\
		oReq.send();";

    std::shared_ptr<HoloJs::IHoloJsScriptHost> scriptHost(HoloJs::CreateHoloJsScriptHost(),
                                                          &HoloJs::DeleteHoloJsScriptHost);

    unique_ptr<XHRChecker> consoleConfig = make_unique<XHRChecker>();
    scriptHost->setConsoleConfig(static_cast<IConsoleConfiguration*>(consoleConfig.get()));

	auto configuration = HoloJs::ViewConfiguration();
    Assert::IsTrue(SUCCEEDED(scriptHost->initialize(configuration)), L"Failed to initialize script host");

    auto failure_task = complete_after(3000).then([scriptHost] { scriptHost->stopExecution(); });

    scriptHost->start(script);

    Assert::IsTrue(consoleConfig->textLoaded);
    Assert::IsTrue(consoleConfig->binaryLoaded);
}

void XHRTests::GetResponseHeadersTest()
{
    PCWSTR script =
        L"function reqListener () { \
            let contentType = this.getResponseHeader('Content-type');\
            console.log(contentType);\
            if (contentType !== 'application/octet-stream') console.log('failure');\
            let unknownHeader = this.getResponseHeader('some-random-header');\
            if (unknownHeader !== null) {\
                console.log('failure');\
            }\
		} \
 \
		var oReq = new XMLHttpRequest(); \
		oReq.onreadystatechange = reqListener; \
		oReq.open('GET', 'http://localhost:8001/test-content/ballshooter.xrs');\
		oReq.send();";

    std::shared_ptr<HoloJs::IHoloJsScriptHost> scriptHost(HoloJs::CreateHoloJsScriptHost(),
                                                          &HoloJs::DeleteHoloJsScriptHost);

    unique_ptr<XHRChecker> consoleConfig = make_unique<XHRChecker>();
    scriptHost->setConsoleConfig(static_cast<IConsoleConfiguration*>(consoleConfig.get()));

	auto configuration = HoloJs::ViewConfiguration();
    Assert::IsTrue(SUCCEEDED(scriptHost->initialize(configuration)), L"Failed to initialize script host");

    auto failure_task = complete_after(3000).then([scriptHost] { scriptHost->stopExecution(); });

    scriptHost->start(script);
    Assert::IsFalse(consoleConfig->failureCase);
}

// For the post tests, run python post-server.py in the unittests directory
void XHRTests::PostBinaryTest()
{
    PCWSTR script =
        L"function reqListener () { \
            console.log('onreadystatechange event handler');\
		} \
 \
		var oReq = new XMLHttpRequest(); \
		oReq.onreadystatechange = reqListener; \
		oReq.open('POST', 'http://localhost:8001/test');\
		oReq.setRequestHeader('Content-type', 'application/binary');\
        oReq.send(new Float32Array(4));";

    std::shared_ptr<HoloJs::IHoloJsScriptHost> scriptHost(HoloJs::CreateHoloJsScriptHost(),
                                                          &HoloJs::DeleteHoloJsScriptHost);

    unique_ptr<XHRChecker> consoleConfig = make_unique<XHRChecker>();
    scriptHost->setConsoleConfig(static_cast<IConsoleConfiguration*>(consoleConfig.get()));

	auto configuration = HoloJs::ViewConfiguration();
    Assert::IsTrue(SUCCEEDED(scriptHost->initialize(configuration)), L"Failed to initialize script host");

    auto failure_task = complete_after(3000).then([scriptHost] { scriptHost->stopExecution(); });

    scriptHost->start(script);
    Assert::IsTrue(consoleConfig->onReadyStateChangeEventHandler);
}

void XHRTests::PostTextTest()
{
    PCWSTR script =
        L"function reqListener () { \
            console.log('onreadystatechange event handler');\
		} \
 \
		var oReq = new XMLHttpRequest(); \
		oReq.onreadystatechange = reqListener; \
		oReq.open('POST', 'http://localhost:8001/test');\
		oReq.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');\
        oReq.send('fname=Henry&lname=Ford');";

    std::shared_ptr<HoloJs::IHoloJsScriptHost> scriptHost(HoloJs::CreateHoloJsScriptHost(),
                                                          &HoloJs::DeleteHoloJsScriptHost);

    unique_ptr<XHRChecker> consoleConfig = make_unique<XHRChecker>();
    scriptHost->setConsoleConfig(static_cast<IConsoleConfiguration*>(consoleConfig.get()));

	auto configuration = HoloJs::ViewConfiguration();
    Assert::IsTrue(SUCCEEDED(scriptHost->initialize(configuration)), L"Failed to initialize script host");

    auto failure_task = complete_after(3000).then([scriptHost] { scriptHost->stopExecution(); });

    scriptHost->start(script);
    Assert::IsTrue(consoleConfig->onReadyStateChangeEventHandler);
}