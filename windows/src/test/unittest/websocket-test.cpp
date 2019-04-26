#include "stdafx.h"
#include "CppUnitTest.h"
#include "holojs/holojs.h"
#include "websocket-test.h"
#include <string.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace HoloJs;
using namespace std;

using namespace libholojsunittest;

TEST_CLASS(WebSocket){public : 
	TEST_METHOD(CreateNoProcotol) { WebSocketTest::CreateNoProtocol(); }
	TEST_METHOD(CreateOneProtocol) { WebSocketTest::CreateOneProtocol(); }
	TEST_METHOD(CreateMultipleProtocols) { WebSocketTest::CreateMultipleProtocols(); }
	TEST_METHOD(CreateInvalidProtocolArg) { WebSocketTest::CreateInvalidProtocolArg(); }
	TEST_METHOD(CreateInvalidUrlArg) { WebSocketTest::CreateInvalidUrlArg(); }

	TEST_METHOD(SendTextMessage) { WebSocketTest::SendTextMessageTest(); }
	TEST_METHOD(SendArrayBufferMessage) { WebSocketTest::SendArrayBufferTest(); }
	TEST_METHOD(SendBlobMessage) { WebSocketTest::SendBlobTest(); }
	TEST_METHOD(Close) { WebSocketTest::Close(); }
TEST_METHOD(ConnectBadUrl) { WebSocketTest::ConnectBadUrl(); }
TEST_METHOD(ConnectSuccess) { WebSocketTest::ConnectSuccess(); }
}
;

class WebSocketChecker : public IConsoleConfiguration {
   public:
    bool connected;
	bool created;
	bool exception;
	bool error;
	bool close;
	bool receiveMessage;
	bool receiveMessageConfirmation;

    virtual void onConsoleLog(const wchar_t* message)
    {
        if (_wcsicmp(message, L"connected") == 0) {
            connected = true;
		}
		else if (_wcsicmp(message, L"created") == 0) {
			created = true;
		}
		else if (_wcsicmp(message, L"exception") == 0) {
			exception = true;
		}
		else if (_wcsicmp(message, L"error") == 0) {
			error = true;
		}
		else if (_wcsicmp(message, L"close") == 0) {
			close = true;
		}
		else if (_wcsicmp(message, L"server message") == 0) {
			receiveMessage = true;
		}
		else if (_wcsicmp(message, L"server echo") == 0) {
			receiveMessageConfirmation = true;
		}
    }

    virtual void onConsoleDebug(const wchar_t* message) {}
};

void RunScriptTest(PCWSTR script, shared_ptr<WebSocketChecker> console)
{
    shared_ptr<IHoloJsScriptHost> scriptHost(HoloJs::CreateHoloJsScriptHost(), &DeleteHoloJsScriptHost);

    scriptHost->setConsoleConfig(static_cast<IConsoleConfiguration*>(console.get()));

    auto configuration = HoloJs::ViewConfiguration();
	//scriptHost->enableDebugger();
    Assert::IsTrue(SUCCEEDED(scriptHost->initialize(configuration)));

    complete_after(3000).then([scriptHost] { scriptHost->stopExecution(); });

    Assert::IsTrue(SUCCEEDED(scriptHost->start(script)));
}

void WebSocketTest::CreateNoProtocol()
{
    PCWSTR script =
        L"var socket = new WebSocket('ws://localhost:9001/socketserver');\
		if (socket.readyState >= 0) {\
			console.log('created');\
		};\
        ";

    shared_ptr<WebSocketChecker> consoleConfig = make_shared<WebSocketChecker>();
    RunScriptTest(script, consoleConfig);
    Assert::IsTrue(consoleConfig->created);
}

