#pragma once

#include "audio-node.h"
#include "holojs/private/chakra.h"
#include "holojs/private/event-target.h"
#include "holojs/private/holojs-view.h"
#include "holojs/private/object-lifetime.h"
#include "holojs/private/webaudio-interfaces.h"

namespace HoloJs {

class HoloJsScriptHost;

namespace Win32 {

namespace WebAudio {

class AudioDestinationNode : public HoloJs::IAudioDestinationNode, public AudioNode {
   public:
    AudioDestinationNode(std::shared_ptr<lab::AudioContext> context,
                         std::shared_ptr<lab::AudioDestinationNode> audioDestinationNode)
        : m_context(context), m_audioDestinationNode(audioDestinationNode), AudioNode(context, audioDestinationNode)
    {
    }

    virtual ~AudioDestinationNode() { OutputDebugString(L"D"); }

    virtual void Release() {  }

    virtual JsValueRef connect(IAudioNode* destination) { return AudioNode::audioNodeConnect(destination); }

    virtual JsValueRef disconnect() { return AudioNode::audioNodeDisconnect(); }

   private:
    std::shared_ptr<lab::AudioContext> m_context;
    std::shared_ptr<lab::AudioDestinationNode> m_audioDestinationNode;
};
}  // namespace WebAudio
}  // namespace Win32
}  // namespace HoloJs