#pragma once

#include "holojs/private/script-window.h"

namespace HoloJs {
namespace UWP {
public
ref class CoreWindowInput sealed {
   public:
    CoreWindowInput() {}
    virtual ~CoreWindowInput() {}

    internal : HRESULT registerEventHandlers();

    void setWindowElement(HoloJs::IWindow* windowElement)
    {
        m_windowElement = windowElement;
    }

   private:
    HoloJs::IWindow* m_windowElement;

    void OnKeyUp(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::KeyEventArgs ^ args);
    void OnKeyDown(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::KeyEventArgs ^ args);
    void OnPointerWheelChanged(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::PointerEventArgs ^ args);
    void OnPointerPressed(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::PointerEventArgs ^ args);
    void OnPointerReleased(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::PointerEventArgs ^ args);
    void OnPointerMoved(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::PointerEventArgs ^ args);
};
}  // namespace UWP
}  // namespace HoloJs