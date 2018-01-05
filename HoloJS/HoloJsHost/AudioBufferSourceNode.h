#pragma once

#include "AudioNode.h"
#include "ChakraForHoloJS.h"
#include "AudioScheduledSourceNode.h"
#include "LabSound/extended/LabSound.h"

namespace HologramJS {
namespace Audio {
class AudioBufferSourceNode : public AudioNode {
   public:
    AudioBufferSourceNode(std::shared_ptr<lab::AudioContext> context,
                          std::shared_ptr<lab::AudioBufferSourceNode> audioBufferSourceNode);

    virtual ~AudioBufferSourceNode() {
        if (m_scriptOnEndedFunction != JS_INVALID_REFERENCE) {
            JsRelease(m_scriptOnEndedFunction, nullptr);
        }
    }

    static bool InitializeProjections();

   private:
    std::shared_ptr<lab::AudioBufferSourceNode> m_audioBufferSourceNode;

    static JsValueRef CHAKRA_CALLBACK setBuffer(JsValueRef callee,
                                                bool isConstructCall,
                                                JsValueRef* arguments,
                                                unsigned short argumentCount,
                                                PVOID callbackData);

    static JsValueRef CHAKRA_CALLBACK getPlaybackRate(JsValueRef callee,
                                                      bool isConstructCall,
                                                      JsValueRef* arguments,
                                                      unsigned short argumentCount,
                                                      PVOID callbackData);

    static JsValueRef CHAKRA_CALLBACK start(JsValueRef callee,
                                            bool isConstructCall,
                                            JsValueRef* arguments,
                                            unsigned short argumentCount,
                                            PVOID callbackData);

    static JsValueRef CHAKRA_CALLBACK stop(JsValueRef callee,
                                           bool isConstructCall,
                                           JsValueRef* arguments,
                                           unsigned short argumentCount,
                                           PVOID callbackData);

    static JsValueRef CHAKRA_CALLBACK register_onended(JsValueRef callee,
                                                       bool isConstructCall,
                                                       JsValueRef* arguments,
                                                       unsigned short argumentCount,
                                                       PVOID callbackData);

    void onFinished(AudioBufferSourceNode*) {}

    void callbackScriptOnFinished();
    JsValueRef m_scriptOnEndedFunction = JS_INVALID_REFERENCE;
};

}  // namespace Audio
}  // namespace HologramJS
