#pragma once

#include "stdafx.h"
#include "CppUnitTest.h"
#include "blob-tests.h"
#include "holojs/holojs.h"
#include <string.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace HoloJs;
using namespace std;

using namespace libholojsunittest;

TEST_CLASS(Blob)
{
   public:
    TEST_METHOD(CreateFromArrayBufferView) { BlobTests::CreateFromArrayBufferView(); }
    TEST_METHOD(CreateFromString) { BlobTests::CreateFromString(); }
    TEST_METHOD(CreateFromArrayBufferNoOptions) { BlobTests::CreateFromArrayBufferWithoutOptions(); }
    TEST_METHOD(CreateFromArrayBufferWithType) { BlobTests::CreateFromArrayBufferWithType(); }
    TEST_METHOD(CreateFromArrayBufferNoType) { BlobTests::CreateFromArrayBufferNoType(); }
    TEST_METHOD(CreateFromCombinationParts) { BlobTests::CreateFromCombinationParts(); }
    TEST_METHOD(CreateFromBlob) { BlobTests::CreateFromBlob(); }
    TEST_METHOD(CreateFromBadBlob) { BlobTests::CreateFromBadBlob(); }
    }
    ;

class BlobChecker : public IConsoleConfiguration {
   public:
    bool sizeMatch;
    bool sizeMismatch;
    bool typeMatch;
    bool exception;
    bool unexpected;
    bool complete;

    virtual void onConsoleLog(const wchar_t* message)
    {
        if (_wcsicmp(message, L"type match") == 0) {
            typeMatch = true;
        } else if (_wcsicmp(message, L"size match") == 0) {
            sizeMatch = true;
        } else if (_wcsicmp(message, L"size mismatch") == 0) {
            sizeMatch = true;
        } else if (_wcsicmp(message, L"unexpected") == 0) {
            unexpected = true;
        } else if (_wcsicmp(message, L"exception") == 0) {
            exception = true;
        } else if (_wcsicmp(message, L"complete") == 0) {
            complete = true;
        }
    }

    virtual void onConsoleDebug(const wchar_t* message) {}
};

void BlobTests::CreateFromArrayBufferView()
{
    PCWSTR script =
        L"var buffer = new Uint16Array([21, 31, 64]);\
        var blob = new Blob([buffer], {type : 'application/custom'});\
        if (blob.size === 6) console.log('size match');\
        if (blob.type === 'application/custom') console.log('type match');\
        console.log('complete');\
        ";

    shared_ptr<BlobChecker> consoleConfig = make_shared<BlobChecker>();
    shared_ptr<IHoloJsScriptHost> scriptHost(HoloJs::CreateHoloJsScriptHost(), &DeleteHoloJsScriptHost);

    scriptHost->setConsoleConfig(static_cast<IConsoleConfiguration*>(consoleConfig.get()));

	auto configuration = HoloJs::ViewConfiguration();
    Assert::IsTrue(SUCCEEDED(scriptHost->initialize(configuration)));

    complete_after(1000).then([scriptHost, consoleConfig] { scriptHost->stopExecution(); });

    Assert::IsTrue(SUCCEEDED(scriptHost->start(script)));

    Assert::IsTrue(consoleConfig->sizeMatch);
    Assert::IsTrue(consoleConfig->typeMatch);
    Assert::IsTrue(consoleConfig->complete);
}

void BlobTests::CreateFromString()
{
    PCWSTR script =
        L"var blob = new Blob(['12345678'], {endings : 'transparent'});\
        if (blob.size === 16) console.log('size match');\
        if (blob.type === '') console.log('type match');\
        console.log('complete');\
        ";

    shared_ptr<BlobChecker> consoleConfig = make_shared<BlobChecker>();
    shared_ptr<IHoloJsScriptHost> scriptHost(HoloJs::CreateHoloJsScriptHost(), &DeleteHoloJsScriptHost);

    scriptHost->setConsoleConfig(static_cast<IConsoleConfiguration*>(consoleConfig.get()));

	auto configuration = HoloJs::ViewConfiguration();
    Assert::IsTrue(SUCCEEDED(scriptHost->initialize(configuration)));

    complete_after(1000).then([scriptHost, consoleConfig] { scriptHost->stopExecution(); });

    Assert::IsTrue(SUCCEEDED(scriptHost->start(script)));

    Assert::IsTrue(consoleConfig->sizeMatch);
    Assert::IsTrue(consoleConfig->typeMatch);
    Assert::IsTrue(consoleConfig->complete);
}

