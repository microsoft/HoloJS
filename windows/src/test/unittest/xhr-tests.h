#pragma once
namespace libholojsunittest {
class XHRTests {
   public:
    XHRTests() {}
    ~XHRTests() {}

    static void DownloadBinaryTest();
    static void DownloadNotFoundResourceTest();
    static void DownloadTextTest();
    static void OnReadyStateChangeTest();
    static void GetResponseHeadersTest();
    static void PostTextTest();
    static void PostBinaryTest();
};
}  // namespace libholojsunittest