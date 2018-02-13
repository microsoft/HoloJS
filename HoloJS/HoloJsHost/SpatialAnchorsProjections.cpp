#include "pch.h"
#include "SpatialAnchorsProjections.h"
#include "ExternalObject.h"
#include "IBufferOnMemory.h"
#include "ScriptHostUtilities.h"
#include "ScriptResourceTracker.h"
#include "SpatialAnchor.h"

using namespace HologramJS::Utilities;
using namespace HologramJS::Spatial;
using namespace HologramJS::API;
using namespace Windows::Foundation::Numerics;
using namespace Windows::Perception::Spatial;
using namespace Windows::Storage::Streams;
using namespace concurrency;
using namespace std;

SpatialStationaryFrameOfReference ^ SpatialAnchorsProjections::m_frameOfReference;

bool SpatialAnchorsProjections::Initialize()
{
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"createAnchor", L"anchors", createAnchor));
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"enumerateAnchors", L"anchors", enumerateAnchors));
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"openAnchor", L"anchors", openAnchor));
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"saveAnchor", L"anchors", saveAnchor));
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"deleteAnchor", L"anchors", deleteAnchor));
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"importAnchor", L"anchors", importAnchor));
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"exportAnchor", L"anchors", exportAnchor));
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"getTransformToOrigin", L"anchors", getTransformToOrigin));

    return true;
}

JsValueRef CHAKRA_CALLBACK SpatialAnchorsProjections::getTransformToOrigin(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);
    RETURN_INVALID_REF_IF_NULL(m_frameOfReference);

    auto anchorRef = arguments[1];
    auto anchor = ScriptResourceTracker::ExternalToObject<HologramJS::Spatial::SpatialAnchor>(anchorRef);
    RETURN_INVALID_REF_IF_NULL(anchor);

    auto transfrom = anchor->m_anchor->CoordinateSystem->TryGetTransformTo(m_frameOfReference->CoordinateSystem);
    if (transfrom) {
        JsValueRef transformRef;
        RETURN_INVALID_REF_IF_JS_ERROR(
            JsCreateTypedArray(JsTypedArrayType::JsArrayTypeFloat32, JS_INVALID_REFERENCE, 0, 16, &transformRef));

        float* transformStorage;
        unsigned int transformStorageSize;
        RETURN_INVALID_REF_IF_JS_ERROR(JsGetTypedArrayStorage(
            transformRef, reinterpret_cast<byte**>(&transformStorage), &transformStorageSize, nullptr, nullptr));

        auto t = transfrom->Value;
        memcpy(transformStorage, &(t.m11), 16 * sizeof(float));

        return transformRef;
    } else {
        JsValueRef undefined;
        RETURN_INVALID_REF_IF_JS_ERROR(JsGetUndefinedValue(&undefined));

        return undefined;
    }
}

JsValueRef CHAKRA_CALLBACK SpatialAnchorsProjections::createAnchor(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    RETURN_IF_NULL(m_frameOfReference);

    ExternalObject* externalObject = new ExternalObject();

    float3 position = float3(0, 0, 0);
    if (argumentCount > 3) {
        position.x = ScriptHostUtilities::GLfloatFromJsRef(arguments[1]);
        position.y = ScriptHostUtilities::GLfloatFromJsRef(arguments[2]);
        position.z = ScriptHostUtilities::GLfloatFromJsRef(arguments[3]);
    }

    quaternion rotation = quaternion::identity();
    if (argumentCount > 7) {
        rotation.x = ScriptHostUtilities::GLfloatFromJsRef(arguments[4]);
        rotation.y = ScriptHostUtilities::GLfloatFromJsRef(arguments[5]);
        rotation.z = ScriptHostUtilities::GLfloatFromJsRef(arguments[6]);
        rotation.w = ScriptHostUtilities::GLfloatFromJsRef(arguments[7]);
    }

    RETURN_INVALID_REF_IF_FALSE(externalObject->Initialize(
        SpatialAnchor::CreateRelativeTo(m_frameOfReference->CoordinateSystem, position, rotation)));
    return ScriptResourceTracker::ObjectToDirectExternal(externalObject);
}

