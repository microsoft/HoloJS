#include "pch.h"
#include "SpatialMapping.h"
#include <WindowsNumerics.h>
#include <Robuffer.h>
#include "InputInterface.h"
#include <DirectXMath.h>

using namespace HologramJS::Input;
using namespace Windows::Perception::Spatial;
using namespace concurrency;
using namespace Windows::Perception::Spatial::Surfaces;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::DirectX;
using namespace HologramJS::Utilities;
using namespace DirectX;

SpatialMapping::SpatialMapping()
{
}


SpatialMapping::~SpatialMapping()
{
}

JsValueRef
SpatialMapping::GetSpatialData()
{
	RETURN_INVALID_REF_IF_FALSE(m_surfaceAccessAllowed);
	RETURN_INVALID_REF_IF_FALSE(CreateSurfaceObserver());

	auto mapContainingSurfaceCollection = m_surfaceObserver->GetObservedSurfaces();

	int counter = 0;
	for (auto const& pair : mapContainingSurfaceCollection)
	{
		// Store the ID and metadata for each surface.
		auto const& id = pair->Key;
		auto const& surfaceInfo = pair->Value;
		ProcessSurface(pair->Value);
	}
	
	return JS_INVALID_REFERENCE;
}

void
SpatialMapping::EnableSpatialMapping(SpatialStationaryFrameOfReference^ frameOfReference)
{
	m_frameOfReference = frameOfReference;

	auto initSurfaceObserverTask = create_task(SpatialSurfaceObserver::RequestAccessAsync());
	initSurfaceObserverTask.then([this](Windows::Perception::Spatial::SpatialPerceptionAccessStatus status)
	{
 		switch (status)
		{
		case SpatialPerceptionAccessStatus::Allowed:
			m_surfaceAccessAllowed = true;
			break;
		default:
			// Access was denied. This usually happens because your AppX manifest file does not declare the
			// spatialPerception capability.
			// For info on what else can cause this, see: http://msdn.microsoft.com/library/windows/apps/mt621422.aspx
			m_surfaceAccessAllowed = false;
			break;
		}
	});
}

bool
SpatialMapping::CreateSurfaceObserver()
{
	RETURN_IF_FALSE(m_surfaceAccessAllowed);

	if (m_surfaceObserver != nullptr)
	{
		return true;
	}

	SpatialBoundingBox axisAlignedBoundingBox =
	{
		{ 0.f,  0.f, 0.f },
		{ 20.f, 20.f, 5.f },
	};
	SpatialBoundingVolume^ bounds = SpatialBoundingVolume::FromBox(m_frameOfReference->CoordinateSystem, axisAlignedBoundingBox);
		
	m_surfaceMeshOptions = ref new SpatialSurfaceMeshOptions();
	IVectorView<DirectXPixelFormat>^ supportedVertexPositionFormats = m_surfaceMeshOptions->SupportedVertexPositionFormats;
	unsigned int formatIndex = 0;
	if (supportedVertexPositionFormats->IndexOf(DirectXPixelFormat::R16G16B16A16IntNormalized, &formatIndex))
	{
		m_surfaceMeshOptions->VertexPositionFormat = DirectXPixelFormat::R16G16B16A16IntNormalized;
	}

	IVectorView<DirectXPixelFormat>^ supportedVertexNormalFormats = m_surfaceMeshOptions->SupportedVertexNormalFormats;
	if (supportedVertexNormalFormats->IndexOf(DirectXPixelFormat::R8G8B8A8IntNormalized, &formatIndex))
	{
		m_surfaceMeshOptions->VertexNormalFormat = DirectXPixelFormat::R8G8B8A8IntNormalized;
	}

	m_surfaceMeshOptions->IncludeVertexNormals = true;

	// Create the observer.
	m_surfaceObserver = ref new SpatialSurfaceObserver();
	RETURN_IF_NULL(m_surfaceObserver);
	
	m_surfaceObserver->SetBoundingVolume(bounds);

	return true;
}

byte* GetPointerToData(Windows::Storage::Streams::IBuffer^ buffer)
{
	Microsoft::WRL::ComPtr<Windows::Storage::Streams::IBufferByteAccess> bufferByteAccess;
	reinterpret_cast<IInspectable*>(buffer)->QueryInterface(IID_PPV_ARGS(&bufferByteAccess));

	byte* pointer;
	bufferByteAccess->Buffer(&pointer);

	return pointer;
}

