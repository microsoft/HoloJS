#include "stdafx.h"
#include "holojs/private/error-handling.h"
#include "holojs/private/holojs-script-host-internal.h"
#include "holojs/private/script-host-utilities.h"
#include "include/holojs/windows/IBufferOnMemory.h"
#include "surface-mapper.h"
#include <algorithm>
#include <list>
#include <map>
#include <ppltasks.h>
#include <string>
#include <vector>

using namespace concurrency;

using namespace HoloJs::Platforms::Win32;
using namespace Windows::Perception::Spatial::Surfaces;
using namespace Windows::Perception::Spatial;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::DirectX;
using namespace std;
using namespace std::placeholders;
using namespace DirectX;
using namespace Windows::Storage::Streams;

SurfaceMapper::~SurfaceMapper() { stop(); }

long SurfaceMapper::start(JsValueRef mapperRef)
{
    RETURN_IF_TRUE(m_isStarted);

    m_isStarted = true;
    m_mapperRef = mapperRef;
    RETURN_IF_JS_ERROR(JsAddRef(m_mapperRef, nullptr));

    auto asyncRequestAccess = SpatialSurfaceObserver::RequestAccessAsync();
    m_host->runInBackground([this, asyncRequestAccess]() -> long {
        create_task(asyncRequestAccess).wait();
        auto result = asyncRequestAccess->GetResults();
        if (result == SpatialPerceptionAccessStatus::Allowed) {
            createSurfaceObserver();
        } else {
            m_isStarted = false;
        }

        return S_OK;
    });

    return S_OK;
}

HRESULT SurfaceMapper::createSurfaceObserver()
{
    SpatialBoundingBox axisAlignedBoundingBox;
    axisAlignedBoundingBox.Center.x = m_centerX;
    axisAlignedBoundingBox.Center.y = m_centerY;
    axisAlignedBoundingBox.Center.z = m_centerZ;
    axisAlignedBoundingBox.Extents.x = static_cast<float>(m_extentX);
    axisAlignedBoundingBox.Extents.y = static_cast<float>(m_extentY);
    axisAlignedBoundingBox.Extents.z = static_cast<float>(m_extentZ);

    void* stationaryCoordinateSystemUnknown;
    RETURN_IF_FAILED(m_host->getStationaryCoordinateSystem(&stationaryCoordinateSystemUnknown));

    auto stationaryCoordinateSystem =
        safe_cast<SpatialCoordinateSystem ^>(reinterpret_cast<Platform::Object ^>(stationaryCoordinateSystemUnknown));

    SpatialBoundingVolume ^ bounds = SpatialBoundingVolume::FromBox(stationaryCoordinateSystem, axisAlignedBoundingBox);

    m_surfaceMeshOptions = ref new SpatialSurfaceMeshOptions();
    unsigned int formatIndex = 0;

    // Set the vertex format
    IVectorView<DirectXPixelFormat> ^ supportedVertexPositionFormats =
        m_surfaceMeshOptions->SupportedVertexPositionFormats;

    RETURN_IF_FALSE(supportedVertexPositionFormats->IndexOf(DirectXPixelFormat::R32G32B32A32Float, &formatIndex));
    m_surfaceMeshOptions->VertexPositionFormat = DirectXPixelFormat::R32G32B32A32Float;

    // Set the normals format
    IVectorView<DirectXPixelFormat> ^ supportedVertexNormalFormats = m_surfaceMeshOptions->SupportedVertexNormalFormats;
    RETURN_IF_FALSE(supportedVertexNormalFormats->IndexOf(DirectXPixelFormat::R8G8B8A8IntNormalized, &formatIndex));
    m_surfaceMeshOptions->VertexNormalFormat = DirectXPixelFormat::R8G8B8A8IntNormalized;

    m_surfaceMeshOptions->IncludeVertexNormals = true;

    // Create the observer.
    m_surfaceObserver = ref new SpatialSurfaceObserver();
    RETURN_IF_NULL(m_surfaceObserver);

    m_surfaceObserver->SetBoundingVolume(bounds);

    m_updateRegistrationToken = m_surfaceObserver->ObservedSurfacesChanged +=
        ref new Windows::Foundation::TypedEventHandler<Windows::Perception::Spatial::Surfaces::SpatialSurfaceObserver ^,
                                                       Platform::Object ^>(
            std::bind(&SurfaceMapper::onObservedSurfacesChanged, this, _1, _2));

    return S_OK;
}

