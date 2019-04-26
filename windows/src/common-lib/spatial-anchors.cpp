#include "stdafx.h"
#include "holojs/private/error-handling.h"
#include "holojs/private/holojs-script-host-internal.h"
#include "holojs/private/script-host-utilities.h"
#include "spatial-anchors.h"
#include <ppltasks.h>

using namespace concurrency;

using namespace HoloJs::Platforms::Win32;
using namespace Windows::Perception::Spatial;
using namespace Windows::Foundation::Numerics;
using namespace std;
using namespace concurrency;
using namespace DirectX;

bool SpatialAnchors::areAnchorsImplemented()
{
    void* stationaryCoordinateSystemUnknown;

    return SUCCEEDED(m_host->getStationaryCoordinateSystem(&stationaryCoordinateSystemUnknown)) &&
           stationaryCoordinateSystemUnknown != nullptr;
}

HoloJs::ISpatialAnchor* HoloJsSpatialAnchor::create(HoloJs::IHoloJsScriptHostInternal* host,
                                                    HoloJs::ISpatialAnchor* relativeTo,
                                                    const std::array<double, 3>& position,
                                                    const std::array<double, 4>& orientation)
{
    SpatialCoordinateSystem ^ referenceSystem;

    if (relativeTo != nullptr) {
        auto spatialAnchor = static_cast<HoloJsSpatialAnchor*>(relativeTo);
        RETURN_NULL_IF_TRUE(spatialAnchor == nullptr);

        auto platformAnchor = spatialAnchor->getPerceptionAnchor();
        RETURN_NULL_IF_TRUE(platformAnchor == nullptr);

        referenceSystem = platformAnchor->CoordinateSystem;
    } else {
        void* stationaryCoordinateSystemUnknown;
        RETURN_NULL_IF_FAILED(host->getStationaryCoordinateSystem(&stationaryCoordinateSystemUnknown));

        referenceSystem = safe_cast<SpatialCoordinateSystem ^>(
            reinterpret_cast<Platform::Object ^>(stationaryCoordinateSystemUnknown));
    }

    float3 positionInitializer;
    positionInitializer.x = static_cast<float>(position[0]);
    positionInitializer.y = static_cast<float>(position[1]);
    positionInitializer.z = static_cast<float>(position[2]);

    quaternion orientationInitializer;
    orientationInitializer.x = static_cast<float>(orientation[0]);
    orientationInitializer.y = static_cast<float>(orientation[1]);
    orientationInitializer.z = static_cast<float>(orientation[2]);
    orientationInitializer.w = static_cast<float>(orientation[3]);

    auto newAnchor = SpatialAnchor::TryCreateRelativeTo(referenceSystem, positionInitializer, orientationInitializer);

    return new HoloJsSpatialAnchor(newAnchor);
}

long HoloJsSpatialAnchor::tryGetPositionRelativeTo(HoloJs::IHoloJsScriptHostInternal* host,
                                                   HoloJs::ISpatialAnchor* other,
                                                   std::array<double, 3>& position,
                                                   std::array<double, 4>& orientation)
{
    SpatialCoordinateSystem ^ otherSystem;

    if (other != nullptr) {
        auto otherSpatialAnchor = static_cast<HoloJsSpatialAnchor*>(other);
        RETURN_IF_TRUE(otherSpatialAnchor == nullptr);

        auto otherPlatformAnchor = otherSpatialAnchor->getPerceptionAnchor();
        RETURN_IF_TRUE(otherPlatformAnchor == nullptr);

        otherSystem = otherPlatformAnchor->CoordinateSystem;
    } else {
        void* stationaryCoordinateSystemUnknown;
        RETURN_IF_FAILED(host->getStationaryCoordinateSystem(&stationaryCoordinateSystemUnknown));

        otherSystem = safe_cast<SpatialCoordinateSystem ^>(
            reinterpret_cast<Platform::Object ^>(stationaryCoordinateSystemUnknown));
    }

    auto transformBox = m_anchor->CoordinateSystem->TryGetTransformTo(otherSystem);
    RETURN_IF_TRUE(transformBox == nullptr);
    auto transform = transformBox->Value;

    JsValueRef transformRef;
    RETURN_IF_FAILED(ScriptHostUtilities::CreateTypedArrayFromBuffer(
        JsArrayTypeFloat32, &transformRef, 16, reinterpret_cast<unsigned char*>(&transform.m11)));

    auto transformXM = XMLoadFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&transform));
    XMVECTOR scaleXM, rotationXM, translationXM;
    XMMatrixDecompose(&scaleXM, &rotationXM, &translationXM, transformXM);

    XMFLOAT4 rotation;
    XMFLOAT3 translation;
    XMStoreFloat4(&rotation, rotationXM);
    XMStoreFloat3(&translation, translationXM);

    position[0] = translation.x;
    position[1] = translation.y;
    position[2] = translation.z;

    orientation[0] = rotation.x;
    orientation[1] = rotation.y;
    orientation[2] = rotation.z;
    orientation[3] = rotation.w;

    return S_OK;
}

