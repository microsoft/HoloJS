#pragma once

#include "ChakraForHoloJS.h"
#include "IRelease.h"

namespace HologramJS {
namespace Spatial {
class SpatialAnchorsProjections;
class SpatialAnchor : public HologramJS::Utilities::IRelease {
    friend class HologramJS::Spatial::SpatialAnchorsProjections;

   public:
    static SpatialAnchor* CreateRelativeTo(Windows::Perception::Spatial::SpatialCoordinateSystem ^ coordinateSystem,
                                           const Windows::Foundation::Numerics::float3& position,
                                           const Windows::Foundation::Numerics::quaternion& orientation);

    static SpatialAnchor* FromSpatialAnchor(Windows::Perception::Spatial::SpatialAnchor ^ anchor);

    ~SpatialAnchor() {}
    virtual void Release() {}

   private:
    SpatialAnchor() {}
    Windows::Perception::Spatial::SpatialAnchor ^ m_anchor;

    concurrency::task<void> ExportAsync(const std::wstring anchorName, JsValueRef anchorRef, JsValueRef callback);

    concurrency::task<void> SaveAsync(const std::wstring anchorName, JsValueRef anchorRef, JsValueRef callback);
};

}  // namespace Spatial
}  // namespace HologramJS
