#pragma once

#include "holojs/windows/mixed-reality/spatial-input.h"

namespace HoloJs {
namespace Win32 {
class Win32SpatialInput : public HoloJs::MixedReality::Input::SpatialInput {
   public:
    Win32SpatialInput() {}
    ~Win32SpatialInput() {}

    protected:

    virtual HRESULT createSpatialInteractionManager();
};
}  // namespace Win32
}  // namespace HoloJs