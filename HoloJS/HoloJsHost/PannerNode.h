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

            virtual void Release() {};

        private:
            std::shared_ptr<lab::PannerNode> m_pannerNode;
        };

    }  // namespace Audio
}  // namespace HologramJS


