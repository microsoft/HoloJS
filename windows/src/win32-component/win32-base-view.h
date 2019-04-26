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
#include <queue>
#include <wrl.h>

namespace HoloJs {
namespace Win32 {

class QueuedAppStartWorkItem : public HoloJs::IForegroundWorkItem {
   public:
    QueuedAppStartWorkItem(std::shared_ptr<HoloJs::AppModel::HoloJsApp> app) : m_app(app) {}

    virtual ~QueuedAppStartWorkItem() {}

    virtual void execute() {}

    std::shared_ptr<HoloJs::AppModel::HoloJsApp> getApp() { return m_app; }

    virtual long long getTag() { return 1000; }

    static long long QueuedAppStartWorkItemId;

   private:
    std::shared_ptr<HoloJs::AppModel::HoloJsApp> m_app;
};

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

    virtual void runApp(std::shared_ptr<HoloJs::AppModel::HoloJsApp> appDefinition) = 0;

   protected:
    HoloJs::IWindow* m_windowElement;

    std::recursive_mutex m_foregroundWorkItemQueue;
    std::queue<std::shared_ptr<HoloJs::IForegroundWorkItem>> m_foregroundWorkItems;

    long queueForegroundWorkItem(HoloJs::IForegroundWorkItem* workItem);
    void executeOneForegroundWorkItem();

    bool m_closeRequested = false;
};
}  // namespace Win32
}  // namespace HoloJs