JsValueRef CHAKRA_CALLBACK SpatialAnchorsProjections::deleteAnchor(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    wstring anchorName;
    RETURN_INVALID_REF_IF_FALSE(ScriptHostUtilities::GetString(arguments[1], anchorName));

    auto callback = arguments[2];
    RETURN_INVALID_REF_IF_JS_ERROR(JsAddRef(callback, nullptr));

    deleteAnchorAsync(anchorName, callback);

    return JS_INVALID_REFERENCE;
}

JsValueRef CHAKRA_CALLBACK SpatialAnchorsProjections::openAnchor(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    wstring anchorName;
    RETURN_INVALID_REF_IF_FALSE(ScriptHostUtilities::GetString(arguments[1], anchorName));

    auto callback = arguments[2];
    RETURN_INVALID_REF_IF_JS_ERROR(JsAddRef(callback, nullptr));

    openAnchorAsync(anchorName, callback);

    return JS_INVALID_REFERENCE;
}

JsValueRef CHAKRA_CALLBACK SpatialAnchorsProjections::enumerateAnchors(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    auto callback = arguments[1];
    RETURN_INVALID_REF_IF_JS_ERROR(JsAddRef(callback, nullptr));

    enumerateAnchorsAsync(callback);

    return JS_INVALID_REFERENCE;
}

JsValueRef CHAKRA_CALLBACK SpatialAnchorsProjections::saveAnchor(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    auto anchorRef = arguments[1];
    RETURN_INVALID_REF_IF_JS_ERROR(JsAddRef(anchorRef, nullptr));
    auto anchor = ScriptResourceTracker::ExternalToObject<HologramJS::Spatial::SpatialAnchor>(anchorRef);
    RETURN_INVALID_REF_IF_NULL(anchor);

    wstring anchorName;
    RETURN_INVALID_REF_IF_FALSE(ScriptHostUtilities::GetString(arguments[2], anchorName));

    auto callback = arguments[3];
    RETURN_INVALID_REF_IF_JS_ERROR(JsAddRef(callback, nullptr));

    anchor->SaveAsync(anchorName, anchorRef, callback);

    return JS_INVALID_REFERENCE;
}

JsValueRef CHAKRA_CALLBACK SpatialAnchorsProjections::exportAnchor(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    auto anchorRef = arguments[1];
    RETURN_INVALID_REF_IF_JS_ERROR(JsAddRef(anchorRef, nullptr));
    auto anchor = ScriptResourceTracker::ExternalToObject<HologramJS::Spatial::SpatialAnchor>(anchorRef);
    RETURN_INVALID_REF_IF_NULL(anchor);

    wstring anchorName;
    RETURN_INVALID_REF_IF_FALSE(ScriptHostUtilities::GetString(arguments[2], anchorName));

    auto callback = arguments[3];
    RETURN_INVALID_REF_IF_JS_ERROR(JsAddRef(callback, nullptr));

    anchor->ExportAsync(anchorName, anchorRef, callback);

    return JS_INVALID_REFERENCE;
}