void WebSocketTest::CreateOneProtocol()
{
    PCWSTR script =
        L"var socket = new WebSocket('ws://localhost:9001/socketserver', 'protocol');\
        if (socket.readyState >= 0) {\
			console.log('created');\
		};\
        ";

    shared_ptr<WebSocketChecker> consoleConfig = make_shared<WebSocketChecker>();
    RunScriptTest(script, consoleConfig);
    Assert::IsTrue(consoleConfig->created);
}

void WebSocketTest::CreateMultipleProtocols()
{
    PCWSTR script =
        L"var socket = new WebSocket('ws://localhost:9001/socketserver', ['protocol1', 'protocol2']);\
        if (socket.readyState >= 0) {\
			console.log('created');\
		};\
        ";

    shared_ptr<WebSocketChecker> consoleConfig = make_shared<WebSocketChecker>();
    RunScriptTest(script, consoleConfig);
    Assert::IsTrue(consoleConfig->created);
}

void WebSocketTest::CreateInvalidProtocolArg()
{
    PCWSTR script =
        L"try {\
			var socket = new WebSocket('ws://localhost:9001/socketserver', [12, 13]);\
			console.log('created');\
			} catch(e) {\
		      console.log('exception');\
			}\
        ";

    shared_ptr<WebSocketChecker> consoleConfig = make_shared<WebSocketChecker>();
    RunScriptTest(script, consoleConfig);
    Assert::IsFalse(consoleConfig->created);
	Assert::IsTrue(consoleConfig->exception);
}
void WebSocketTest::CreateInvalidUrlArg()
{
    PCWSTR script =
        L"try {\
			var socket = new WebSocket(1);\
			console.log('created');\
			} catch(e) {console.log('exception');}\
        ";

    shared_ptr<WebSocketChecker> consoleConfig = make_shared<WebSocketChecker>();
    RunScriptTest(script, consoleConfig);
    Assert::IsFalse(consoleConfig->created);
	Assert::IsTrue(consoleConfig->exception);
}

void WebSocketTest::ConnectSuccess()
{
    PCWSTR script =
        L"var socket = new WebSocket('ws://localhost:9001/socketserver', 'protocolOne');\
        if (socket.readyState >= 0) {\
			console.log('created');\
		};\
		socket.onopen = function (event) {\
			console.log('connected');\
		};\
		socket.onerror = function (event) {\
			console.log('error');\
		};\
		socket.onclose = function (event) {\
			console.log('close');\
		};\
		socket.onmessage = function (message) {\
			console.log(message);\
		};\
        ";

    shared_ptr<WebSocketChecker> consoleConfig = make_shared<WebSocketChecker>();
    RunScriptTest(script, consoleConfig);

    Assert::IsTrue(consoleConfig->connected);
	Assert::IsTrue(consoleConfig->created);
	Assert::IsTrue(consoleConfig->receiveMessage);
	Assert::IsFalse(consoleConfig->error);
}

void WebSocketTest::Close()
{
    PCWSTR script =
        L"var socket = new WebSocket('ws://localhost:9001/socketserver', 'protocolOne');\
        if (socket.readyState >= 0) {\
			console.log('created');\
		};\
		socket.onopen = function (event) {\
			console.log('connected');\
		};\
		socket.onerror = function (event) {\
			console.log('error');\
		};\
		socket.onclose = function (event) {\
			console.log('close');\
		};\
		socket.onmessage = function (message) {\
			console.log(message);\
			socket.close();\
		};\
        ";

    shared_ptr<WebSocketChecker> consoleConfig = make_shared<WebSocketChecker>();
    RunScriptTest(script, consoleConfig);

    Assert::IsTrue(consoleConfig->connected);
	Assert::IsTrue(consoleConfig->created);
	Assert::IsTrue(consoleConfig->receiveMessage);
	Assert::IsFalse(consoleConfig->error);
	Assert::IsTrue(consoleConfig->close);
}

