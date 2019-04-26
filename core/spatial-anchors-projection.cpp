#include "spatial-anchors-projection.h"

#include "blob.h"
#include "include/holojs/private/error-handling.h"
#include "include/holojs/private/platform-interfaces.h"
#include "include/holojs/private/script-host-utilities.h"
#include "include/holojs/private/speech-recognizer.h"
#include "resource-management/external-object.h"
#include <array>
#include <string>
#include <vector>

using namespace HoloJs;
using namespace HoloJs::ResourceManagement;
using namespace std;

long SpatialAnchorsProjection::initialize()
{
    JS_PROJECTION_REGISTER(L"SpatialAnchors", L"areAvailable", areAvailable);
    JS_PROJECTION_REGISTER(L"SpatialAnchors", L"canPersist", canPersist);
    JS_PROJECTION_REGISTER(L"SpatialAnchors", L"createAnchor", createAnchor);
    JS_PROJECTION_REGISTER(L"SpatialAnchors", L"saveAnchor", saveAnchor);
    JS_PROJECTION_REGISTER(L"SpatialAnchors", L"deleteAnchor", deleteAnchor)
    JS_PROJECTION_REGISTER(L"SpatialAnchors", L"enumerateAnchors", enumerateAnchors);
    JS_PROJECTION_REGISTER(L"SpatialAnchors", L"createStore", createStore);
    JS_PROJECTION_REGISTER(L"SpatialAnchors", L"initializeStore", initializeStore);
    JS_PROJECTION_REGISTER(L"SpatialAnchors", L"openAnchor", openAnchor);
    JS_PROJECTION_REGISTER(L"SpatialAnchors", L"tryGetTransformTo", tryGetTransformTo);

    return HoloJs::Success;
}

JsValueRef SpatialAnchorsProjection::_createStore(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    RETURN_INVALID_REF_IF_FALSE(getPlatform()->canPersistAnchors(m_host));

    auto newStore = getPlatform()->getSpatialAnchorsStore(m_host);
    RETURN_INVALID_REF_IF_NULL(newStore);

    auto storeNativeRef =
        m_resourceManager->objectToDirectExternal<ISpatialAnchorsStore>(newStore, ObjectType::ISpatialAnchorStore);

    RETURN_INVALID_REF_IF_JS_ERROR(ScriptHostUtilities::SetJsProperty(arguments[1], L"native", storeNativeRef));

    return JS_INVALID_REFERENCE;
}

JsValueRef SpatialAnchorsProjection::_initializeStore(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    auto storeRef = arguments[1];
    RETURN_INVALID_REF_IF_JS_ERROR(JsAddRef(storeRef, nullptr));
    auto store = m_resourceManager->externalToObject<ISpatialAnchorsStore>(storeRef, ObjectType::ISpatialAnchorStore);
    RETURN_INVALID_REF_IF_NULL(store);

    store->initialize(storeRef);

    return JS_INVALID_REFERENCE;
}

JsValueRef SpatialAnchorsProjection::_canPersist(JsValueRef* arguments, unsigned short argumentCount)
{
    bool canPersist = getPlatform()->canPersistAnchors(m_host);
    JsValueRef canPersistRef;
    RETURN_INVALID_REF_IF_JS_ERROR(JsBoolToBoolean(canPersist, &canPersistRef));

    return canPersistRef;
}

JsValueRef SpatialAnchorsProjection::_areAvailable(JsValueRef* arguments, unsigned short argumentCount)
{
    bool anchorsAvailable = getPlatform()->spatialAnchorsSupported(m_host);
    JsValueRef anchorsAvailableRef;
    RETURN_INVALID_REF_IF_JS_ERROR(JsBoolToBoolean(anchorsAvailable, &anchorsAvailableRef));

    return anchorsAvailableRef;
}