JsValueRef CHAKRA_CALLBACK SpatialAnchorsProjections::importAnchor(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    auto dataRef = arguments[1];
    RETURN_INVALID_REF_IF_JS_ERROR(JsAddRef(dataRef, nullptr));

    // Get pointer to the script's array
    BYTE* scriptArrayPointer;
    unsigned int scriptArrayLength;
    RETURN_INVALID_REF_IF_JS_ERROR(JsGetArrayBufferStorage(&dataRef, &scriptArrayPointer, &scriptArrayLength));

    // Create an IBuffer over the script's array
    Microsoft::WRL::ComPtr<HologramJS::Utilities::BufferOnMemory> bufferOnMemory;
    Microsoft::WRL::Details::MakeAndInitialize<HologramJS::Utilities::BufferOnMemory>(
        &bufferOnMemory, scriptArrayPointer, scriptArrayLength);
    auto iinspectable = (IInspectable*)reinterpret_cast<IInspectable*>(bufferOnMemory.Get());
    IBuffer ^ anchorBuffer = reinterpret_cast<IBuffer ^>(iinspectable);

    auto callback = arguments[2];
    RETURN_INVALID_REF_IF_JS_ERROR(JsAddRef(callback, nullptr));

    importAnchorAsync(dataRef, anchorBuffer, callback);

    return JS_INVALID_REFERENCE;
}

task<void> SpatialAnchorsProjections::enumerateAnchorsAsync(JsValueRef callback)
{
    auto autoReleaseCallback = JsRefReleaseAtScopeExit(callback);
    auto autoInvokeCallbackOnFailure = JsCallAtScopeExit(callback);

    auto anchorStore = await SpatialAnchorManager::RequestStoreAsync();
    EXIT_IF_TRUE(anchorStore == nullptr);

    auto savedAnchors = anchorStore->GetAllSavedAnchors();

    JsValueRef resultArray;
    EXIT_IF_JS_ERROR(JsCreateArray(savedAnchors->Size, &resultArray));

    auto iterator = savedAnchors->First();
    int index = 0;
    while (iterator->HasCurrent) {
        JsValueRef anchorEntry;
        EXIT_IF_JS_ERROR(JsCreateObject(&anchorEntry));

        JsValueRef idRef;
        EXIT_IF_JS_ERROR(JsPointerToString(iterator->Current->Key->Data(), iterator->Current->Key->Length(), &idRef));
        EXIT_IF_FALSE(ScriptHostUtilities::SetJsProperty(anchorEntry, L"id", idRef));

        auto externalAnchor = new ExternalObject();
        EXIT_IF_FALSE(externalAnchor->Initialize(
            HologramJS::Spatial::SpatialAnchor::FromSpatialAnchor(iterator->Current->Value)));
        auto anchorRef = ScriptResourceTracker::ObjectToDirectExternal(externalAnchor);
        EXIT_IF_FALSE(ScriptHostUtilities::SetJsProperty(anchorEntry, L"anchor", anchorRef));

        JsValueRef indexRef;
        EXIT_IF_JS_ERROR(JsIntToNumber(index, &indexRef));

        EXIT_IF_JS_ERROR(JsSetIndexedProperty(resultArray, indexRef, anchorEntry));

        iterator->MoveNext();
    }

    JsValueRef arguments[2];
    arguments[0] = callback;
    arguments[1] = resultArray;

    JsValueRef result;
    EXIT_IF_JS_ERROR(JsCallFunction(callback, arguments, ARRAYSIZE(arguments), &result));

    autoInvokeCallbackOnFailure.Revoke();
}

task<void> SpatialAnchorsProjections::openAnchorAsync(const wstring anchorName, JsValueRef callback)
{
    auto autoReleaseCallback = JsRefReleaseAtScopeExit(callback);
    auto autoInvokeCallbackOnFailure = JsCallAtScopeExit(callback);

    auto anchorStore = await SpatialAnchorManager::RequestStoreAsync();
    EXIT_IF_TRUE(anchorStore == nullptr);

    JsValueRef arguments[2];
    arguments[0] = callback;

    auto savedAnchors = anchorStore->GetAllSavedAnchors();
    if (savedAnchors->HasKey(Platform::StringReference(anchorName.c_str()).GetString())) {
        auto anchor = savedAnchors->Lookup(Platform::StringReference(anchorName.c_str()).GetString());

        auto externalAnchor = new ExternalObject();
        EXIT_IF_FALSE(externalAnchor->Initialize(HologramJS::Spatial::SpatialAnchor::FromSpatialAnchor(anchor)));
        arguments[1] = ScriptResourceTracker::ObjectToDirectExternal(externalAnchor);
    } else {
        EXIT_IF_JS_ERROR(JsGetUndefinedValue(&arguments[1]));
    }

    JsValueRef result;
    EXIT_IF_JS_ERROR(JsCallFunction(callback, arguments, ARRAYSIZE(arguments), &result));

    autoInvokeCallbackOnFailure.Revoke();
}

