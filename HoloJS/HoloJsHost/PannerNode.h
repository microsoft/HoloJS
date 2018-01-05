#pragma once

#include "AudioNode.h"
#include "LabSound/extended/LabSound.h"

namespace HologramJS {
namespace Audio {
class PannerNode : public AudioNode {
   public:
    PannerNode(std::shared_ptr<lab::AudioContext> context, std::shared_ptr<lab::PannerNode> pannerNode)
        : m_pannerNode(pannerNode), AudioNode(context, pannerNode)
    {
    }
    virtual ~PannerNode() {}

    virtual void Release(){};

    static bool InitializeProjections();

   private:
    std::shared_ptr<lab::PannerNode> m_pannerNode;

    static JsValueRef CHAKRA_CALLBACK setPosition(JsValueRef callee,
                                                  bool isConstructCall,
                                                  JsValueRef* arguments,
                                                  unsigned short argumentCount,
                                                  PVOID callbackData);

    static JsValueRef CHAKRA_CALLBACK setOrientation(JsValueRef callee,
                                                     bool isConstructCall,
                                                     JsValueRef* arguments,
                                                     unsigned short argumentCount,
                                                     PVOID callbackData);

    static JsValueRef CHAKRA_CALLBACK setVelocity(JsValueRef callee,
                                                  bool isConstructCall,
                                                  JsValueRef* arguments,
                                                  unsigned short argumentCount,
                                                  PVOID callbackData);
};

}  // namespace Audio
}  // namespace HologramJS
