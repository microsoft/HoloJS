#pragma once

#include "include/holojs/private/chakra.h"
#include "include/holojs/private/holojs-view.h"
#include "include/holojs/private/holojs-script-host-internal.h"
#include "resource-management/external-object.h"

namespace HoloJs {

class HoloJsScriptHost;

namespace Interfaces {

class IImage;
class ImageProjections {
   public:
    ImageProjections(std::shared_ptr<HoloJs::ResourceManagement::ResourceManager> resourceManager,
                     HoloJs::IHoloJsScriptHostInternal* host)
        : m_resourceManager(resourceManager), m_host(host)
    {
    }
    ~ImageProjections() {}

    virtual void Release() {}

    long initialize();

   private:
    JsValueRef m_createImageFunction;
    static JsValueRef CHAKRA_CALLBACK staticCreateImage(_In_ JsValueRef callee,
                                                                 _In_ bool isConstructCall,
                                                                 _In_ JsValueRef* arguments,
                                                                 _In_ unsigned short argumentCount,
                                                                 _In_ PVOID callbackData)
    {
        return reinterpret_cast<ImageProjections*>(callbackData)
            ->createImage(arguments, argumentCount);
    }
    JsValueRef createImage(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_setSourceFunction;
    static JsValueRef CHAKRA_CALLBACK staticSetSource(_In_ JsValueRef callee,
                                                               _In_ bool isConstructCall,
                                                               _In_ JsValueRef* arguments,
                                                               _In_ unsigned short argumentCount,
                                                               _In_ PVOID callbackData)
    {
        return reinterpret_cast<ImageProjections*>(callbackData)->setSource(arguments, argumentCount);
    }
    JsValueRef setSource(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_setSourceFromDataUrlFunction;
    static JsValueRef CHAKRA_CALLBACK staticSetSourceFromDataUrl(_In_ JsValueRef callee,
                                                             _In_ bool isConstructCall,
                                                             _In_ JsValueRef* arguments,
                                                             _In_ unsigned short argumentCount,
                                                             _In_ PVOID callbackData)
    {
        return reinterpret_cast<ImageProjections*>(callbackData)
            ->setSourceFromDataUrl(arguments, argumentCount);
    }
    JsValueRef setSourceFromDataUrl(JsValueRef* arguments, unsigned short argumentCount);

    std::shared_ptr<HoloJs::ResourceManagement::ResourceManager> m_resourceManager;
    HoloJs::IHoloJsScriptHostInternal* m_host;
};

}  // namespace Interfaces
}  // namespace HoloJs