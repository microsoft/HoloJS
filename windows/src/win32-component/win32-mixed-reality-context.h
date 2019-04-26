#pragma once

#include "holojs/windows/mixed-reality/mixed-reality-context.h"

namespace HoloJs {
namespace Win32 {
class Win32MixedRealityContext : public HoloJs::MixedReality::MixedRealityContext {
   public:
    Win32MixedRealityContext() {}
    ~Win32MixedRealityContext() {}

    virtual HRESULT createHolographicSpace();
};
}  // namespace Win32
}  // namespace HoloJs