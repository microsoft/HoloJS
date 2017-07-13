#include "pch.h"
#include "SpatialMapping.h"
#include "InputInterface.h"
#include "ScriptErrorHandling.h"
#include "ScriptHostUtilities.h"
#include <DirectXMath.h>
#include <Robuffer.h>
#include <WindowsNumerics.h>

using namespace HologramJS::Input;
using namespace Windows::Perception::Spatial;
using namespace concurrency;
using namespace Windows::Perception::Spatial::Surfaces;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::DirectX;
using namespace HologramJS::Utilities;
using namespace DirectX;

SpatialMapping::SpatialMapping() {}

SpatialMapping::~SpatialMapping() {}

JsValueRef SpatialMapping::GetSpatialData(float extentX, float extentY, float extentZ, int trianglesPerCubicMeter)
{
    RETURN_INVALID_REF_IF_FALSE(m_surfaceAccessAllowed);
    RETURN_INVALID_REF_IF_FALSE(CreateSurfaceObserver(extentX, extentY, extentZ));

    m_trianglesResolution = trianglesPerCubicMeter;

    auto mapContainingSurfaceCollection = m_surfaceObserver->GetObservedSurfaces();

    int counter = 0;
    for (auto const& pair : mapContainingSurfaceCollection) {
        // Store the ID and metadata for each surface.
        auto const& id = pair->Key;
        auto const& surfaceInfo = pair->Value;
        ProcessSurface(pair->Value);
    }

    return JS_INVALID_REFERENCE;
}

void SpatialMapping::EnableSpatialMapping(SpatialStationaryFrameOfReference ^ frameOfReference)
{
    m_frameOfReference = frameOfReference;

    auto initSurfaceObserverTask = create_task(SpatialSurfaceObserver::RequestAccessAsync());
    initSurfaceObserverTask.then([this](Windows::Perception::Spatial::SpatialPerceptionAccessStatus status) {
        switch (status) {
            case SpatialPerceptionAccessStatus::Allowed:
                m_surfaceAccessAllowed = true;
                break;
            default:
                // Access was denied. This usually happens because your AppX manifest file does not declare the
                // spatialPerception capability.
                // For info on what else can cause this, see:
                // http://msdn.microsoft.com/library/windows/apps/mt621422.aspx
                m_surfaceAccessAllowed = false;
                break;
        }
    });
}

bool SpatialMapping::CreateSurfaceObserver(float extentX, float extentY, float extentZ)
{
    RETURN_IF_FALSE(m_surfaceAccessAllowed);

    if (m_surfaceObserver != nullptr) {
        return true;
    }

    SpatialBoundingBox axisAlignedBoundingBox = {
        {0.f, 0.f, 0.f}, {extentX, extentY, extentZ},
    };
    SpatialBoundingVolume ^ bounds =
        SpatialBoundingVolume::FromBox(m_frameOfReference->CoordinateSystem, axisAlignedBoundingBox);

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

    for (const auto format : m_surfaceMeshOptions->SupportedTriangleIndexFormats) {
        OutputDebugStringW(format.ToString()->Data());
    }

    m_surfaceMeshOptions->IncludeVertexNormals = true;

    // Create the observer.
    m_surfaceObserver = ref new SpatialSurfaceObserver();
    RETURN_IF_NULL(m_surfaceObserver);

    m_surfaceObserver->SetBoundingVolume(bounds);

    return true;
}

byte* GetPointerToData(Windows::Storage::Streams::IBuffer ^ buffer)
{
    Microsoft::WRL::ComPtr<Windows::Storage::Streams::IBufferByteAccess> bufferByteAccess;
    reinterpret_cast<IInspectable*>(buffer)->QueryInterface(IID_PPV_ARGS(&bufferByteAccess));

    byte* pointer;
    bufferByteAccess->Buffer(&pointer);

    return pointer;
}