JsValueRef SpatialAnchorsProjection::_createAnchor(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    auto optionsRef = arguments[2];
    JsValueRef positionRef;
    array<double, 3> position;
    if (SUCCEEDED(ScriptHostUtilities::GetJsProperty(optionsRef, L"position", &positionRef))) {
        const PCWSTR componentNames[] = {L"x", L"y", L"z"};
        for (int i = 0; i < position.size(); i++) {
            JsValueRef componentRef;
            RETURN_INVALID_REF_IF_FAILED(
                ScriptHostUtilities::GetJsProperty(positionRef, componentNames[i], &componentRef));
            position[i] = ScriptHostUtilities::doubleFromJsRef(componentRef);
        }
    } else {
        position.fill(0);
    }

    JsValueRef orientationRef;
    array<double, 4> orientation;
    if (SUCCEEDED(ScriptHostUtilities::GetJsProperty(optionsRef, L"orientation", &orientationRef))) {
        const PCWSTR componentNames[] = {L"x", L"y", L"z", L"w"};
        for (int i = 0; i < orientation.size(); i++) {
            JsValueRef componentRef;
            RETURN_INVALID_REF_IF_FAILED(
                ScriptHostUtilities::GetJsProperty(orientationRef, componentNames[i], &componentRef));
            orientation[i] = ScriptHostUtilities::doubleFromJsRef(componentRef);
        }
    } else {
        orientation.fill(0);
        orientation[3] = 1;
    }

    JsValueRef relativeToRef;
    ISpatialAnchor* relativeTo = nullptr;
    if (SUCCEEDED(ScriptHostUtilities::GetJsProperty(optionsRef, L"relativeTo", &relativeToRef))) {
        JsValueRef nativeRelativeToRef;
        RETURN_INVALID_REF_IF_FAILED(
            ScriptHostUtilities::GetJsProperty(relativeToRef, L"native", &nativeRelativeToRef));

        relativeTo =
            m_resourceManager->externalToObject<ISpatialAnchor>(nativeRelativeToRef, ObjectType::ISpatialAnchor);
        RETURN_INVALID_REF_IF_NULL(relativeTo);
    }

    auto newAnchor = getPlatform()->createAnchor(m_host, relativeTo, position, orientation);
    RETURN_INVALID_REF_IF_NULL(newAnchor);

    auto externalAnchor = m_resourceManager->objectToDirectExternal(newAnchor, ObjectType::ISpatialAnchor);
    RETURN_INVALID_REF_IF_TRUE(externalAnchor == JS_INVALID_REFERENCE);

    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::SetJsProperty(arguments[1], L"native", externalAnchor));

    return JS_INVALID_REFERENCE;
}

JsValueRef SpatialAnchorsProjection::_openAnchor(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    auto store =
        m_resourceManager->externalToObject<ISpatialAnchorsStore>(arguments[1], ObjectType::ISpatialAnchorStore);
    RETURN_INVALID_REF_IF_NULL(store);

    wstring anchorName;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetString(arguments[2], anchorName));

    HoloJs::ISpatialAnchor* openedAnchor;
    RETURN_INVALID_REF_IF_FAILED(store->open(anchorName, &openedAnchor));

    return m_resourceManager->objectToDirectExternal(openedAnchor, ObjectType::ISpatialAnchor);
}

JsValueRef SpatialAnchorsProjection::_saveAnchor(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    auto store =
        m_resourceManager->externalToObject<ISpatialAnchorsStore>(arguments[1], ObjectType::ISpatialAnchorStore);
    RETURN_INVALID_REF_IF_NULL(store);

    auto anchor = m_resourceManager->externalToObject<ISpatialAnchor>(arguments[2], ObjectType::ISpatialAnchor);
    RETURN_INVALID_REF_IF_NULL(anchor);

    wstring anchorName;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetString(arguments[3], anchorName));

    RETURN_INVALID_REF_IF_FAILED(store->save(anchor, anchorName));

    JsValueRef trueRef;
    RETURN_INVALID_REF_IF_JS_ERROR(JsGetTrueValue(&trueRef));

    return trueRef;
}

JsValueRef SpatialAnchorsProjection::_deleteAnchor(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    auto store =
        m_resourceManager->externalToObject<ISpatialAnchorsStore>(arguments[1], ObjectType::ISpatialAnchorStore);
    RETURN_INVALID_REF_IF_NULL(store);

    wstring anchorName;

    JsValueType deleteType;
    RETURN_INVALID_REF_IF_JS_ERROR(JsGetValueType(arguments[2], &deleteType));
    if (deleteType == JsString) {
        RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetString(arguments[2], anchorName));
    } else if (deleteType == JsObject) {
        JsValueRef nativeAnchorRef;
        RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetJsProperty(arguments[2], L"native", &nativeAnchorRef));

        auto anchor = m_resourceManager->externalToObject<ISpatialAnchor>(nativeAnchorRef, ObjectType::ISpatialAnchor);
        RETURN_INVALID_REF_IF_NULL(anchor);
        RETURN_INVALID_REF_IF_FALSE(anchor->isPersisted());
        RETURN_INVALID_REF_IF_FAILED(anchor->getName(anchorName));
    }

    RETURN_INVALID_REF_IF_FAILED(store->deleteAnchor(anchorName));

    JsValueRef trueRef;
    RETURN_INVALID_REF_IF_JS_ERROR(JsGetTrueValue(&trueRef));

    return trueRef;
}

