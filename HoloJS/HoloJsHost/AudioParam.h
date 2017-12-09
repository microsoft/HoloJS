#pragma once

#include "IRelease.h"
#include "ChakraForHoloJS.h"
#include "LabSound/extended/LabSound.h"

namespace HologramJS {
namespace Audio {
class AudioParam : public HologramJS::Utilities::IRelease {
   public:
    AudioParam(std::shared_ptr<lab::AudioContext> context, std::shared_ptr<lab::AudioParam> audioParam)
        : m_audioParam(audioParam), m_context(context)
    {
    }
    virtual ~AudioParam() {}

    virtual void Release(){};

    static bool InitializeProjections();

   private:
    std::shared_ptr<lab::AudioParam> m_audioParam;
    std::shared_ptr<lab::AudioContext> m_context;

    static JsValueRef CHAKRA_CALLBACK setValueAtTime(JsValueRef callee,
                                                     bool isConstructCall,
                                                     JsValueRef* arguments,
                                                     unsigned short argumentCount,
                                                     PVOID callbackData);
};

}  // namespace Audio
}  // namespace HologramJS