void SpatialMapping::ProcessOneSpatialMappingDataUpdate()
{
    if (m_scriptCallback == JS_INVALID_REFERENCE) {
        return;
    }

    std::lock_guard<std::mutex> guard(m_processingLock);
    if (m_meshIds.size() == 0) {
        return;
    }

    JsTypedArrayType arrayType;
    int elementSize;
    unsigned int typedArraySize;
    byte* typedArrayStorage;

    JsValueRef originToSurfaceRef;
    EXIT_IF_JS_ERROR(JsCreateTypedArray(
        JsArrayTypeFloat32, JS_INVALID_REFERENCE, 0, m_originToSurfaces.front().size(), &originToSurfaceRef));
    EXIT_IF_JS_ERROR(
        JsGetTypedArrayStorage(originToSurfaceRef, &typedArrayStorage, &typedArraySize, &arrayType, &elementSize));
    memcpy(typedArrayStorage, m_originToSurfaces.front().data(), typedArraySize);

    JsValueRef normalBufferRef;
    EXIT_IF_JS_ERROR(
        JsCreateTypedArray(JsArrayTypeInt8, JS_INVALID_REFERENCE, 0, m_normalBuffers.front().size(), &normalBufferRef));
    EXIT_IF_JS_ERROR(
        JsGetTypedArrayStorage(normalBufferRef, &typedArrayStorage, &typedArraySize, &arrayType, &elementSize));
    memcpy(typedArrayStorage, m_normalBuffers.front().data(), typedArraySize);

    JsValueRef indexBufferRef;
    EXIT_IF_JS_ERROR(
        JsCreateTypedArray(JsArrayTypeInt16, JS_INVALID_REFERENCE, 0, m_indexBuffers.front().size(), &indexBufferRef));
    EXIT_IF_JS_ERROR(
        JsGetTypedArrayStorage(indexBufferRef, &typedArrayStorage, &typedArraySize, &arrayType, &elementSize));
    memcpy(typedArrayStorage, m_indexBuffers.front().data(), typedArraySize);

    JsValueRef vertexBufferRef;
    EXIT_IF_JS_ERROR(JsCreateTypedArray(
        JsArrayTypeFloat32, JS_INVALID_REFERENCE, 0, m_vertexBuffers.front().size(), &vertexBufferRef));
    EXIT_IF_JS_ERROR(
        JsGetTypedArrayStorage(vertexBufferRef, &typedArrayStorage, &typedArraySize, &arrayType, &elementSize));
    memcpy(typedArrayStorage, m_vertexBuffers.front().data(), typedArraySize);

    JsValueRef meshIdRef;
    EXIT_IF_JS_ERROR(
        JsCreateTypedArray(JsArrayTypeInt8, JS_INVALID_REFERENCE, 0, m_meshIds.front().size(), &meshIdRef));
    EXIT_IF_JS_ERROR(JsGetTypedArrayStorage(meshIdRef, &typedArrayStorage, &typedArraySize, &arrayType, &elementSize));
    memcpy(typedArrayStorage, m_meshIds.front().data(), typedArraySize);

    JsValueRef meshObjectRef;
    EXIT_IF_JS_ERROR(JsCreateObject(&meshObjectRef));
    EXIT_IF_FALSE(ScriptHostUtilities::SetJsProperty(meshObjectRef, L"originToSurfaceTransform", originToSurfaceRef));
    EXIT_IF_FALSE(ScriptHostUtilities::SetJsProperty(meshObjectRef, L"normals", normalBufferRef));
    EXIT_IF_FALSE(ScriptHostUtilities::SetJsProperty(meshObjectRef, L"indices", indexBufferRef));
    EXIT_IF_FALSE(ScriptHostUtilities::SetJsProperty(meshObjectRef, L"vertices", vertexBufferRef));
    EXIT_IF_FALSE(ScriptHostUtilities::SetJsProperty(meshObjectRef, L"id", meshIdRef));

    // Create JS callback parameters
    JsValueRef parameters[3];
    parameters[0] = m_scriptCallback;
    EXIT_IF_JS_ERROR(JsIntToNumber(static_cast<int>(NativeToScriptInputType::SpatialMapping), &parameters[1]));
    parameters[2] = meshObjectRef;

    // Invoke JS callback
    JsValueRef result;
    HANDLE_EXCEPTION_IF_JS_ERROR(JsCallFunction(m_scriptCallback, parameters, ARRAYSIZE(parameters), &result));

    m_meshIds.pop_front();
    m_vertexBuffers.pop_front();
    m_indexBuffers.pop_front();
    m_normalBuffers.pop_front();
    m_originToSurfaces.pop_front();
}

