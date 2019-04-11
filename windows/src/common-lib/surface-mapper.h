#pragma once

#include "holojs/private/chakra.h"
#include "holojs/private/surface-mapping.h"
#include <memory>
#include <mutex>

namespace HoloJs {
class IHoloJsScriptHostInternal;
}

namespace HoloJs {
namespace Platforms {
namespace Win32 {
class SurfaceMapper : public HoloJs::ISurfaceMapper, public HoloJs::EventTarget {
   public:
    SurfaceMapper(HoloJs::IHoloJsScriptHostInternal* host) : m_host(host) {}
    ~SurfaceMapper();

    virtual void Release() {}

    virtual long start(JsValueRef mapperRef);
    virtual long stop();

    virtual void setBoundingCube(
        float centerX, float centerY, float centerZ, float extentX, float extentY, float extentZ)
    {
        m_centerX = centerX;
        m_centerY = centerY;
        m_centerZ = centerZ;

        m_extentX = extentX;
        m_extentY = extentY;
        m_extentZ = extentZ;
    }

    virtual void getBoundingCube(
        float* centerX, float* centerY, float* centerZ, float* extentX, float* extentY, float* extentZ)
    {
        *centerX = m_centerX;
        *centerY = m_centerY;
        *centerZ = m_centerZ;

        *extentX = m_extentX;
        *extentY = m_extentY;
        *extentZ = m_extentZ;
    }

    virtual void getTriangleDensity(float* triangleDensity) { *triangleDensity = m_triangleDensity; }
    virtual void setTriangleDensity(float triangleDensity) { m_triangleDensity = triangleDensity; }

   private:
    Windows::Perception::Spatial::Surfaces::SpatialSurfaceObserver ^ m_surfaceObserver;
    Windows::Perception::Spatial::Surfaces::SpatialSurfaceMeshOptions ^ m_surfaceMeshOptions;
    Windows::Foundation::EventRegistrationToken m_updateRegistrationToken;

    HRESULT createSurfaceObserver();
    HRESULT processObservedSurfaces();

    float m_extentX = 5, m_extentY = 5, m_extentZ = 5;
    float m_centerX = 0, m_centerY = 0, m_centerZ = 0;
    float m_triangleDensity = 1200;

    HRESULT createPositionRef(Windows::Perception::Spatial::Surfaces::SpatialSurfaceMesh ^ mesh,
                              JsValueRef* positionRef);

    HRESULT createNormalsBuffer(Windows::Perception::Spatial::Surfaces::SpatialSurfaceMeshBuffer ^ meshBuffer,
                                JsValueRef* scriptBufferRef);

    HRESULT createIndicesBuffer(Windows::Perception::Spatial::Surfaces::SpatialSurfaceMeshBuffer ^ meshBuffer,
                                JsValueRef* scriptBufferRef);

    HRESULT createVerticesBuffer(Windows::Perception::Spatial::Surfaces::SpatialSurfaceMeshBuffer ^ meshBuffer,
                                 JsValueRef* scriptBufferRef);

    bool m_isStarted;
    HoloJs::IHoloJsScriptHostInternal* m_host;
    JsValueRef m_mapperRef;

    std::mutex m_backgroundUpdateLock;
    void onObservedSurfacesChanged(Windows::Perception::Spatial::Surfaces::SpatialSurfaceObserver ^ sender,
                                   Platform::Object ^ args);

    std::map<std::wstring, long long> m_knownSurfaceIds;
};

}  // namespace Win32
}  // namespace Platforms
}  // namespace HoloJs
