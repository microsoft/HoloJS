#pragma once

#include "holojs/private/chakra.h"
#include "holojs/private/spatial-anchors.h"
#include <memory>
#include <string>
#include <vector>

namespace HoloJs {
class IHoloJsScriptHostInternal;
}

namespace HoloJs {
namespace Platforms {
namespace Win32 {

class HoloJsSpatialAnchor : public HoloJs::ISpatialAnchor {
   public:
    HoloJsSpatialAnchor(Windows::Perception::Spatial::SpatialAnchor ^ anchor) : m_anchor(anchor) {}
    HoloJsSpatialAnchor(Windows::Perception::Spatial::SpatialAnchor ^ anchor, const wchar_t* name)
        : m_anchor(anchor), m_name(name)
    {
    }
    virtual ~HoloJsSpatialAnchor() {}

    virtual void Release() {}

    Windows::Perception::Spatial::SpatialAnchor ^ getPerceptionAnchor() { return m_anchor; }

        static HoloJs::ISpatialAnchor* create(HoloJs::IHoloJsScriptHostInternal* host,
                                              HoloJs::ISpatialAnchor* relativeTo,
                                              const std::array<double, 3>& position,
                                              const std::array<double, 4>& orientation);

    virtual bool isPersisted() { return !m_name.empty(); }
    virtual long getName(std::wstring& name)
    {
        RETURN_IF_FALSE(isPersisted());
        name = m_name;
        return S_OK;
    }

    virtual long tryGetPositionRelativeTo(HoloJs::IHoloJsScriptHostInternal* host,
                                          HoloJs::ISpatialAnchor* other,
                                          std::array<double, 3>& position,
                                          std::array<double, 4>& orientation);

   private:
    Windows::Perception::Spatial::SpatialAnchor ^ m_anchor;
    std::wstring m_name;
};

class SpatialAnchors : public HoloJs::ISpatialAnchorsStore, public HoloJs::EventTarget {
   public:
    SpatialAnchors(HoloJs::IHoloJsScriptHostInternal* host) : m_host(host) {}
    ~SpatialAnchors() {}

    virtual void Release() {}

    virtual void initialize(JsValueRef storeRef);

    virtual std::vector<HoloJs::ISpatialAnchor*> enumerate();
    virtual long open(const std::wstring& name, HoloJs::ISpatialAnchor** anchor);
    virtual long deleteAnchor(const std::wstring& name);
    virtual bool areAnchorsImplemented();
    virtual long save(HoloJs::ISpatialAnchor* anchor, const std::wstring& anchorName);

   private:
    HoloJs::IHoloJsScriptHostInternal* m_host;
    Windows::Perception::Spatial::SpatialAnchorStore ^ m_store;
};

}  // namespace Win32
}  // namespace Platforms
}  // namespace HoloJs
