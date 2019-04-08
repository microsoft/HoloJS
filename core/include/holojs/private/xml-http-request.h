#pragma once

#include "event-target.h"
#include "holojs-view.h"
#include "object-lifetime.h"

namespace HoloJs {

class IXmlHttpRequest : public HoloJs::EventTarget, public HoloJs::ResourceManagement::IRelease {
   public:

    virtual ~IXmlHttpRequest() {}

    virtual long createHttpContent(JsValueRef payload) = 0;

    virtual long send(const std::wstring& method,
                      const std::wstring& uri,
                      const std::wstring& type,
                      JsValueRef xhrRef) = 0;

    virtual void setRequestHeader(const std::wstring& name, const std::wstring& value) = 0;

    virtual JsValueRef getResponseHeader(const std::wstring& name) = 0;
};

}  // namespace HoloJs