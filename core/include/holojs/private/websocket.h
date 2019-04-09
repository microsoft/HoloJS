#pragma once

#include "event-target.h"
#include "holojs-view.h"
#include "object-lifetime.h"
#include "blob-interface.h"
#include <string>

namespace HoloJs {

class IWebSocket : public HoloJs::ResourceManagement::IRelease {
   public:

    virtual ~IWebSocket() {}
	virtual int getReadyState() = 0;

	virtual void connect(const std::wstring& url, const std::vector<std::wstring>& protocols) = 0;

	virtual void send(const std::wstring& message) = 0;
	virtual void send(unsigned char* data, unsigned int dataSize, JsValueRef dataRef) = 0;
	virtual void send(HoloJs::IBlob* blob, JsValueRef dataRef) = 0;

	virtual void close(int code, const std::wstring& reason) = 0;
};

}  // namespace HoloJs