#pragma once

#include "ChakraForHoloJS.h"
#include "SpatialAnchor.h"

namespace HologramJS {
namespace Spatial {
class SpatialAnchorsProjections {
   public:
    SpatialAnchorsProjections() {}
    ~SpatialAnchorsProjections() {}

    static bool Initialize();

    static void SetStationaryFrameOfReference(Windows::Perception::Spatial::SpatialStationaryFrameOfReference ^
                                              frameOfReference)
    {
        m_frameOfReference = frameOfReference;
    }

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

    static JsValueRef CHAKRA_CALLBACK saveAnchor(JsValueRef callee,
                                                 bool isConstructCall,
                                                 JsValueRef* arguments,
                                                 unsigned short argumentCount,
                                                 PVOID callbackData);

    static JsValueRef CHAKRA_CALLBACK deleteAnchor(JsValueRef callee,
                                                   bool isConstructCall,
                                                   JsValueRef* arguments,
                                                   unsigned short argumentCount,
                                                   PVOID callbackData);

    static JsValueRef CHAKRA_CALLBACK importAnchors(JsValueRef callee,
                                                   bool isConstructCall,
                                                   JsValueRef* arguments,
                                                   unsigned short argumentCount,
                                                   PVOID callbackData);

    static JsValueRef CHAKRA_CALLBACK exportAnchor(JsValueRef callee,
                                                   bool isConstructCall,
                                                   JsValueRef* arguments,
                                                   unsigned short argumentCount,
                                                   PVOID callbackData);

    static JsValueRef CHAKRA_CALLBACK getTransformToOrigin(JsValueRef callee,
                                                           bool isConstructCall,
                                                           JsValueRef* arguments,
                                                           unsigned short argumentCount,
                                                           PVOID callbackData);

    static Windows::Perception::Spatial::SpatialStationaryFrameOfReference ^ m_frameOfReference;

    static concurrency::task<void> enumerateAnchorsAsync(JsValueRef callback);
    static concurrency::task<void> openAnchorAsync(const std::wstring anchorName, JsValueRef callback);
    static concurrency::task<void> deleteAnchorAsync(const std::wstring anchorName, JsValueRef callback);
    static concurrency::task<void> importAnchorsAsync(JsValueRef dataRef, Windows::Storage::Streams::IBuffer ^ anchorBuffer,
                                                     JsValueRef callback);
};

}  // namespace Spatial
}  // namespace HologramJS
