#pragma once

#include "ChakraForHoloJS.h"
#include "IRelease.h"

namespace HologramJS {
namespace Audio {
class AudioContext : public HologramJS::Utilities::IRelease {
   public:
    AudioContext();
    ~AudioContext();

    void Release() {}

    bool DecodeAudioData(JsValueRef data, JsValueRef onSuccess, JsValueRef onError);

   private:
    JsValueRef m_audioContentRef = JS_INVALID_REFERENCE;
};
}  // namespace Audio
}  // namespace HologramJS
