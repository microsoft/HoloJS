#include "stdafx.h"
#include "win32-base-view.h"
#include "win32-spatial-input.h"
#include "win32-timers-implementation.h"
#include <commctrl.h>
#include <d3d11_1.h>
#include <directxcolors.h>
#include <memory>

using namespace HoloJs::Win32;
using namespace HoloJs::AppModel;
using namespace std;
using namespace concurrency;

long long QueuedAppStartWorkItem::QueuedAppStartWorkItemId = 1000;

void getKeyAndCodeFromVirtualKey(int vKeyCode, wstring& key, wstring& code)
{
    if ((vKeyCode >= 0x41 && vKeyCode <= 0x5A) || (vKeyCode >= 0x30 && vKeyCode <= 0x39)) {
        key = static_cast<char>(vKeyCode);
        code = key;
    }
}

void Win32HoloJsBaseView::onKeyDown(WPARAM wParam, LPARAM lParam)
{
    if (m_windowElement == nullptr) {
        return;
    }

    wstring key;
    wstring code;
    getKeyAndCodeFromVirtualKey(static_cast<int>(wParam), key, code);

    m_windowElement->keyEvent(HoloJs::KeyEventType::KeyDown, key, code, 0, static_cast<int>(wParam));
}

void Win32HoloJsBaseView::onKeyUp(WPARAM wParam, LPARAM lParam)
{
    if (m_windowElement == nullptr) {
        return;
    }

    wstring key;
    wstring code;
    getKeyAndCodeFromVirtualKey(static_cast<int>(wParam), key, code);

    m_windowElement->keyEvent(HoloJs::KeyEventType::KeyUp, key, code, 0, static_cast<int>(wParam));
}

void Win32HoloJsBaseView::onMouseEvent(HoloJs::MouseButtonEventType eventType, WPARAM wParam, LPARAM lParam)
{
    if (m_windowElement == nullptr) {
        return;
    }

    auto xPos = LOWORD(lParam);
    auto yPos = HIWORD(lParam);

    unsigned int buttons = 0;
    if (wParam & MK_LBUTTON) {
        buttons |= static_cast<int>(HoloJs::MouseButton::Main);
    } else if (wParam & MK_MBUTTON) {
        buttons |= static_cast<int>(HoloJs::MouseButton::Auxiliary);
    } else if (wParam & MK_RBUTTON) {
        buttons |= static_cast<int>(HoloJs::MouseButton::Second);
    }

    m_windowElement->mouseEvent(eventType, xPos, yPos, 0, buttons);
}

void Win32HoloJsBaseView::onMouseButtonEvent(HoloJs::MouseButtonEventType eventType,
                                             WPARAM wParam,
                                             LPARAM lParam,
                                             HoloJs::MouseButton button)
{
    if (m_windowElement == nullptr) {
        return;
    }

    auto xPos = LOWORD(lParam);
    auto yPos = HIWORD(lParam);

    unsigned int buttons = 0;
    if (wParam & MK_LBUTTON) {
        buttons |= static_cast<int>(HoloJs::MouseButton::Main);
    } else if (wParam & MK_MBUTTON) {
        buttons |= static_cast<int>(HoloJs::MouseButton::Auxiliary);
    } else if (wParam & MK_RBUTTON) {
        buttons |= static_cast<int>(HoloJs::MouseButton::Second);
    }

    m_windowElement->mouseEvent(eventType, xPos, yPos, static_cast<int>(button), 0);
}

void Win32HoloJsBaseView::onMouseWheelEvent(HoloJs::MouseButtonEventType eventType, WPARAM wParam, LPARAM lParam)
{
    if (m_windowElement == nullptr) {
        return;
    }

    auto xPos = LOWORD(lParam);
    auto yPos = HIWORD(lParam);

    auto delta = static_cast<short>(static_cast<unsigned long long>(wParam & 0x00000000ffff0000) >> 16);

    auto lowOrderButtons = LOWORD(wParam);

    unsigned int buttons = 0;
    if (lowOrderButtons & MK_LBUTTON) {
        buttons |= static_cast<int>(HoloJs::MouseButton::Main);
    } else if (lowOrderButtons & MK_MBUTTON) {
        buttons |= static_cast<int>(HoloJs::MouseButton::Auxiliary);
    } else if (lowOrderButtons & MK_RBUTTON) {
        buttons |= static_cast<int>(HoloJs::MouseButton::Second);
    }

    m_windowElement->mouseEvent(eventType, xPos, yPos, delta, buttons);
}

long Win32HoloJsBaseView::queueForegroundWorkItem(HoloJs::IForegroundWorkItem* workItem)
{
    std::lock_guard<std::recursive_mutex> guard(m_foregroundWorkItemQueue);
    shared_ptr<HoloJs::IForegroundWorkItem> trackedWorkItem(workItem);

    RETURN_IF_TRUE(m_closeRequested);

    m_foregroundWorkItems.push(trackedWorkItem);

    return S_OK;
}

void Win32HoloJsBaseView::executeOneForegroundWorkItem()
{
    if (m_foregroundWorkItems.size() > 0) {
        std::lock_guard<std::recursive_mutex> guard(m_foregroundWorkItemQueue);
        auto workItem = m_foregroundWorkItems.front();
        m_foregroundWorkItems.pop();

        if (workItem->getTag() == QueuedAppStartWorkItem::QueuedAppStartWorkItemId) {
            auto app = dynamic_cast<QueuedAppStartWorkItem*>(workItem.get())->getApp();
            runApp(app);
        } else {
            workItem->execute();
        }
    }
}