void SpatialMapping::ProcessSurface(SpatialSurfaceInfo ^ surface)
{
    auto computeMeshTask = create_task(surface->TryComputeLatestMeshAsync(m_trianglesResolution, m_surfaceMeshOptions));

    computeMeshTask.then(
        [this](SpatialSurfaceMesh ^ mesh) {
            if (mesh == nullptr) {
                return;
            }

            m_processingLock.lock();

            // Combine origin transform with mesh scale to create position transform
            auto originToSurface = mesh->CoordinateSystem->TryGetTransformTo(m_frameOfReference->CoordinateSystem);
            DirectX::XMMATRIX translationMat = XMLoadFloat4x4(&originToSurface->Value);
            XMMATRIX scaleMat =
                XMMatrixScaling(mesh->VertexPositionScale.x, mesh->VertexPositionScale.y, mesh->VertexPositionScale.z);
            XMMATRIX posTransformMat = scaleMat * translationMat;
            XMFLOAT4X4 posTransformf4;
            XMStoreFloat4x4(&posTransformf4, posTransformMat);

            // Copy position transform to buffer
            m_originToSurfaces.emplace_back(sizeof(posTransformf4));
            memcpy(m_originToSurfaces.back().data(), posTransformf4.m, sizeof(posTransformf4));

            // Copy normals to buffer; go from float4 to float3
            m_normalBuffers.emplace_back(3 * mesh->VertexNormals->ElementCount);
            auto destNormalPointer = m_normalBuffers.back().data();
            auto sourceNormalPointer = reinterpret_cast<byte*>(GetPointerToData(mesh->VertexNormals->Data));
            for (unsigned int i = 0; i < mesh->VertexNormals->ElementCount; i++) {
                memcpy(destNormalPointer, sourceNormalPointer, 3 * sizeof(byte));
                destNormalPointer += 3;
                sourceNormalPointer += 4;
            }

            // Copy indices to buffer; swap order to CW
            m_indexBuffers.emplace_back(mesh->TriangleIndices->ElementCount);
            auto destIndexPointer = m_indexBuffers.back().data();
            auto sourceIndexPointer = reinterpret_cast<short int*>(GetPointerToData(mesh->TriangleIndices->Data));
            for (unsigned int i = 0; i <= mesh->TriangleIndices->ElementCount - 3; i += 3) {
                destIndexPointer[i] = sourceIndexPointer[i + 2];
                destIndexPointer[i + 1] = sourceIndexPointer[i + 1];
                destIndexPointer[i + 2] = sourceIndexPointer[i];
            }

            // Copy positions to buffer; go from float4 to float3
            m_vertexBuffers.emplace_back(3 * mesh->VertexPositions->ElementCount);
            auto destVertexPointer = m_vertexBuffers.back().data();
            auto sourceVertexPointer = reinterpret_cast<float*>(GetPointerToData(mesh->VertexPositions->Data));
            for (unsigned int i = 0; i < mesh->VertexPositions->ElementCount; i++) {
                memcpy(destVertexPointer, sourceVertexPointer, 3 * sizeof(float));
                destVertexPointer += 3;
                sourceVertexPointer += 4;
            }

            // Copy mesh ID
            m_meshIds.emplace_back(sizeof(GUID));
            memcpy(m_meshIds.back().data(), &mesh->SurfaceInfo->Id, sizeof(GUID));

            m_processingLock.unlock();
        },
        concurrency::task_continuation_context::use_arbitrary());
}
