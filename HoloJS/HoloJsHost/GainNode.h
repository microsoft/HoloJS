#pragma once

#include "AudioNode.h"
#include "LabSound/extended/LabSound.h"

namespace HologramJS {
namespace Audio {
class GainNode : public AudioNode {
   public:
    GainNode(std::shared_ptr<lab::AudioContext> context, std::shared_ptr<lab::GainNode> gainNode)
        : m_gainNode(gainNode), AudioNode(context, gainNode)
    {
    }
    virtual ~GainNode() {}

    virtual void Release(){};

   private:
    std::shared_ptr<lab::GainNode> m_gainNode;
};

}  // namespace Audio
}  // namespace HologramJS
