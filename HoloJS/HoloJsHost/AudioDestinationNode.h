#pragma once

#include "AudioNode.h"
#include "LabSound/extended/LabSound.h"

namespace HologramJS {
namespace Audio {
class AudioDestinationNode : public AudioNode {
   public:
    AudioDestinationNode(std::shared_ptr<lab::AudioContext> context,
                         std::shared_ptr<lab::AudioDestinationNode> destination)
        : m_destination(destination), AudioNode(context, destination)
    {
    }
    virtual ~AudioDestinationNode() {}

    virtual void Release() {}

   private:
    std::shared_ptr<lab::AudioDestinationNode> m_destination;
};

}  // namespace Audio
}  // namespace HologramJS
