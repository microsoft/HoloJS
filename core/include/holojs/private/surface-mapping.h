#pragma once

#include "chakra.h"
#include "event-target.h"
#include "holojs-view.h"
#include "object-lifetime.h"
#include "blob-interface.h"

namespace HoloJs {

class ISurfaceMapper : public HoloJs::ResourceManagement::IRelease {
   public:
    virtual ~ISurfaceMapper() {}

    virtual long start(JsValueRef mapperRef) = 0;
	virtual long stop() = 0;

    virtual void setBoundingCube(
            float centerX, float centerY, float centerZ, float extentX, float extentY, float extentZ) = 0;

    virtual void getBoundingCube(
        float* centerX, float* centerY, float* centerZ, float* extentX, float* extentY, float* extentZ) = 0;

    virtual void getTriangleDensity(float* triangleDensity) = 0;
    virtual void setTriangleDensity(float triangleDensity) = 0;
};

}  // namespace HoloJs