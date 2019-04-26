#pragma once

#include "chakra.h"
#include "event-target.h"
#include "holojs-view.h"
#include "object-lifetime.h"
#include <array>
#include <string>

namespace HoloJs {

class ISpatialAnchor : public HoloJs::ResourceManagement::IRelease {
   public:
    virtual ~ISpatialAnchor() {}
    virtual bool isPersisted() = 0;
    virtual long getName(std::wstring& name) = 0;
    virtual long tryGetPositionRelativeTo(HoloJs::IHoloJsScriptHostInternal* host,
                                          HoloJs::ISpatialAnchor* other,
                                          std::array<double, 3>& position,
                                          std::array<double, 4>& orientation) = 0;
};

class ISpatialAnchorsStore : public HoloJs::ResourceManagement::IRelease {
   public:
    virtual std::vector<HoloJs::ISpatialAnchor*> enumerate() = 0;
    virtual long open(const std::wstring& name, HoloJs::ISpatialAnchor** anchor) = 0;
    virtual long deleteAnchor(const std::wstring& name) = 0;
    virtual bool areAnchorsImplemented() = 0;
    virtual long save(HoloJs::ISpatialAnchor* anchor, const std::wstring& anchorName) = 0;
    virtual void initialize(JsValueRef storeRef) = 0;
};

}  // namespace HoloJs