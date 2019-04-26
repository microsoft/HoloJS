#pragma once

namespace libholojsunittest {
class WebSocketTest {
   public:
    static void CreateNoProtocol();
	static void CreateOneProtocol();
	static void CreateMultipleProtocols();
	static void CreateInvalidProtocolArg();
	static void CreateInvalidUrlArg();

	static void SendTextMessageTest();
	static void SendArrayBufferTest();
	static void SendBlobTest();

	static void Close();

	static void ConnectSuccess();
	static void ConnectBadUrl();
};
}  // namespace libholojsunittest