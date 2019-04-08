#pragma once

#include "../include/holojs/private/chakra.h"
#include "../include/holojs/private/error-handling.h"
#include "external-object.h"
#include <list>
#include <mutex>

namespace HoloJs {

namespace Interfaces {
class ElementWithEvents;
}
namespace ResourceManagement {
class ResourceManager {
   public:
    ~ResourceManager() { releaseAll(); }

    static JsErrorCode createAndTrackExternalBuffer(size_t bufferLength,
                                                    void** allocatedBuffer,
                                                    JsValueRef* scriptBuffer);

    void trackObject(ExternalObject* object);
    JsValueRef objectToDirectExternal(ExternalObject* object);
    JsValueRef objectToExternal(ExternalObject* object);

    static HoloJs::EventTarget* externalToEventsInterface(JsValueRef value);

    template <typename T>
    JsValueRef objectToDirectExternal(T* object, ObjectType type, ObjectType baseType = ObjectType::None)
    {
        if (object != nullptr) {
            HoloJs::ResourceManagement::ExternalObject* externalObject =
                new HoloJs::ResourceManagement::ExternalObject(this);
            RETURN_INVALID_REF_IF_FAILED(externalObject->initialize(object, type, baseType));
            return ResourceManager::objectToDirectExternal(externalObject);
        } else {
            return JS_INVALID_REFERENCE;
        }
    }

    template <typename T>
    JsValueRef objectToExternal(T* object, ObjectType type, ObjectType baseType = ObjectType::None)
    {
        if (object == nullptr) {
            return JS_INVALID_REFERENCE;
        } else {
            HoloJs::ResourceManagement::ExternalObject* externalObject =
                new HoloJs::ResourceManagement::ExternalObject(this);
            RETURN_INVALID_REF_IF_FAILED(externalObject->initialize(object, type, baseType));
            return ResourceManager::objectToExternal(externalObject);
        }
    }

    template <typename T>
    T* externalToObject(JsValueRef value, ObjectType type)
    {
        bool hasExternal;
        RETURN_NULL_IF_JS_ERROR(JsHasExternalData(value, &hasExternal));

        if (hasExternal) {
            HoloJs::ResourceManagement::ExternalObject* externalObject;
            RETURN_NULL_IF_JS_ERROR(JsGetExternalData(value, (void**)&externalObject));

            RETURN_NULL_IF_FALSE(externalObject->getType() == type ||
                                 (externalObject->hasBaseType() && externalObject->getBaseType() == type));
            return reinterpret_cast<T*>(externalObject->getObject());
        } else {
            return nullptr;
        }
    }

   private:
    std::list<HoloJs::ResourceManagement::ExternalObject*> m_projectedResources;
    std::mutex m_resourcesListLock;

    void releaseResource(HoloJs::ResourceManagement::ExternalObject* resource);
    void releaseAll();

    static void CHAKRA_CALLBACK jsFinalize(_In_opt_ void* data);
    static void CHAKRA_CALLBACK jsFinalizeArray(_In_opt_ void* data)
    {
        if (data != nullptr) {
            delete[] data;
        }
    }
};

}  // namespace ResourceManagement
}  // namespace HoloJs