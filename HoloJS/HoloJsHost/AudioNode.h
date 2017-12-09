#pragma once

#include "AudioContext.h"
#include "LabSound/extended/LabSound.h"

namespace HologramJS {
namespace Audio {
class AudioNode : public HologramJS::Utilities::IRelease {
   public:
    AudioNode(std::shared_ptr<lab::AudioContext> context, std::shared_ptr<lab::AudioNode> node)
        : m_node(node), m_context(context)
    {
    }
    virtual ~AudioNode();

    virtual void Release(){};

    static bool InitializeProjections();

   protected:
    std::shared_ptr<lab::AudioContext> m_context;

   private:
    std::shared_ptr<lab::AudioNode> m_node;

    static JsValueRef CHAKRA_CALLBACK connect(JsValueRef callee,
                                              bool isConstructCall,
                                              JsValueRef* arguments,
                                              unsigned short argumentCount,
                                              PVOID callbackData);

    static JsValueRef CHAKRA_CALLBACK disconnect(JsValueRef callee,
                                                 bool isConstructCall,
                                                 JsValueRef* arguments,
                                                 unsigned short argumentCount,
                                                 PVOID callbackData);
};

}  // namespace Audio
}  // namespace HologramJS
