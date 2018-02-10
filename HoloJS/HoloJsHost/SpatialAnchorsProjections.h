#pragma once

#include "ChakraForHoloJS.h"

namespace HologramJS {
namespace Spatial {
class SpatialAnchorsProjections {
   public:
    SpatialAnchorsProjections() {}
    ~SpatialAnchorsProjections() {}

    static bool Initialize(Windows::Perception::Spatial::SpatialStationaryFrameOfReference ^ frameOfReference);

   private:
    static JsValueRef CHAKRA_CALLBACK createAnchor(JsValueRef callee,
                                                   bool isConstructCall,
                                                   JsValueRef* arguments,
                                                   unsigned short argumentCount,
                                                   PVOID callbackData);

    static JsValueRef CHAKRA_CALLBACK enumerateAnchors(JsValueRef callee,
                                                       bool isConstructCall,
                                                       JsValueRef* arguments,
                                                       unsigned short argumentCount,
                                                       PVOID callbackData);

    static JsValueRef CHAKRA_CALLBACK openAnchor(JsValueRef callee,
                                                 bool isConstructCall,
                                                 JsValueRef* arguments,
                                                 unsigned short argumentCount,
                                                 PVOID callbackData);

    static Windows::Perception::Spatial::SpatialStationaryFrameOfReference ^ m_frameOfReference;

    concurrency::task<void> enumerateAnchorsAsync(JsValueRef callback);
    concurrency::task<void> openAnchorAsync(const std::wstring anchorName, JsValueRef callback);
};

}  // namespace Spatial
}  // namespace HologramJS
