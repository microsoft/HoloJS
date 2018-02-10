#include "pch.h"
#include "SpatialAnchorsProjections.h"
#include "ExternalObject.h"
#include "ScriptHostUtilities.h"
#include "ScriptResourceTracker.h"
#include "SpatialAnchor.h"

using namespace HologramJS::Utilities;
using namespace HologramJS::Spatial;
using namespace HologramJS::API;
using namespace Windows::Foundation::Numerics;
using namespace Windows::Perception::Spatial;
using namespace concurrency;
using namespace std;

SpatialStationaryFrameOfReference ^ SpatialAnchorsProjections::m_frameOfReference;

bool SpatialAnchorsProjections::Initialize(SpatialStationaryFrameOfReference ^ frameOfReference)
{
    m_frameOfReference = frameOfReference;
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"createAnchor", L"anchors", createAnchor));
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"enumerateAnchors", L"anchors", enumerateAnchors));
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"openAnchor", L"anchors", openAnchor));
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"saveAnchor", L"anchors", saveAnchor));

    return true;
}

JsValueRef CHAKRA_CALLBACK SpatialAnchorsProjections::createAnchor(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    ExternalObject* externalObject = new ExternalObject();

    float3 position = float3(0, 0, 0);
    if (argumentCount > 3) {
        position.x = ScriptHostUtilities::GLfloatFromJsRef(arguments[1]);
        position.y = ScriptHostUtilities::GLfloatFromJsRef(arguments[2]);
        position.z = ScriptHostUtilities::GLfloatFromJsRef(arguments[3]);
    }

    quaternion rotation;
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
    
    saveAnchorAsync(anchorName, anchor, callback);

    return JS_INVALID_REFERENCE;
}

task<void> SpatialAnchorsProjections::enumerateAnchorsAsync(JsValueRef callback)
{
    auto autoReleaseCallback = JsRefReleaseAtScopeExit(callback);
    auto anchorStore = await SpatialAnchorManager::RequestStoreAsync();

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
        EXIT_IF_JS_ERROR(externalAnchor->Initialize(
            HologramJS::Spatial::SpatialAnchor::FromSpatialAnchor(iterator->Current->Value)));
        auto anchorRef = ScriptResourceTracker::ObjectToDirectExternal(externalAnchor);
        EXIT_IF_FALSE(ScriptHostUtilities::SetJsProperty(anchorEntry, L"anchor", anchorRef));

        JsValueRef indexRef;
        EXIT_IF_JS_ERROR(JsIntToNumber(index, &indexRef));

        JsSetIndexedProperty(resultArray, indexRef, anchorEntry);
    }

    JsValueRef arguments[2];
    arguments[0] = callback;
    arguments[1] = resultArray;

    JsValueRef result;
    EXIT_IF_JS_ERROR(JsCallFunction(callback, arguments, ARRAYSIZE(arguments), &result));
}

task<void> SpatialAnchorsProjections::openAnchorAsync(const wstring anchorName, JsValueRef callback)
{
    auto autoReleaseCallback = JsRefReleaseAtScopeExit(callback);
    auto anchorStore = await SpatialAnchorManager::RequestStoreAsync();

    auto savedAnchors = anchorStore->GetAllSavedAnchors();
    auto anchor = savedAnchors->Lookup(Platform::StringReference(anchorName.c_str()));

    JsValueRef arguments[2];
    arguments[0] = callback;
    if (anchor) {
        auto externalAnchor = new ExternalObject();
        EXIT_IF_JS_ERROR(externalAnchor->Initialize(HologramJS::Spatial::SpatialAnchor::FromSpatialAnchor(anchor)));
        arguments[1] = ScriptResourceTracker::ObjectToDirectExternal(externalAnchor);
    } else {
        EXIT_IF_JS_ERROR(JsGetUndefinedValue(&arguments[1]));
    }

    JsValueRef result;
    EXIT_IF_JS_ERROR(JsCallFunction(callback, arguments, ARRAYSIZE(arguments), &result));
}

task<void> SpatialAnchorsProjections::saveAnchorAsync(const wstring anchorName,
                                                      HologramJS::Spatial::SpatialAnchor* anchor,
                                                      JsValueRef callback)
{
    auto autoReleaseCallback = JsRefReleaseAtScopeExit(callback);
    auto anchorStore = await SpatialAnchorManager::RequestStoreAsync();

    auto saveResult = anchorStore->TrySave(Platform::StringReference(anchorName.c_str()), anchor->m_anchor);

    JsValueRef arguments[2];
    arguments[0] = callback;
    EXIT_IF_JS_ERROR(JsBoolToBoolean(saveResult, &arguments[1]));

    JsValueRef result;
    EXIT_IF_JS_ERROR(JsCallFunction(callback, arguments, ARRAYSIZE(arguments), &result));
}
