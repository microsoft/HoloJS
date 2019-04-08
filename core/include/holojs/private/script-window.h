#pragma once

#include "chakra.h"
#include <string>
namespace HoloJs {

class IHoloJsScriptHost;

enum class KeyEventType { KeyDown, KeyUp };

enum class MouseButtonEventType { Down, Up, Move, Wheel, Click, DblClick };

enum class MouseButton {
    Main = 0,
    Auxiliary = 1,
    Second = 2
};

class IWindow {
   public:
    virtual long initialize() = 0;

    virtual long resize(int width, int height) = 0;
    virtual long vSync() = 0;
    virtual long deviceLost() = 0;
    virtual long deviceRestored() = 0;

    virtual long gamepadConnected(JsValueRef gamepadEvent) = 0;
    virtual long gamepadDisconnected(JsValueRef gamepadEvent) = 0;

    virtual long keyEvent(
        KeyEventType type, const std::wstring& key, const std::wstring& code, int location, int keyCode) = 0;
    virtual long mouseEvent(MouseButtonEventType type, int x, int y, int button, int buttons) = 0;

    virtual long setHeadsetAvailable(bool available) = 0;
};
}  // namespace HoloJs