#pragma once
namespace libholojsunittest {
class ImageTests {
   public:
    ImageTests() {}
    ~ImageTests() {}

    static void LoadImageTest();
    static void LoadUrlNotFoundTest();
    static void LoadInvalidUrlTest();
    static void loadDataUrlImage();
    static void loadInvalidDataUrlImage();
};
}  // namespace libholojsunittest