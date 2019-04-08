#pragma once

#include "holojs-script-host.h"
#include "include/holojs/private/chakra.h"
#include "include/holojs/private/event-target.h"
#include "include/holojs/private/object-lifetime.h"
#include "include/holojs/private/script-window.h"

namespace HoloJs {

class HoloJsScriptHostInternal;

namespace Interfaces {

enum class DeviceContextEventType : int { Lost, Restored };

class WindowElement : public HoloJs::IWindow, public HoloJs::ResourceManagement::IRelease, public HoloJs::EventTarget {
   public:
    WindowElement(HoloJs::IHoloJsScriptHostInternal* host) : m_host(host) {}
    ~WindowElement() { cleanup(); }

    virtual void Release() { cleanup(); }

    virtual long initialize();

    virtual long resize(int width, int height);
    virtual long vSync();

    virtual long deviceLost() { return invokeEventListeners(L"webglcontextlost"); }

    virtual long deviceRestored() { return invokeEventListeners(L"webglcontextrestored"); }

    virtual long gamepadConnected(JsValueRef gamepadEvent)
    {
        return invokeEventListeners(L"gamepadconnected", gamepadEvent);
    }

    virtual long gamepadDisconnected(JsValueRef gamepadEvent)
    {
        return invokeEventListeners(L"gamepaddisconnected", gamepadEvent);
    }

    virtual long keyEvent(
        HoloJs::KeyEventType type, const std::wstring& key, const std::wstring& code, int location, int keyCode);
    virtual long mouseEvent(MouseButtonEventType type, int x, int y, int button, int buttons);

    virtual long setHeadsetAvailable(bool available);

   private:
    int m_width;
    int m_height;

    IHoloJsScriptHostInternal* m_host;

    void cleanup();

    JsValueRef m_windowExternalObjectRef;

    JsValueRef m_windowRef = JS_INVALID_REFERENCE;
    JsValueRef m_vsyncCallback = JS_INVALID_REFERENCE;

    JsValueRef m_nativeInterfaceRef = JS_INVALID_REFERENCE;

    JS_PROJECTION_DEFINE(WindowElement, register)
    JS_PROJECTION_DEFINE(WindowElement, requestPresent)
    JS_PROJECTION_DEFINE(WindowElement, btoa)
    JS_PROJECTION_DEFINE(WindowElement, atob)

    // No-op preventDefault on mouse events
    JsValueRef m_noopFunction = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK noopStatic(JsValueRef callee,
                                                 bool isConstructCall,
                                                 JsValueRef* arguments,
                                                 unsigned short argumentCount,
                                                 PVOID callbackData)
    {
        return JS_INVALID_REFERENCE;
    }

    bool m_isRegistered = false;
    bool m_headsetPresent = false;

    JsValueRef m_presentingVrDisplayRef = JS_INVALID_REFERENCE;

    std::map<HoloJs::MouseButtonEventType, JsValueRef> m_mouseToEventNameRefMap;
};
}  // namespace Interfaces
}  // namespace HoloJs