void WebSocketTest::SendArrayBufferTest()
{
	PCWSTR script =
        L"var socket = new WebSocket('ws://demos.kaazing.com/echo', 'protocolOne');\
        if (socket.readyState >= 0) {\
			console.log('created');\
		};\
		socket.onopen = function (event) {\
			console.log('connected');\
			socket.send(new ArrayBuffer(8));\
		};\
		socket.onerror = function (event) {\
			console.log('error');\
		};\
		socket.onclose = function (event) {\
			console.log('close');\
		};\
		socket.onmessage = function (message) {\
			console.log('server echo');\
			console.log(message);\
		};\
        ";

    shared_ptr<WebSocketChecker> consoleConfig = make_shared<WebSocketChecker>();
    RunScriptTest(script, consoleConfig);

    Assert::IsTrue(consoleConfig->connected);
	Assert::IsTrue(consoleConfig->created);
	Assert::IsFalse(consoleConfig->error);
	Assert::IsTrue(consoleConfig->receiveMessageConfirmation);
}

void WebSocketTest::SendBlobTest()
{
	PCWSTR script =
        L"var socket = new WebSocket('ws://demos.kaazing.com/echo', 'protocolOne');\
        if (socket.readyState >= 0) {\
			console.log('created');\
		};\
		socket.onopen = function (event) {\
			console.log('connected');\
			var buffer = new Uint16Array([21, 31, 64]);\
			var blob = new Blob([buffer], {type : 'application/custom'});\
			socket.send(blob);\
		};\
		socket.onerror = function (event) {\
			console.log('error');\
		};\
		socket.onclose = function (event) {\
			console.log('close');\
		};\
		socket.onmessage = function (message) {\
			console.log('server echo');\
			console.log(message);\
		};\
        ";

    shared_ptr<WebSocketChecker> consoleConfig = make_shared<WebSocketChecker>();
    RunScriptTest(script, consoleConfig);

    Assert::IsTrue(consoleConfig->connected);
	Assert::IsTrue(consoleConfig->created);
	Assert::IsFalse(consoleConfig->error);
	Assert::IsTrue(consoleConfig->receiveMessageConfirmation);
}

void WebSocketTest::SendTextMessageTest()
{
    PCWSTR script =
        L"var socket = new WebSocket('ws://localhost:9001/socketserver', 'protocolOne');\
        if (socket.readyState >= 0) {\
			console.log('created');\
		};\
		socket.onopen = function (event) {\
			console.log('connected');\
			socket.send('client message');\
		};\
		socket.onerror = function (event) {\
			console.log('error');\
		};\
		socket.onclose = function (event) {\
			console.log('close');\
		};\
		socket.onmessage = function (message) {\
			console.log(message);\
		};\
        ";

    shared_ptr<WebSocketChecker> consoleConfig = make_shared<WebSocketChecker>();
    RunScriptTest(script, consoleConfig);

    Assert::IsTrue(consoleConfig->connected);
	Assert::IsTrue(consoleConfig->created);
	Assert::IsTrue(consoleConfig->receiveMessage);
	Assert::IsFalse(consoleConfig->error);
	Assert::IsTrue(consoleConfig->receiveMessageConfirmation);
}


void WebSocketTest::ConnectBadUrl()
{
	PCWSTR script =
		L"var socket = new WebSocket('ws://badhost/socketserver', 'protocolOne');\
        if (socket.readyState >= 0) {\
			console.log('created');\
		};\
		socket.onopen = function (event) {\
			console.log('connected');\
		};\
		socket.onerror = function (event) {\
			console.log('error');\
		};\
		socket.onclose = function (event) {\
			console.log('close');\
		};\
        ";

    shared_ptr<WebSocketChecker> consoleConfig = make_shared<WebSocketChecker>();
    RunScriptTest(script, consoleConfig);

	Assert::IsTrue(consoleConfig->created);
    Assert::IsFalse(consoleConfig->connected);
	Assert::IsTrue(consoleConfig->error);
	Assert::IsTrue(consoleConfig->close);
}