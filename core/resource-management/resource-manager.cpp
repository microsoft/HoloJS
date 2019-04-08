#pragma once
#include "stdafx.h"
#include "resource-manager.h"

using namespace HoloJs::ResourceManagement;

void CHAKRA_CALLBACK ResourceManager::jsFinalize(void* data)
{
    ExternalObject* iRelease = reinterpret_cast<ExternalObject*>(data);
    if (iRelease == nullptr) {
        return;
    }

    iRelease->getResourceTracker()->releaseResource(iRelease);
}

void ResourceManager::releaseResource(ExternalObject* resource)
{
    std::lock_guard<std::mutex> guard(m_resourcesListLock);

    for (auto& object : m_projectedResources) {
        if (object == resource) {
            delete resource;
            m_projectedResources.remove(object);
            break;
        }
    }
}

void ResourceManager::releaseAll()
{
    std::lock_guard<std::mutex> guard(m_resourcesListLock);

    for (auto& object : m_projectedResources) {
        if (object != nullptr) {
            delete object;
        }
    }

    m_projectedResources.clear();
}

JsErrorCode ResourceManager::createAndTrackExternalBuffer(size_t bufferLength,
                                                          void** allocatedBuffer,
                                                          JsValueRef* scriptBuffer)
{
    if (bufferLength > UINT32_MAX) {
        return JsErrorFatal;
    }

    *allocatedBuffer = new unsigned char[bufferLength];
    return JsCreateExternalArrayBuffer(
        *allocatedBuffer, static_cast<unsigned int>(bufferLength), jsFinalizeArray, *allocatedBuffer, scriptBuffer);
}

void ResourceManager::trackObject(ExternalObject* object)
{
    std::lock_guard<std::mutex> guard(m_resourcesListLock);
    m_projectedResources.push_back(object);
}

JsValueRef ResourceManager::objectToDirectExternal(ExternalObject* object)
{
    JsValueRef externalObjectRef;
    RETURN_INVALID_REF_IF_JS_ERROR(JsCreateExternalObject(object, jsFinalize, &externalObjectRef));
    trackObject(object);

    return externalObjectRef;
}

JsValueRef ResourceManager::objectToExternal(ExternalObject* object)
{
    JsValueRef externalObject;
    RETURN_INVALID_REF_IF_JS_ERROR(JsCreateExternalObject(object, jsFinalize, &externalObject));

    JsValueRef wrapperObject;
    RETURN_INVALID_REF_IF_JS_ERROR(JsCreateObject(&wrapperObject));

    JsPropertyIdRef externalPropertyId;
    RETURN_INVALID_REF_IF_JS_ERROR(JsGetPropertyIdFromName(L"external", &externalPropertyId));

    RETURN_INVALID_REF_IF_JS_ERROR(JsSetProperty(wrapperObject, externalPropertyId, externalObject, true));

    trackObject(object);

    return wrapperObject;
}

HoloJs::EventTarget* ResourceManager::externalToEventsInterface(JsValueRef value)
{
    bool hasExternal;
    RETURN_NULL_IF_JS_ERROR(JsHasExternalData(value, &hasExternal));
    RETURN_NULL_IF_FALSE(hasExternal);

    ExternalObject* externalObject;
    RETURN_NULL_IF_JS_ERROR(JsGetExternalData(value, (void**)&externalObject));

    return externalObject->getEventTarget();
}