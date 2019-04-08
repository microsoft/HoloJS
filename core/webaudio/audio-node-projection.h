#pragma once

#include "../include/holojs/private/chakra.h"
#include "../resource-management/resource-manager.h"
#include <memory>

namespace HoloJs {

namespace WebAudio {

class AudioNodeProjection {
   public:
    AudioNodeProjection(std::shared_ptr<HoloJs::ResourceManagement::ResourceManager> resourceManager)
        : m_resourceManager(resourceManager)
    {
    }

    HRESULT initialize();

   private:
    std::shared_ptr<HoloJs::ResourceManagement::ResourceManager> m_resourceManager;

    JsValueRef m_connectFunction = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK connectStatic(JsValueRef callee,
                                                    bool isConstructCall,
                                                    JsValueRef* arguments,
                                                    unsigned short argumentCount,
                                                    PVOID callbackData)
    {
        return reinterpret_cast<AudioNodeProjection*>(callbackData)->connect(arguments, argumentCount);
    }
	JsValueRef connect(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_disconnectFunction = JS_INVALID_REFERENCE;
	static JsValueRef CHAKRA_CALLBACK disconnectStatic(JsValueRef callee,
		bool isConstructCall,
		JsValueRef* arguments,
		unsigned short argumentCount,
		PVOID callbackData)
	{
		return reinterpret_cast<AudioNodeProjection*>(callbackData)->disconnect(arguments, argumentCount);
	}
	JsValueRef disconnect(JsValueRef* arguments, unsigned short argumentCount);
};
}  // namespace WebAudio
}  // namespace HoloJs
