#pragma once

#include "../dom-events.h"
#include "../include/holojs/private/object-lifetime.h"

namespace HoloJs {

namespace ResourceManagement {

class ResourceManager;

class ExternalObject {
   public:
    ExternalObject(ResourceManager* resourceTracker) : m_resourceTracker(resourceTracker) {}

    template <typename T>
    HRESULT initialize(T* object, ObjectType type, ObjectType baseType = ObjectType::None)
    {
        IRelease* releaseInterface = dynamic_cast<IRelease*>(object);

        RETURN_IF_NULL(releaseInterface);

        m_object = object;
        m_eventTarget = dynamic_cast<HoloJs::EventTarget*>(object);
        m_releaseInterface = releaseInterface;
        m_type = type;
        m_baseType = baseType;

        return S_OK;
    }

    virtual ~ExternalObject()
    {
        if (m_releaseInterface) {
            delete m_releaseInterface;
            m_object = nullptr;
            m_releaseInterface = nullptr;
            m_eventTarget = nullptr;
        }
    }

    void* getObject() { return m_object; }
    HoloJs::EventTarget* getEventTarget() { return m_eventTarget; }

    void setResourceTracker(ResourceManager* resourceTracker) { m_resourceTracker = resourceTracker; }

    ResourceManager* getResourceTracker() { return m_resourceTracker; }

    ObjectType getType() const { return m_type; }
    ObjectType getBaseType() const { return m_baseType; }
    bool hasBaseType() const { return (m_baseType != ObjectType::None); }

   private:
    void* m_object = nullptr;
    HoloJs::EventTarget* m_eventTarget = nullptr;
    IRelease* m_releaseInterface = nullptr;
    ObjectType m_type;
    ObjectType m_baseType;

    ResourceManager* m_resourceTracker;
};

}  // namespace ResourceManagement
}  // namespace HoloJs
