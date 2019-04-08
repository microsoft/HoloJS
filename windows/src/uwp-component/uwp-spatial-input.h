#pragma once

#include "holojs/windows/mixed-reality/spatial-input.h"

namespace HoloJs {
namespace UWP {
class UWPSpatialInput : public HoloJs::MixedReality::Input::SpatialInput {
   public:
    UWPSpatialInput() {}
    ~UWPSpatialInput() {}

    protected:

    virtual HRESULT createSpatialInteractionManager();
};
}  // namespace Win32
}  // namespace HoloJs