void BlobTests::CreateFromArrayBufferNoType() {
    PCWSTR script =
        L"var buffer = new ArrayBuffer(8);\
        var blob = new Blob([buffer], {endings : 'transparent'});\
        if (blob.size === 8) console.log('size match');\
        if (blob.type === '') console.log('type match');\
        console.log('complete');\
        ";

    shared_ptr<BlobChecker> consoleConfig = make_shared<BlobChecker>();
    shared_ptr<IHoloJsScriptHost> scriptHost(HoloJs::CreateHoloJsScriptHost(), &DeleteHoloJsScriptHost);

    scriptHost->setConsoleConfig(static_cast<IConsoleConfiguration*>(consoleConfig.get()));

	auto configuration = HoloJs::ViewConfiguration();
    Assert::IsTrue(SUCCEEDED(scriptHost->initialize(configuration)));

    complete_after(1000).then([scriptHost, consoleConfig] { scriptHost->stopExecution(); });

    Assert::IsTrue(SUCCEEDED(scriptHost->start(script)));

    Assert::IsTrue(consoleConfig->sizeMatch);
    Assert::IsTrue(consoleConfig->typeMatch);
    Assert::IsTrue(consoleConfig->complete);
}

void BlobTests::CreateFromArrayBufferWithoutOptions()
{
    PCWSTR script =
        L"var buffer = new ArrayBuffer(8);\
        var blob = new Blob([buffer]);\
        if (blob.size === 8) console.log('size match');\
        if (blob.type === '') console.log('type match');\
        console.log('complete');\
        ";

    shared_ptr<BlobChecker> consoleConfig = make_shared<BlobChecker>();
    shared_ptr<IHoloJsScriptHost> scriptHost(HoloJs::CreateHoloJsScriptHost(), &DeleteHoloJsScriptHost);

    scriptHost->setConsoleConfig(static_cast<IConsoleConfiguration*>(consoleConfig.get()));

	auto configuration = HoloJs::ViewConfiguration();
    Assert::IsTrue(SUCCEEDED(scriptHost->initialize(configuration)));

    complete_after(1000).then([scriptHost, consoleConfig] { scriptHost->stopExecution(); });

    Assert::IsTrue(SUCCEEDED(scriptHost->start(script)));

    Assert::IsTrue(consoleConfig->sizeMatch);
    Assert::IsTrue(consoleConfig->typeMatch);
    Assert::IsTrue(consoleConfig->complete);
}

void BlobTests::CreateFromArrayBufferWithType()
{
    PCWSTR script =
        L"var buffer = new ArrayBuffer(8);\
        var blob = new Blob([buffer], {type : 'application/binary'});\
        if (blob.size === 8) console.log('size match');\
        if (blob.type === 'application/binary') console.log('type match');\
        console.log('complete');\
        ";

    shared_ptr<BlobChecker> consoleConfig = make_shared<BlobChecker>();
    shared_ptr<IHoloJsScriptHost> scriptHost(HoloJs::CreateHoloJsScriptHost(), &DeleteHoloJsScriptHost);

    scriptHost->setConsoleConfig(static_cast<IConsoleConfiguration*>(consoleConfig.get()));

	auto configuration = HoloJs::ViewConfiguration();
    Assert::IsTrue(SUCCEEDED(scriptHost->initialize(configuration)));

    complete_after(1000).then([scriptHost, consoleConfig] { scriptHost->stopExecution(); });

    Assert::IsTrue(SUCCEEDED(scriptHost->start(script)));

    Assert::IsTrue(consoleConfig->sizeMatch);
    Assert::IsTrue(consoleConfig->typeMatch);
    Assert::IsTrue(consoleConfig->complete);
}