task<void> SpatialAnchorsProjections::deleteAnchorAsync(const wstring anchorName, JsValueRef callback)
{
    auto autoReleaseCallback = JsRefReleaseAtScopeExit(callback);
    auto autoInvokeCallback = JsCallAtScopeExit(callback);

    auto anchorStore = await SpatialAnchorManager::RequestStoreAsync();
    EXIT_IF_TRUE(anchorStore == nullptr);

    auto savedAnchors = anchorStore->GetAllSavedAnchors();
    if (savedAnchors->HasKey(Platform::StringReference(anchorName.c_str()).GetString())) {
        anchorStore->Remove(Platform::StringReference(anchorName.c_str()).GetString());
    }
}

task<void> SpatialAnchorsProjections::importAnchorAsync(JsValueRef dataRef, IBuffer ^ anchorBuffer, JsValueRef callback)
{
    auto autoReleaseDataRef = JsRefReleaseAtScopeExit(dataRef);
    auto autoReleaseCallback = JsRefReleaseAtScopeExit(callback);
    auto autoInvokeCallback = JsCallAtScopeExit(callback);

    // Request access to the transfer manager
    auto accessResult = await SpatialAnchorTransferManager::RequestAccessAsync();
    EXIT_IF_TRUE(accessResult != SpatialPerceptionAccessStatus::Allowed);

    InMemoryRandomAccessStream ^ stream = ref new InMemoryRandomAccessStream();
    await stream->WriteAsync(anchorBuffer);

    auto asyncResult = SpatialAnchorTransferManager::TryImportAnchorsAsync(stream->GetInputStreamAt(0));
    auto importResult = asyncResult->GetResults();
    EXIT_IF_TRUE(importResult == nullptr);

    // Create a JSON with the result: [{ id : "name", anchor : <anchorObject>}]

    JsValueRef resultArray;
    EXIT_IF_JS_ERROR(JsCreateArray(importResult->Size, &resultArray));

    auto iterator = importResult->First();
    int index = 0;
    while (iterator->HasCurrent) {
        JsValueRef anchorEntry;
        EXIT_IF_JS_ERROR(JsCreateObject(&anchorEntry));

        JsValueRef idRef;
        EXIT_IF_JS_ERROR(JsPointerToString(iterator->Current->Key->Data(), iterator->Current->Key->Length(), &idRef));
        EXIT_IF_FALSE(ScriptHostUtilities::SetJsProperty(anchorEntry, L"id", idRef));

        auto externalAnchor = new ExternalObject();
        EXIT_IF_FALSE(externalAnchor->Initialize(
            HologramJS::Spatial::SpatialAnchor::FromSpatialAnchor(iterator->Current->Value)));
        auto anchorRef = ScriptResourceTracker::ObjectToDirectExternal(externalAnchor);
        EXIT_IF_FALSE(ScriptHostUtilities::SetJsProperty(anchorEntry, L"anchor", anchorRef));

        JsValueRef indexRef;
        EXIT_IF_JS_ERROR(JsIntToNumber(index, &indexRef));

        EXIT_IF_JS_ERROR(JsSetIndexedProperty(resultArray, indexRef, anchorEntry));

        iterator->MoveNext();
    }

    JsValueRef arguments[2];
    arguments[0] = callback;
    arguments[1] = resultArray;

    JsValueRef result;
    EXIT_IF_JS_ERROR(JsCallFunction(callback, arguments, ARRAYSIZE(arguments), &result));

    autoInvokeCallback.Revoke();
}
