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
    RETURN_INVALID_REF_IF_JS_ERROR(JsAddRef(arguments[2], nullptr));

    return JS_INVALID_REFERENCE;
}

JsValueRef CHAKRA_CALLBACK SpatialAnchorsProjections::enumerateAnchors(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    RETURN_INVALID_REF_IF_JS_ERROR(JsAddRef(arguments[1], nullptr));

    return JS_INVALID_REFERENCE;
}

task<void> SpatialAnchorsProjections::enumerateAnchorsAsync(JsValueRef callback)
{
    auto anchorStore = await SpatialAnchorManager::RequestStoreAsync();

    auto savedAnchors = anchorStore->GetAllSavedAnchors();

    auto iterator = savedAnchors->First();
    while (iterator->HasCurrent) {
        JsValueRef arguments[2];
        arguments[0] = callback;
        ExternalObject* externalAnchor;
        EXIT_IF_JS_ERROR(externalAnchor->Initialize(
            HologramJS::Spatial::SpatialAnchor::FromSpatialAnchor(iterator->Current->Value)));
        arguments[1] = ScriptResourceTracker::ObjectToDirectExternal(externalAnchor);

        JsValueRef result;
        EXIT_IF_JS_ERROR(JsCallFunction(callback, arguments, ARRAYSIZE(arguments), &result));
    }

    JsRelease(callback, nullptr);
}

task<void> SpatialAnchorsProjections::openAnchorAsync(const wstring anchorName, JsValueRef callback)
{
    auto anchorStore = await SpatialAnchorManager::RequestStoreAsync();

    auto savedAnchors = anchorStore->GetAllSavedAnchors();
    auto anchor = savedAnchors->Lookup(Platform::StringReference(anchorName.c_str()));

    auto iterator = savedAnchors->First();
    while (iterator->HasCurrent) {
        JsValueRef arguments[2];
        arguments[0] = callback;
        ExternalObject* externalAnchor;
        EXIT_IF_JS_ERROR(externalAnchor->Initialize(
            HologramJS::Spatial::SpatialAnchor::FromSpatialAnchor(iterator->Current->Value)));
        arguments[1] = ScriptResourceTracker::ObjectToDirectExternal(externalAnchor);

        JsValueRef result;
        EXIT_IF_JS_ERROR(JsCallFunction(callback, arguments, ARRAYSIZE(arguments), &result));
    }
}
