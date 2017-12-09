#pragma once

#include "AudioContext.h"
#include "LabSound/extended/LabSound.h"

namespace HologramJS {
namespace Audio {
class AudioScheduledSourceNode {
   public:
    AudioScheduledSourceNode(std::shared_ptr<lab::AudioScheduledSourceNode> node) : m_node(node) {}
    virtual ~AudioScheduledSourceNode() {}

    static bool InitializeProjections();

   private:
    std::shared_ptr<lab::AudioScheduledSourceNode> m_node;

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