JsValueRef SpatialAnchorsProjection::_tryGetTransformTo(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    auto anchor = m_resourceManager->externalToObject<ISpatialAnchor>(arguments[1], ObjectType::ISpatialAnchor);
    RETURN_INVALID_REF_IF_NULL(anchor);

    ISpatialAnchor* otherAnchor = nullptr;

    JsValueType otherAnchorType;
    RETURN_INVALID_REF_IF_JS_ERROR(JsGetValueType(arguments[2], &otherAnchorType));
    if (otherAnchorType == JsObject) {
        JsValueRef nativeAnchorRef;
        RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetJsProperty(arguments[2], L"native", &nativeAnchorRef));

        otherAnchor = m_resourceManager->externalToObject<ISpatialAnchor>(nativeAnchorRef, ObjectType::ISpatialAnchor);
        RETURN_INVALID_REF_IF_NULL(otherAnchor);
    }

    array<double, 3> position;
    array<double, 4> orientation;
    RETURN_INVALID_REF_IF_FAILED(anchor->tryGetPositionRelativeTo(m_host, otherAnchor, position, orientation));

    JsValueRef positionRef;
    RETURN_INVALID_REF_IF_JS_ERROR(JsCreateObject(&positionRef));
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::SetJsProperty(positionRef, L"x", position[0]));
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::SetJsProperty(positionRef, L"y", position[1]));
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::SetJsProperty(positionRef, L"z", position[2]));

    JsValueRef orientationRef;
    RETURN_INVALID_REF_IF_JS_ERROR(JsCreateObject(&orientationRef));
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::SetJsProperty(orientationRef, L"x", orientation[0]));
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::SetJsProperty(orientationRef, L"y", orientation[1]));
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::SetJsProperty(orientationRef, L"z", orientation[2]));
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::SetJsProperty(orientationRef, L"w", orientation[3]));

    JsValueRef returnValue;
    RETURN_INVALID_REF_IF_JS_ERROR(JsCreateObject(&returnValue));
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::SetJsProperty(returnValue, L"position", positionRef));
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::SetJsProperty(returnValue, L"orientation", orientationRef));

    return returnValue;
}

JsValueRef SpatialAnchorsProjection::_enumerateAnchors(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    auto storeRef = arguments[1];
    RETURN_INVALID_REF_IF_JS_ERROR(JsAddRef(storeRef, nullptr));
    auto store = m_resourceManager->externalToObject<ISpatialAnchorsStore>(storeRef, ObjectType::ISpatialAnchorStore);
    RETURN_INVALID_REF_IF_NULL(store);

    auto anchors = store->enumerate();

    RETURN_INVALID_REF_IF_TRUE(anchors.size() > UINT32_MAX);

    JsValueRef anchorsArray;
    RETURN_INVALID_REF_IF_JS_ERROR(JsCreateArray(static_cast<unsigned int>(anchors.size()), &anchorsArray));

    for (int i = 0; i < anchors.size(); i++) {
        JsValueRef indexRef;
        RETURN_INVALID_REF_IF_JS_ERROR(JsIntToNumber(i, &indexRef));

        JsValueRef anchorRef =
            m_resourceManager->objectToDirectExternal<ISpatialAnchor>(anchors[i], ObjectType::ISpatialAnchor);

        JsValueRef anchorPair;
        RETURN_INVALID_REF_IF_JS_ERROR(JsCreateObject(&anchorPair));

        wstring anchorName;
        RETURN_INVALID_REF_IF_FAILED(anchors[i]->getName(anchorName));

        RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::SetJsProperty(anchorPair, L"name", anchorName));
        RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::SetJsProperty(anchorPair, L"anchor", anchorRef));

        RETURN_INVALID_REF_IF_JS_ERROR(JsSetIndexedProperty(anchorsArray, indexRef, anchorPair));
    }

    return anchorsArray;
}