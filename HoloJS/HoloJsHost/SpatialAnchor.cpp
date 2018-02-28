#include "pch.h"
#include "SpatialAnchor.h"
#include "IBufferOnMemory.h"

using namespace HologramJS::Spatial;
using namespace Windows::Perception;
using namespace std;
using namespace concurrency;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage::Streams;

SpatialAnchor* SpatialAnchor::CreateRelativeTo(Windows::Perception::Spatial::SpatialCoordinateSystem ^ coordinateSystem,
                                               const Windows::Foundation::Numerics::float3& position,
                                               const Windows::Foundation::Numerics::quaternion& orientation)
{
    std::unique_ptr<SpatialAnchor> newAnchor;
    newAnchor.reset(new SpatialAnchor());
    newAnchor->m_anchor =
        Windows::Perception::Spatial::SpatialAnchor::TryCreateRelativeTo(coordinateSystem, position, orientation);

    return newAnchor->m_anchor == nullptr ? nullptr : newAnchor.release();
}

SpatialAnchor* SpatialAnchor::FromSpatialAnchor(Windows::Perception::Spatial::SpatialAnchor ^ anchor)
{
    auto newAnchor = new SpatialAnchor();
    newAnchor->m_anchor = anchor;

    return newAnchor;
}

task<void> SpatialAnchor::SaveAsync(const wstring anchorName,
	JsValueRef anchorRef,
	JsValueRef callback)
{
	auto autoReleaseCallback = JsRefReleaseAtScopeExit(callback);
	auto autoReleaseAnchorRef = JsRefReleaseAtScopeExit(anchorRef);
	auto autoInvokeCallbackOnFailure = JsCallAtScopeExit(callback);

	auto anchorStore = await Windows::Perception::Spatial::SpatialAnchorManager::RequestStoreAsync();
	EXIT_IF_TRUE(anchorStore == nullptr);

	auto savedAnchors = anchorStore->GetAllSavedAnchors();
	if (savedAnchors->HasKey(Platform::StringReference(anchorName.c_str()).GetString())) {
		anchorStore->Remove(Platform::StringReference(anchorName.c_str()).GetString());
	}

	auto saveResult = anchorStore->TrySave(Platform::StringReference(anchorName.c_str()), m_anchor);

	JsValueRef arguments[2];
	arguments[0] = callback;
	EXIT_IF_JS_ERROR(JsBoolToBoolean(saveResult, &arguments[1]));

	JsValueRef result;
	EXIT_IF_JS_ERROR(JsCallFunction(callback, arguments, ARRAYSIZE(arguments), &result));

	autoInvokeCallbackOnFailure.Revoke();
}


task<void> SpatialAnchor::ExportAsync(const wstring anchorName, JsValueRef anchorRef, JsValueRef callback)
{
	auto autoReleaseCallback = JsRefReleaseAtScopeExit(callback);
	auto autoReleaseAnchorRef = JsRefReleaseAtScopeExit(anchorRef);
	auto autoInvokeCallbackOnFailure = JsCallAtScopeExit(callback);

	// Request access to the transfer manager
	auto accessResult = await ::Spatial::SpatialAnchorTransferManager::RequestAccessAsync();
	EXIT_IF_TRUE(accessResult != ::Spatial::SpatialPerceptionAccessStatus::Allowed);

	// Create a map of the anchors that should be exported
	IMap<Platform::String^, ::Spatial::SpatialAnchor^>^ map = ref new Map<Platform::String^, ::Spatial::SpatialAnchor^>();
	map->Insert(Platform::StringReference(anchorName.c_str()).GetString(), m_anchor);

	// Create an memory stream to hold the result
	InMemoryRandomAccessStream^ stream = ref new InMemoryRandomAccessStream();

	auto exportResult = await ::Spatial::SpatialAnchorTransferManager::TryExportAnchorsAsync(map, stream);
	EXIT_IF_FALSE(exportResult);

	// Create an array to store the result for the script
	JsValueRef anchorStreamRef;
	EXIT_IF_JS_ERROR(JsCreateArrayBuffer(stream->Size, &anchorStreamRef));

	// Get pointer to the script's array
	BYTE* anchorNativeBuffer;
	unsigned int anchorBufferLength;
	EXIT_IF_JS_ERROR(JsGetArrayBufferStorage(anchorStreamRef, &anchorNativeBuffer, &anchorBufferLength));

	// Create an IBuffer over the script's array
	Microsoft::WRL::ComPtr<HologramJS::Utilities::BufferOnMemory> anchorBufferOnMemory;
	Microsoft::WRL::Details::MakeAndInitialize<HologramJS::Utilities::BufferOnMemory>(
		&anchorBufferOnMemory, anchorNativeBuffer, anchorBufferLength);
	auto iinspectable = (IInspectable*)reinterpret_cast<IInspectable*>(anchorBufferOnMemory.Get());
	IBuffer ^ anchorBuffer = reinterpret_cast<IBuffer ^>(iinspectable);

	// Copy the serialized anchor from the stream to the script array
	stream->Seek(0);
	await stream->ReadAsync(anchorBuffer, anchorBufferLength, InputStreamOptions::None);

	JsValueRef arguments[2];
	arguments[0] = callback;
	arguments[1] = anchorStreamRef;

	// Invoke the "done" callbacl
	JsValueRef result;
	EXIT_IF_JS_ERROR(JsCallFunction(callback, arguments, ARRAYSIZE(arguments), &result));
	
	// We were successfull, dismiss the "autoinvoke on error" object
	autoInvokeCallbackOnFailure.Revoke();
}