HRESULT SurfaceMapper::processObservedSurfaces()
{
    auto observedSurfaces = m_surfaceObserver->GetObservedSurfaces();

    map<wstring, SpatialSurfaceMesh ^> meshes;

    auto surfaceIterator = observedSurfaces->First();
    auto endOfSurfaces = !surfaceIterator->HasCurrent;
    while (!endOfSurfaces) {
        auto surface = surfaceIterator->Current->Value;

        wstring surfaceId = surfaceIterator->Current->Key.ToString()->Data();

        auto existingMesh = m_knownSurfaceIds.find(surfaceId);
        if (existingMesh != m_knownSurfaceIds.end() && existingMesh->second == surface->UpdateTime.UniversalTime) {
            meshes.emplace(surfaceId, nullptr);
        } else {
            auto computeMeshAsync = surface->TryComputeLatestMeshAsync(m_triangleDensity, m_surfaceMeshOptions);
            create_task(computeMeshAsync).wait();
            auto mesh = computeMeshAsync->GetResults();
            if (mesh != nullptr) {
                meshes.emplace(surfaceId, mesh);
            }
        }

        endOfSurfaces = !surfaceIterator->MoveNext();
    }

    if (meshes.size() > 0) {
        m_host->runInScriptContext([this, meshes]() {
            map<wstring, long long> newKnownMeshes;
            for (const auto& meshEntry : meshes) {
                auto mesh = meshEntry.second;
                auto meshId = meshEntry.first;

                auto existingMesh = m_knownSurfaceIds.find(meshId);
                
                if (mesh == nullptr) {
                    EXIT_IF_TRUE(existingMesh == m_knownSurfaceIds.end());
                    newKnownMeshes.emplace(meshId, existingMesh->second);
                    // Skip unchanged meshes
                    continue;
                }

                JsValueRef positionRef;
                EXIT_IF_FAILED(createPositionRef(mesh, &positionRef));

                JsValueRef normalsRef;
                EXIT_IF_FAILED(createNormalsBuffer(mesh->VertexNormals, &normalsRef));

                JsValueRef indicesRef;
                EXIT_IF_FAILED(createIndicesBuffer(mesh->TriangleIndices, &indicesRef));

                JsValueRef verticesRef;
                EXIT_IF_FAILED(createVerticesBuffer(mesh->VertexPositions, &verticesRef));

                JsValueRef meshIdRef;
                EXIT_IF_FAILED(JsPointerToString(meshId.c_str(), meshId.length(), &meshIdRef));

                JsValueRef surfaceObject;
                EXIT_IF_JS_ERROR(JsCreateObject(&surfaceObject));
                ScriptHostUtilities::SetJsProperty(surfaceObject, L"id", meshIdRef);
                ScriptHostUtilities::SetJsProperty(surfaceObject, L"position", positionRef);
                ScriptHostUtilities::SetJsProperty(surfaceObject, L"indices", indicesRef);
                ScriptHostUtilities::SetJsProperty(surfaceObject, L"normals", normalsRef);
                ScriptHostUtilities::SetJsProperty(surfaceObject, L"vertices", verticesRef);

                if (existingMesh == m_knownSurfaceIds.end()) {
                    invokeEventListeners(L"newmesh", surfaceObject);
                } else {
                    invokeEventListeners(L"updatedmesh", surfaceObject);
                }

                // Keep track of new mesh ids
                newKnownMeshes.emplace(meshId, meshEntry.second->SurfaceInfo->UpdateTime.UniversalTime);
            }

            for (const auto& knownMeshId : m_knownSurfaceIds) {
                if (meshes.find(knownMeshId.first) == meshes.end()) {
                    JsValueRef meshIdRef;
                    EXIT_IF_FAILED(
                        JsPointerToString(knownMeshId.first.c_str(), knownMeshId.first.length(), &meshIdRef));
                    invokeEventListeners(L"deletedmesh", meshIdRef);
                }
            }

            // Store the new list of mesh ids
            m_knownSurfaceIds = std::move(newKnownMeshes);
        });
    }

    return S_OK;
}

HRESULT SurfaceMapper::createNormalsBuffer(SpatialSurfaceMeshBuffer ^ meshBuffer, JsValueRef* scriptBufferRef)
{
    unsigned char* destination;
    RETURN_IF_FAILED(ScriptHostUtilities::CreateTypedArray(
        JsArrayTypeUint8, scriptBufferRef, 3 * meshBuffer->ElementCount, &destination));

    Microsoft::WRL::ComPtr<::IBufferByteAccess> bufferByteAccess;
    reinterpret_cast<IInspectable*>(meshBuffer->Data)->QueryInterface(IID_PPV_ARGS(&bufferByteAccess));
    byte* source;
    bufferByteAccess->Buffer(&source);
    for (unsigned int i = 0; i < meshBuffer->ElementCount; i++) {
        memcpy(destination, source, 3 * sizeof(byte));
        destination += 3;
        source += 4;
    }

    return S_OK;
}

