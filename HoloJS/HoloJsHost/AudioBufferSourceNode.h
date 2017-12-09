#pragma once

#include "AudioNode.h"
#include "AudioScheduledSourceNode.h"
#include "LabSound/extended/LabSound.h"

namespace HologramJS {
namespace Audio {
class AudioBufferSourceNode : public AudioNode {
   public:
    AudioBufferSourceNode(std::shared_ptr<lab::AudioContext> context,
                          std::shared_ptr<lab::AudioBufferSourceNode> audioBufferSourceNode)
        : m_audioBufferSourceNode(audioBufferSourceNode),
          AudioNode(context, audioBufferSourceNode)
    {
    }
    virtual ~AudioBufferSourceNode() {}

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
};

}  // namespace Audio
}  // namespace HologramJS
