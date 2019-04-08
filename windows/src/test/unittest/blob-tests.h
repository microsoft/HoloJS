#pragma once

namespace libholojsunittest {
class BlobTests {
   public:
    // Test that console.log works as expected
    static void CreateFromArrayBufferView();
    static void CreateFromString();
    static void CreateFromArrayBufferNoType();
    static void CreateFromArrayBufferWithType();
    static void CreateFromArrayBufferWithoutOptions();
    static void CreateFromBlob();
    static void CreateFromBadBlob();
    static void CreateFromCombinationParts();
};
}  // namespace libholojsunittest