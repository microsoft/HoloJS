#pragma once
namespace libholojsunittest {
class ResourceUtilizationTests {
   public:
    ResourceUtilizationTests() {}
    ~ResourceUtilizationTests() {}

    static void ScriptHostResourceReleaseTest();
    static void Canvas2DResourceRelease();
    static void CanvasImageDataRelease();
};
}  // namespace libholojsunittest