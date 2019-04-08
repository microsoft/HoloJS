#pragma once

#include "include/holojs/private/chakra.h"
#include "include/holojs/private/holojs-view.h"
#include "include/holojs/private/holojs-script-host-internal.h"
#include "resource-management/external-object.h"

namespace HoloJs {

class HoloJsScriptHost;

namespace Interfaces {

class IXmlHttpRequest;
class XmlHttpRequestProjection {
   public:
    XmlHttpRequestProjection(std::shared_ptr<HoloJs::ResourceManagement::ResourceManager> resourceManager,
                             HoloJs::IHoloJsScriptHostInternal* host)
        : m_resourceManager(resourceManager), m_host(host)
    {
    }
    ~XmlHttpRequestProjection() {}

    virtual void Release() {}

    HRESULT initialize();

   private:
    JsValueRef m_createXmlHttpRequestFunction;
    static JsValueRef CHAKRA_CALLBACK staticCreateXmlHttpRequest(_In_ JsValueRef callee,
                                                                 _In_ bool isConstructCall,
                                                                 _In_ JsValueRef* arguments,
                                                                 _In_ unsigned short argumentCount,
                                                                 _In_ PVOID callbackData)
    {
        return reinterpret_cast<XmlHttpRequestProjection*>(callbackData)
            ->createXmlHttpRequest(arguments, argumentCount);
    }
    JsValueRef createXmlHttpRequest(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_sendXmlHttpRequestFunction;
    static JsValueRef CHAKRA_CALLBACK staticSendXmlHttpRequest(_In_ JsValueRef callee,
                                                               _In_ bool isConstructCall,
                                                               _In_ JsValueRef* arguments,
                                                               _In_ unsigned short argumentCount,
                                                               _In_ PVOID callbackData)
    {
        return reinterpret_cast<XmlHttpRequestProjection*>(callbackData)->sendXmlHttpRequest(arguments, argumentCount);
    }
    JsValueRef sendXmlHttpRequest(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_setRequestHeaderFunction;
    static JsValueRef CHAKRA_CALLBACK staticSetRequestHeader(_In_ JsValueRef callee,
                                                             _In_ bool isConstructCall,
                                                             _In_ JsValueRef* arguments,
                                                             _In_ unsigned short argumentCount,
                                                             _In_ PVOID callbackData)
    {
        return reinterpret_cast<XmlHttpRequestProjection*>(callbackData)->setRequestHeader(arguments, argumentCount);
    }
    JsValueRef setRequestHeader(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_getResponseHeaderFunction;
    static JsValueRef CHAKRA_CALLBACK staticGetResponseHeader(_In_ JsValueRef callee,
                                                              _In_ bool isConstructCall,
                                                              _In_ JsValueRef* arguments,
                                                              _In_ unsigned short argumentCount,
                                                              _In_ PVOID callbackData)
    {
        return reinterpret_cast<XmlHttpRequestProjection*>(callbackData)->getResponseHeader(arguments, argumentCount);
    }
    JsValueRef getResponseHeader(JsValueRef* arguments, unsigned short argumentCount);

    std::shared_ptr<HoloJs::ResourceManagement::ResourceManager> m_resourceManager;
    HoloJs::IHoloJsScriptHostInternal* m_host;
};

}  // namespace Interfaces
}  // namespace HoloJs