HRESULT SurfaceMapper::createIndicesBuffer(SpatialSurfaceMeshBuffer ^ meshBuffer, JsValueRef* scriptBufferRef)
{
    unsigned char* byteDestination;
    RETURN_IF_FAILED(ScriptHostUtilities::CreateTypedArray(
        JsArrayTypeUint16, scriptBufferRef, 3 * meshBuffer->ElementCount, &byteDestination));
    auto destination = reinterpret_cast<short*>(byteDestination);

    Microsoft::WRL::ComPtr<::IBufferByteAccess> bufferByteAccess;
    reinterpret_cast<IInspectable*>(meshBuffer->Data)->QueryInterface(IID_PPV_ARGS(&bufferByteAccess));
    byte* byteSource;
    bufferByteAccess->Buffer(&byteSource);
    auto source = reinterpret_cast<short*>(byteSource);

    for (unsigned int i = 0; i < meshBuffer->ElementCount; i += 3) {
        destination[i] = source[i + 2];
        destination[i + 1] = source[i + 1];
        destination[i + 2] = source[i];
    }

    return S_OK;
}

HRESULT SurfaceMapper::createVerticesBuffer(SpatialSurfaceMeshBuffer ^ meshBuffer, JsValueRef* scriptBufferRef)
{
    unsigned char* byteDestination;
    RETURN_IF_FAILED(ScriptHostUtilities::CreateTypedArray(
        JsArrayTypeFloat32, scriptBufferRef, 3 * meshBuffer->ElementCount, &byteDestination));
    auto destination = reinterpret_cast<float*>(byteDestination);

    Microsoft::WRL::ComPtr<::IBufferByteAccess> bufferByteAccess;
    reinterpret_cast<IInspectable*>(meshBuffer->Data)->QueryInterface(IID_PPV_ARGS(&bufferByteAccess));
    byte* byteSource;
    bufferByteAccess->Buffer(&byteSource);
    auto source = reinterpret_cast<float*>(byteSource);

    for (unsigned int i = 0; i < meshBuffer->ElementCount; i++) {
        memcpy(destination, source, 3 * sizeof(float));
        destination += 3;
        source += 4;
    }

    return S_OK;
}

HRESULT SurfaceMapper::createPositionRef(SpatialSurfaceMesh ^ mesh, JsValueRef* positionRef)
{
    void* stationaryCoordinateSystemUnknown;
    RETURN_IF_FAILED(m_host->getStationaryCoordinateSystem(&stationaryCoordinateSystemUnknown));

    auto stationaryCoordinateSystem =
        safe_cast<SpatialCoordinateSystem ^>(reinterpret_cast<Platform::Object ^>(stationaryCoordinateSystemUnknown));

    // Combine origin transform with mesh scale to create position transform
    auto originToSurface = mesh->CoordinateSystem->TryGetTransformTo(stationaryCoordinateSystem);
    RETURN_IF_NULL(originToSurface);

    DirectX::XMMATRIX translation = XMLoadFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&originToSurface->Value));
    XMMATRIX scale =
        XMMatrixScaling(mesh->VertexPositionScale.x, mesh->VertexPositionScale.y, mesh->VertexPositionScale.z);
    XMMATRIX posTransformMat = scale * translation;

    XMFLOAT4X4 transform;
    XMStoreFloat4x4(&transform, posTransformMat);

    RETURN_IF_FAILED(ScriptHostUtilities::CreateTypedArrayFromBuffer(JsArrayTypeFloat32,
        positionRef, 16, reinterpret_cast<unsigned char*>(&transform.m)));

    return S_OK;
}

long SurfaceMapper::stop()
{
    std::lock_guard<std::mutex> guard(m_backgroundUpdateLock);

    JsRelease(m_mapperRef, nullptr);

    if (m_surfaceObserver != nullptr) {
        m_surfaceObserver->ObservedSurfacesChanged -= m_updateRegistrationToken;
        m_surfaceObserver = nullptr;
    }

    return S_OK;
}

void SurfaceMapper::onObservedSurfacesChanged(SpatialSurfaceObserver ^ sender, Platform::Object ^ args)
{
    std::lock_guard<std::mutex> guard(m_backgroundUpdateLock);

    processObservedSurfaces();
}
