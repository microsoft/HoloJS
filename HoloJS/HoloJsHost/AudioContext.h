#pragma once

#include "ChakraForHoloJS.h"
#include "IRelease.h"
#include "LabSound/extended/LabSound.h"

namespace HologramJS {
namespace Audio {

class AudioNode;

class AudioContext : public HologramJS::Utilities::IRelease {
   public:
    AudioContext();
    ~AudioContext();

    void Release() {}

    bool DecodeAudioData(JsValueRef data, JsValueRef onSuccess, JsValueRef onError);

    static bool InitializeProjections();

   private:
    static JsValueRef CHAKRA_CALLBACK createGainStatic(JsValueRef callee,
                                                       bool isConstructCall,
                                                       JsValueRef* arguments,
                                                       unsigned short argumentCount,
                                                       PVOID callbackData);

    static JsValueRef CHAKRA_CALLBACK createPanner(JsValueRef callee,
                                                   bool isConstructCall,
                                                   JsValueRef* arguments,
                                                   unsigned short argumentCount,
                                                   PVOID callbackData);

    static JsValueRef CHAKRA_CALLBACK getDestination(JsValueRef callee,
                                                     bool isConstructCall,
                                                     JsValueRef* arguments,
                                                     unsigned short argumentCount,
                                                     PVOID callbackData);

    static JsValueRef CHAKRA_CALLBACK createBufferSource(JsValueRef callee,
                                                         bool isConstructCall,
                                                         JsValueRef* arguments,
                                                         unsigned short argumentCount,
                                                         PVOID callbackData);

    static JsValueRef CHAKRA_CALLBACK listener_setPosition(JsValueRef callee,
                                                           bool isConstructCall,
                                                           JsValueRef* arguments,
                                                           unsigned short argumentCount,
                                                           PVOID callbackData);

    static JsValueRef CHAKRA_CALLBACK listener_setOrientation(JsValueRef callee,
                                                              bool isConstructCall,
                                                              JsValueRef* arguments,
                                                              unsigned short argumentCount,
                                                              PVOID callbackData);

    void callbackScriptOnDecodeSuccess(std::shared_ptr<lab::SoundBuffer> soundBuffer, JsValueRef callback);
    void callbackScriptOnDecodeError(JsValueRef callback);

    JsValueRef createGain();

    JsValueRef m_audioContentRef = JS_INVALID_REFERENCE;
    std::shared_ptr<lab::AudioContext> m_context;

    std::list<AudioNode*> m_audioNodes;
};
}  // namespace Audio
}  // namespace HologramJS
