#pragma once

#include "holojs/windows/mixed-reality/mixed-reality-context.h"

namespace HoloJs {
namespace UWP {
class UWPMixedRealityContext : public HoloJs::MixedReality::MixedRealityContext {
   public:
    UWPMixedRealityContext() {}
    ~UWPMixedRealityContext() {}

    virtual HRESULT createHolographicSpace();
};
}  // namespace Win32
}  // namespace HoloJs