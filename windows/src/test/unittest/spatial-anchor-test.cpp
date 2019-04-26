#pragma once

#include "stdafx.h"
#include "CppUnitTest.h"
#include "spatial-anchor-test.h"
#include "holojs/holojs.h"
#include <string.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace HoloJs;
using namespace std;

using namespace libholojsunittest;

TEST_CLASS(SpatialAnchor)
{
   public:
    TEST_METHOD(createAnchor) { SpatialAnchorTests::createAnchor(); }
    }
    ;

class SpatialAnchorChecker : public IConsoleConfiguration {
   public:
    bool created;

    virtual void onConsoleLog(const wchar_t* message)
    {
        if (_wcsicmp(message, L"created") == 0) {
            created = true;
        }
    }

    virtual void onConsoleDebug(const wchar_t* message) {}
};

void SpatialAnchorTests::createAnchor()
{
    PCWSTR script =
        L"var anchor = new SpatialAnchor({ position : {x : 0.0, y : 0.0, z : 0.0}});\
        console.log('created');\
        ";

    shared_ptr<SpatialAnchorChecker> consoleConfig = make_shared<SpatialAnchorChecker>();
    shared_ptr<IHoloJsScriptHost> scriptHost(HoloJs::CreateHoloJsScriptHost(), &DeleteHoloJsScriptHost);

    scriptHost->setConsoleConfig(static_cast<IConsoleConfiguration*>(consoleConfig.get()));

	auto configuration = HoloJs::ViewConfiguration();
    Assert::IsTrue(SUCCEEDED(scriptHost->initialize(configuration)));

    complete_after(1000).then([scriptHost, consoleConfig] { scriptHost->stopExecution(); });

    Assert::IsTrue(SUCCEEDED(scriptHost->start(script)));

    Assert::IsTrue(consoleConfig->created);
}