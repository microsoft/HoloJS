#pragma once

#include "IRelease.h"

namespace HologramJS {
namespace Spatial {
class SpatialAnchor : public HologramJS::Utilities::IRelease {
   public:
    static SpatialAnchor* CreateRelativeTo(Windows::Perception::Spatial::SpatialCoordinateSystem ^ coordinateSystem,
                                           const Windows::Foundation::Numerics::float3& position,
                                           const Windows::Foundation::Numerics::quaternion& orientation);

    static SpatialAnchor* FromSpatialAnchor(Windows::Perception::Spatial::SpatialAnchor^ anchor);

    ~SpatialAnchor() {}
    virtual void Release() {}

   private:
    SpatialAnchor() {}
    Windows::Perception::Spatial::SpatialAnchor ^ m_anchor;
};

}  // namespace Spatial
}  // namespace HologramJS
