#pragma once

#include "ChakraForHoloJS.h"

namespace HologramJS {
namespace Audio {
class AudioProjections {
   public:
    static bool Initialize();

   private:
    static JsValueRef CHAKRA_CALLBACK createContext(JsValueRef callee,
                                                    bool isConstructCall,
                                                    JsValueRef* arguments,
                                                    unsigned short argumentCount,
                                                    PVOID callbackData);

    static JsValueRef CHAKRA_CALLBACK decodeAudioData(JsValueRef callee,
                                                      bool isConstructCall,
                                                      JsValueRef* arguments,
                                                      unsigned short argumentCount,
                                                      PVOID callbackData);
};

}  // namespace Audio
}  // namespace HologramJS