void BlobTests::CreateFromBlob()
{
    PCWSTR script =
        L"var buffer = new Uint16Array([21, 31, 64]);\
        var origin_blob = new Blob([buffer], {type : 'application/custom'});\
        var blob = new Blob([origin_blob], {type : 'application/blobfromblob'});\
        if (blob.size === 6) console.log('size match');\
        if (blob.type === 'application/blobfromblob') console.log('type match');\
        console.log('complete');\
        ";

    shared_ptr<BlobChecker> consoleConfig = make_shared<BlobChecker>();
    shared_ptr<IHoloJsScriptHost> scriptHost(HoloJs::CreateHoloJsScriptHost(), &DeleteHoloJsScriptHost);

    scriptHost->setConsoleConfig(static_cast<IConsoleConfiguration*>(consoleConfig.get()));

	auto configuration = HoloJs::ViewConfiguration();
    Assert::IsTrue(SUCCEEDED(scriptHost->initialize(configuration)));

    complete_after(1000).then([scriptHost, consoleConfig] { scriptHost->stopExecution(); });

    Assert::IsTrue(SUCCEEDED(scriptHost->start(script)));

    Assert::IsTrue(consoleConfig->sizeMatch);
    Assert::IsTrue(consoleConfig->typeMatch);
    Assert::IsTrue(consoleConfig->complete);
}

void BlobTests::CreateFromBadBlob()
{
    PCWSTR script =
        L"try {\
            var blob = new Blob([{native : window.native} ], {type : 'application/blobfromblob'});\
            console.log('unexpected');\
        }\
        catch (e)\
        { console.log('exception');}";

    shared_ptr<BlobChecker> consoleConfig = make_shared<BlobChecker>();
    shared_ptr<IHoloJsScriptHost> scriptHost(HoloJs::CreateHoloJsScriptHost(), &DeleteHoloJsScriptHost);

    scriptHost->setConsoleConfig(static_cast<IConsoleConfiguration*>(consoleConfig.get()));

	auto configuration = HoloJs::ViewConfiguration();
    Assert::IsTrue(SUCCEEDED(scriptHost->initialize(configuration)));

    complete_after(1000).then([scriptHost, consoleConfig] { scriptHost->stopExecution(); });

    Assert::IsTrue(SUCCEEDED(scriptHost->start(script)));

    Assert::IsTrue(consoleConfig->exception);
    Assert::IsFalse(consoleConfig->unexpected);
}


void BlobTests::CreateFromCombinationParts() {
    PCWSTR script =
        L"var buffer = new Uint16Array([21, 31, 64]);\
        var origin_blob = new Blob([buffer], {type : 'application/custom'});\
        if (origin_blob.size !== 6) console.log('size mismatch');\
        var blob = new Blob([origin_blob], {type : 'application/blobfromblob'});\
        if (blob.size !== 6) console.log('size mismatch');\
        if (blob.type === 'application/blobfromblob') console.log('type match');\
        var compoundBlob = new Blob([origin_blob, blob, buffer, '12345678'], {type : 'application/compound'});\
        if (compoundBlob.size !== 34) console.log('size mismatch');\
        console.log('complete');\
        ";

    shared_ptr<BlobChecker> consoleConfig = make_shared<BlobChecker>();
    shared_ptr<IHoloJsScriptHost> scriptHost(HoloJs::CreateHoloJsScriptHost(), &DeleteHoloJsScriptHost);

    scriptHost->setConsoleConfig(static_cast<IConsoleConfiguration*>(consoleConfig.get()));

	auto configuration = HoloJs::ViewConfiguration();
    Assert::IsTrue(SUCCEEDED(scriptHost->initialize(configuration)));

    complete_after(1000).then([scriptHost, consoleConfig] { scriptHost->stopExecution(); });

    Assert::IsTrue(SUCCEEDED(scriptHost->start(script)));

    Assert::IsFalse(consoleConfig->sizeMismatch);
    Assert::IsTrue(consoleConfig->complete);
}