void SpatialAnchors::initialize(JsValueRef storeRef)
{
    EXIT_IF_JS_ERROR(JsAddRef(storeRef, nullptr));

    m_host->runInBackground([this, storeRef]() -> long {
        try {
            auto requestOperation = SpatialAnchorManager::RequestStoreAsync();
            create_task(requestOperation).wait();
            m_store = requestOperation->GetResults();
        } catch (...) {
            m_host->runInScriptContext([this, storeRef]() {
                invokeEventListeners(L"error");
                JsRelease(storeRef, nullptr);
            });
            return E_FAIL;
        }

        m_host->runInScriptContext([this, storeRef]() {
            invokeEventListeners(L"initialized");
            JsRelease(storeRef, nullptr);
        });
        return S_OK;
    });
}

std::vector<HoloJs::ISpatialAnchor*> SpatialAnchors::enumerate()
{
    std::vector<HoloJs::ISpatialAnchor*> enumeratedAnchors;

    if (m_store == nullptr) {
        return enumeratedAnchors;
    }

    auto savedAnchors = m_store->GetAllSavedAnchors();

    auto iterator = savedAnchors->First();
    int index = 0;
    while (iterator->HasCurrent) {
        enumeratedAnchors.emplace_back(
            new HoloJsSpatialAnchor(iterator->Current->Value, iterator->Current->Key->Data()));

        iterator->MoveNext();
    }

    return enumeratedAnchors;
}

long SpatialAnchors::save(HoloJs::ISpatialAnchor* anchor, const wstring& anchorName)
{
    RETURN_IF_NULL(m_store);

    auto spatialAnchor = static_cast<HoloJsSpatialAnchor*>(anchor);
    RETURN_IF_NULL(spatialAnchor);

    auto savedAnchors = m_store->GetAllSavedAnchors();
    if (savedAnchors->HasKey(Platform::StringReference(anchorName.c_str()).GetString())) {
        m_store->Remove(Platform::StringReference(anchorName.c_str()).GetString());
    }

    RETURN_IF_FALSE(
        m_store->TrySave(Platform::StringReference(anchorName.c_str()), spatialAnchor->getPerceptionAnchor()));

    return S_OK;
}

long SpatialAnchors::open(const std::wstring& name, HoloJs::ISpatialAnchor** anchor)
{
    RETURN_IF_NULL(m_store);

    auto savedAnchors = m_store->GetAllSavedAnchors();

    if (savedAnchors->HasKey(Platform::StringReference(name.c_str()).GetString())) {
        auto perceptionAnchor = savedAnchors->Lookup(Platform::StringReference(name.c_str()).GetString());

        *anchor = new HoloJsSpatialAnchor(perceptionAnchor, name.c_str());
        return S_OK;
    } else {
        return E_FAIL;
    }
}

long SpatialAnchors::deleteAnchor(const std::wstring& name)
{
    RETURN_IF_NULL(m_store);

    auto savedAnchors = m_store->GetAllSavedAnchors();
    if (savedAnchors->HasKey(Platform::StringReference(name.c_str()).GetString())) {
        m_store->Remove(Platform::StringReference(name.c_str()).GetString());

        return S_OK;
    } else {
        return E_FAIL;
    }
}