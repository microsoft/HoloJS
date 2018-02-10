#include "pch.h"
#include "SpatialAnchor.h"

using namespace HologramJS::Spatial;
using namespace std;

SpatialAnchor* SpatialAnchor::CreateRelativeTo(Windows::Perception::Spatial::SpatialCoordinateSystem ^ coordinateSystem,
                                               const Windows::Foundation::Numerics::float3& position,
                                               const Windows::Foundation::Numerics::quaternion& orientation)
{
    std::unique_ptr<SpatialAnchor> newAnchor;
    newAnchor.reset(new SpatialAnchor());
    newAnchor->m_anchor = Windows::Perception::Spatial::SpatialAnchor::TryCreateRelativeTo(coordinateSystem, position, orientation);

    return newAnchor->m_anchor == nullptr ? nullptr : newAnchor.release();   
}

SpatialAnchor* SpatialAnchor::FromSpatialAnchor(Windows::Perception::Spatial::SpatialAnchor^ anchor)
{
    auto newAnchor = new SpatialAnchor();
    newAnchor->m_anchor = anchor;

    return newAnchor;
}
