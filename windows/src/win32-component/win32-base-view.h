#pragma once

#include "holojs/holojs-script-host.h"
#include "holojs/private/holojs-view.h"
#include "holojs/private/script-host-errors.h"
#include "holojs/private/script-window.h"
#include "holojs/private/timers.h"
#include "win32-mixed-reality-context.h"
#include "win32-opengl-context.h"
#include "win32-timers-implementation.h"
#include <memory>
#include <wrl.h>

namespace HoloJs {
namespace Win32 {

class Win32HoloJsBaseView {
   public:
    Win32HoloJsBaseView() {}
    virtual ~Win32HoloJsBaseView() {}

    void setWindowElement(HoloJs::IWindow* windowElement) { m_windowElement = windowElement; }

    void onKeyDown(WPARAM wParam, LPARAM lParam);
    void onKeyUp(WPARAM wParam, LPARAM lParam);

    void onMouseEvent(HoloJs::MouseButtonEventType eventType, WPARAM wParam, LPARAM lParam);
    void onMouseButtonEvent(HoloJs::MouseButtonEventType eventType,
                            WPARAM wParam,
                            LPARAM lParam,
                            HoloJs::MouseButton button);
    void onMouseWheelEvent(HoloJs::MouseButtonEventType eventType, WPARAM wParam, LPARAM lParam);

   protected:
    HoloJs::IWindow* m_windowElement;
};
}  // namespace Win32
}  // namespace HoloJs
