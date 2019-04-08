#pragma once

#include "LabSound/extended/LabSound.h"
#include "audio-node.h"
#include "holojs/private/chakra.h"
#include "holojs/private/object-lifetime.h"
#include "holojs/private/webaudio-interfaces.h"

namespace HoloJs {

class HoloJsScriptHost;

namespace Win32 {

namespace WebAudio {

class GainNode : public AudioNode, public IGainNode {
   public:
    GainNode(std::shared_ptr<lab::AudioContext> context, std::shared_ptr<lab::GainNode> gainNode)
        : m_context(context), m_gainNode(gainNode), AudioNode(context, gainNode)
    {
    }

    virtual ~GainNode() {  }

    virtual void Release() {}

    virtual JsValueRef connect(IAudioNode* destination)
    {
        return AudioNode::audioNodeConnect(destination);
    }
    virtual JsValueRef disconnect() { return AudioNode::audioNodeDisconnect(); }

   private:
    std::shared_ptr<lab::AudioContext> m_context;
    std::shared_ptr<lab::GainNode> m_gainNode;
};
}  // namespace WebAudio
}  // namespace Win32
}  // namespace HoloJs