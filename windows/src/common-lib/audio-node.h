#pragma once

#include "LabSound/extended/LabSound.h"
#include "holojs/private/chakra.h"
#include "holojs/private/error-handling.h"
#include "holojs/private/object-lifetime.h"
#include "holojs/private/webaudio-interfaces.h"

namespace HoloJs {

class HoloJsScriptHost;

namespace Win32 {

namespace WebAudio {

class AudioNode {
   public:
    AudioNode(std::shared_ptr<lab::AudioContext> context, std::shared_ptr<lab::AudioNode> node)
        : m_node(node), m_context(context)
    {
    }

    virtual ~AudioNode() { lab::ContextRenderLock r(m_context.get(), "delete node"); }

    JsValueRef audioNodeConnect(IAudioNode* destination)
    {
        auto other = dynamic_cast<AudioNode*>(destination);
        RETURN_INVALID_REF_IF_NULL(other);
        m_context->connect(other->m_node, m_node, 0, 0);

        return JS_INVALID_REFERENCE;
    }

    JsValueRef audioNodeDisconnect() { return JS_INVALID_REFERENCE; }

   private:
    std::shared_ptr<lab::AudioContext> m_context;
    std::shared_ptr<lab::AudioNode> m_node;
};
}  // namespace WebAudio
}  // namespace Win32
}  // namespace HoloJs