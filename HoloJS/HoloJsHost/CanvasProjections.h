#pragma once

#include "ChakraForHoloJS.h"

namespace HologramJS {
namespace Canvas {
class CanvasProjections {
   public:
    static bool Initialize();

   private:
    static Windows::UI::Color parseColor(JsValueRef);
    static Windows::Foundation::Rect parseRect(JsValueRef);
    static Windows::Foundation::Numerics::float2 parsePoint(JsValueRef);

    static JsValueRef CHAKRA_CALLBACK createContext2D(JsValueRef callee,
                                                      bool isConstructCall,
                                                      JsValueRef* arguments,
                                                      unsigned short argumentCount,
                                                      PVOID callbackData);

    static JsValueRef CHAKRA_CALLBACK drawImage(JsValueRef callee,
                                                bool isConstructCall,
                                                JsValueRef* arguments,
                                                unsigned short argumentCount,
                                                PVOID callbackData);

    static JsValueRef CHAKRA_CALLBACK clearRect(JsValueRef callee,
                                                bool isConstructCall,
                                                JsValueRef* arguments,
                                                unsigned short argumentCount,
                                                PVOID callbackData);

    static JsValueRef CHAKRA_CALLBACK fillRect(JsValueRef callee,
                                               bool isConstructCall,
                                               JsValueRef* arguments,
                                               unsigned short argumentCount,
                                               PVOID callbackData);

    static JsValueRef CHAKRA_CALLBACK fillRectGradient(JsValueRef callee,
                                                       bool isConstructCall,
                                                       JsValueRef* arguments,
                                                       unsigned short argumentCount,
                                                       PVOID callbackData);

    static JsValueRef CHAKRA_CALLBACK fillText(JsValueRef callee,
                                               bool isConstructCall,
                                               JsValueRef* arguments,
                                               unsigned short argumentCount,
                                               PVOID callbackData);

    static JsValueRef CHAKRA_CALLBACK getImageData(JsValueRef callee,
                                                   bool isConstructCall,
                                                   JsValueRef* arguments,
                                                   unsigned short argumentCount,
                                                   PVOID callbackData);

    static JsValueRef CHAKRA_CALLBACK getWidth(JsValueRef callee,
                                               bool isConstructCall,
                                               JsValueRef* arguments,
                                               unsigned short argumentCount,
                                               PVOID callbackData);

    static JsValueRef CHAKRA_CALLBACK setWidth(JsValueRef callee,
                                               bool isConstructCall,
                                               JsValueRef* arguments,
                                               unsigned short argumentCount,
                                               PVOID callbackData);

    static JsValueRef CHAKRA_CALLBACK getHeight(JsValueRef callee,
                                                bool isConstructCall,
                                                JsValueRef* arguments,
                                                unsigned short argumentCount,
                                                PVOID callbackData);

    static JsValueRef CHAKRA_CALLBACK setHeight(JsValueRef callee,
                                                bool isConstructCall,
                                                JsValueRef* arguments,
                                                unsigned short argumentCount,
                                                PVOID callbackData);
};
}  // namespace Canvas
}  // namespace HologramJS