void
SpatialMapping::ProcessSurface(
	SpatialSurfaceInfo^ surface
)
{
	auto computeMeshTask = create_task(surface->TryComputeLatestMeshAsync(10000, m_surfaceMeshOptions));

	computeMeshTask.then([this](SpatialSurfaceMesh^ mesh)
	{
		JsTypedArrayType arrayType;
		int elementSize;
		float* typedArrayBuffer;
		unsigned int typedArraySize;

		if (mesh == nullptr)
		{
			return;
		}

		JsValueRef meshObject;
		EXIT_IF_JS_ERROR(JsCreateObject(&meshObject));

		// Combine origin transform with mesh scale to create position transform
		auto originToSurface = mesh->CoordinateSystem->TryGetTransformTo(m_frameOfReference->CoordinateSystem);
		DirectX::XMMATRIX translationMat = XMLoadFloat4x4(&originToSurface->Value);
		XMMATRIX scaleMat = XMMatrixScaling(mesh->VertexPositionScale.x, mesh->VertexPositionScale.y, mesh->VertexPositionScale.z);
		XMMATRIX posTransformMat = scaleMat *translationMat;
		XMFLOAT4X4 posTransformf4;
		XMStoreFloat4x4(&posTransformf4, posTransformMat);

		// Store position transform
		JsValueRef originToSurfaceJsArray;
		EXIT_IF_JS_ERROR(JsCreateTypedArray(JsArrayTypeFloat32, JS_INVALID_REFERENCE, 0, 16, &originToSurfaceJsArray));
		EXIT_IF_JS_ERROR(JsGetTypedArrayStorage(originToSurfaceJsArray, reinterpret_cast<BYTE**>(&typedArrayBuffer), &typedArraySize, &arrayType, &elementSize));
		EXIT_IF_FALSE(typedArraySize == sizeof(posTransformf4));
		memcpy(typedArrayBuffer, posTransformf4.m, sizeof(posTransformf4));
		EXIT_IF_FALSE(ScriptHostUtilities::SetJsProperty(meshObject, L"originToSurfaceTransform", originToSurfaceJsArray));

		// Compute normals transform
		// Normals transforms for non-uniform scaling require the inverse transpose of the original transform
		XMMATRIX normTransformMat = XMMatrixTranspose(XMMatrixInverse(nullptr, posTransformMat));
		XMFLOAT4X4 normTransformf4;
		XMStoreFloat4x4(&normTransformf4, posTransformMat);

		// Store normals transform
		JsValueRef normalsTransformJsArray;
		EXIT_IF_JS_ERROR(JsCreateTypedArray(JsArrayTypeFloat32, JS_INVALID_REFERENCE, 0, 16, &normalsTransformJsArray));
		EXIT_IF_JS_ERROR(JsGetTypedArrayStorage(normalsTransformJsArray, reinterpret_cast<BYTE**>(&typedArrayBuffer), &typedArraySize, &arrayType, &elementSize));
		EXIT_IF_FALSE(typedArraySize == sizeof(normTransformf4));
		memcpy(typedArrayBuffer, normTransformf4.m, sizeof(normTransformf4));
		EXIT_IF_FALSE(ScriptHostUtilities::SetJsProperty(meshObject, L"normalTransform", normalsTransformJsArray));

		// Store normals array;
		JsValueRef normalsJsArray;
		const auto normalsElementCount = 4 * mesh->VertexNormals->ElementCount; // 4 * R8G8B8A8IntNormalized
		EXIT_IF_JS_ERROR(JsCreateTypedArray(JsArrayTypeInt8, JS_INVALID_REFERENCE, 0, normalsElementCount, &normalsJsArray));
		EXIT_IF_JS_ERROR(JsGetTypedArrayStorage(normalsJsArray, reinterpret_cast<BYTE**>(&typedArrayBuffer), &typedArraySize, &arrayType, &elementSize));
		EXIT_IF_FALSE(typedArraySize == mesh->VertexNormals->Data->Length);
		memcpy(typedArrayBuffer, GetPointerToData(mesh->VertexNormals->Data), mesh->VertexNormals->Data->Length);
		EXIT_IF_FALSE(ScriptHostUtilities::SetJsProperty(meshObject, L"normals", normalsJsArray));

		// Store indices array
		JsValueRef indicesArray;
		EXIT_IF_JS_ERROR(JsCreateTypedArray(JsArrayTypeInt16, JS_INVALID_REFERENCE, 0, mesh->TriangleIndices->ElementCount, &indicesArray));
		EXIT_IF_JS_ERROR(JsGetTypedArrayStorage(indicesArray, reinterpret_cast<BYTE**>(&typedArrayBuffer), &typedArraySize, &arrayType, &elementSize));
		EXIT_IF_FALSE(typedArraySize == mesh->TriangleIndices->Data->Length);
		memcpy(typedArrayBuffer, GetPointerToData(mesh->TriangleIndices->Data), mesh->TriangleIndices->Data->Length);
		EXIT_IF_FALSE(ScriptHostUtilities::SetJsProperty(meshObject, L"indices", indicesArray));

		//Store positions array
		JsValueRef positionsArray;
		const auto positionsElementCount = 4 * mesh->VertexPositions->ElementCount; // 4 * R16G16B16A16
		EXIT_IF_JS_ERROR(JsCreateTypedArray(JsArrayTypeInt16, JS_INVALID_REFERENCE, 0, positionsElementCount, &positionsArray));
		EXIT_IF_JS_ERROR(JsGetTypedArrayStorage(positionsArray, reinterpret_cast<BYTE**>(&typedArrayBuffer), &typedArraySize, &arrayType, &elementSize));
		EXIT_IF_FALSE(typedArraySize == mesh->VertexPositions->Data->Length);
		memcpy(typedArrayBuffer, GetPointerToData(mesh->VertexPositions->Data), mesh->VertexPositions->Data->Length);
		EXIT_IF_FALSE(ScriptHostUtilities::SetJsProperty(meshObject, L"vertices", positionsArray));

		// Add mesh ID
		JsValueRef meshIdRef;
		const auto meshGuidString = mesh->SurfaceInfo->Id.ToString();
		EXIT_IF_JS_ERROR(JsPointerToString(meshGuidString->Data(), meshGuidString->Length(), &meshIdRef));
		EXIT_IF_FALSE(ScriptHostUtilities::SetJsProperty(meshObject, L"id", meshIdRef));

		// Create JS callback parameters
		JsValueRef parameters[3];
		parameters[0] = m_scriptCallback;
		EXIT_IF_JS_ERROR(JsIntToNumber(static_cast<int>(NativeToScriptInputType::SpatialMapping), &parameters[1]));
		parameters[2] = meshObject;

		// Invoke JS callback
		JsValueRef result;
		EXIT_IF_JS_ERROR(JsCallFunction(m_scriptCallback, parameters, ARRAYSIZE(parameters), &result));
	});
}
