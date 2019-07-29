#include "stdafx.h"
#include "corewindow-input.h"

using namespace Windows::UI::Input::Spatial;
using namespace std;
using namespace HoloJs::UWP;

using namespace Windows::ApplicationModel::Core;
using namespace Windows::UI::Core;

void getKeyAndCodeFromVirtualKey(Windows::System::VirtualKey vKeyCode, wstring& key, wstring& code)
{
    if ((vKeyCode >= Windows::System::VirtualKey::A && vKeyCode <= Windows::System::VirtualKey::Z) ||
        (vKeyCode >= Windows::System::VirtualKey::Number0 && vKeyCode <= Windows::System::VirtualKey::Number9)) {
        key = static_cast<char>(vKeyCode);
        code = key;
    }
}

unsigned int getMouseButtons(Windows::UI::Core::PointerEventArgs ^ args)
{
    unsigned int buttonsMask = 0;
    if (args->CurrentPoint->Properties->IsLeftButtonPressed) {
        buttonsMask |= static_cast<int>(HoloJs::MouseButton::Main);
    } else if (args->CurrentPoint->Properties->IsMiddleButtonPressed) {
        buttonsMask |= static_cast<int>(HoloJs::MouseButton::Auxiliary);
    } else if (args->CurrentPoint->Properties->IsRightButtonPressed) {
        buttonsMask |= static_cast<int>(HoloJs::MouseButton::Second);
    }

    return buttonsMask;
}

unsigned int getFirstMouseButtonPressed(Windows::UI::Core::PointerEventArgs ^ args)
{
    if (args->CurrentPoint->Properties->IsLeftButtonPressed) {
        return static_cast<int>(HoloJs::MouseButton::Main);
    } else if (args->CurrentPoint->Properties->IsMiddleButtonPressed) {
        return static_cast<int>(HoloJs::MouseButton::Auxiliary);
    } else if (args->CurrentPoint->Properties->IsRightButtonPressed) {
        return static_cast<int>(HoloJs::MouseButton::Second);
    }

    return 0;
}

HRESULT CoreWindowInput::registerEventHandlers()
{
    auto coreWindow = CoreApplication::MainView->CoreWindow;

    coreWindow->KeyUp += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^,
                                                                        Windows::UI::Core::KeyEventArgs ^>(
        this, &CoreWindowInput::OnKeyUp);

    coreWindow->KeyDown += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^,
                                                                          Windows::UI::Core::KeyEventArgs ^>(
        this, &CoreWindowInput::OnKeyDown);

    coreWindow->PointerWheelChanged +=
        ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^,
                                                       Windows::UI::Core::PointerEventArgs ^>(
            this, &CoreWindowInput::OnPointerWheelChanged);

    coreWindow->PointerPressed += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^,
                                                                                 Windows::UI::Core::PointerEventArgs ^>(
        this, &CoreWindowInput::OnPointerPressed);

    coreWindow->PointerReleased +=
        ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^,
                                                       Windows::UI::Core::PointerEventArgs ^>(
            this, &CoreWindowInput::OnPointerReleased);

    coreWindow->PointerMoved += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^,
                                                                               Windows::UI::Core::PointerEventArgs ^>(
        this, &CoreWindowInput::OnPointerMoved);

    return S_OK;
}

void CoreWindowInput::OnKeyUp(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::KeyEventArgs ^ args)
{
    if (m_windowElement != nullptr) {
        wstring key;
        wstring code;
        getKeyAndCodeFromVirtualKey(args->VirtualKey, key, code);
        m_windowElement->keyEvent(HoloJs::KeyEventType::KeyUp, key, code, 0, static_cast<int>(args->VirtualKey));
    }
}

void CoreWindowInput::OnKeyDown(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::KeyEventArgs ^ args)
{
    if (m_windowElement != nullptr) {
        wstring key;
        wstring code;
        getKeyAndCodeFromVirtualKey(args->VirtualKey, key, code);
        m_windowElement->keyEvent(HoloJs::KeyEventType::KeyDown, key, code, 0, static_cast<int>(args->VirtualKey));
    }
}

void CoreWindowInput::OnPointerWheelChanged(Windows::UI::Core::CoreWindow ^ sender,
                                            Windows::UI::Core::PointerEventArgs ^ args)
{
    if (m_windowElement != nullptr) {
        m_windowElement->mouseEvent(HoloJs::MouseButtonEventType::Wheel,
                                    static_cast<int>(args->CurrentPoint->Position.X),
                                    static_cast<int>(args->CurrentPoint->Position.Y),
                                    args->CurrentPoint->Properties->MouseWheelDelta,
                                    getMouseButtons(args));
    }
}

void CoreWindowInput::OnPointerPressed(Windows::UI::Core::CoreWindow ^ sender,
                                       Windows::UI::Core::PointerEventArgs ^ args)
{
    if (m_windowElement != nullptr) {
        m_windowElement->mouseEvent(HoloJs::MouseButtonEventType::Down,
                                    static_cast<int>(args->CurrentPoint->Position.X),
                                    static_cast<int>(args->CurrentPoint->Position.Y),
                                    getFirstMouseButtonPressed(args),
                                    getMouseButtons(args));
    }
}

void CoreWindowInput::OnPointerReleased(Windows::UI::Core::CoreWindow ^ sender,
                                        Windows::UI::Core::PointerEventArgs ^ args)
{
    if (m_windowElement != nullptr) {
        m_windowElement->mouseEvent(HoloJs::MouseButtonEventType::Up,
                                    static_cast<int>(args->CurrentPoint->Position.X),
                                    static_cast<int>(args->CurrentPoint->Position.Y),
                                    getFirstMouseButtonPressed(args),
                                    getMouseButtons(args));
    }
}

void CoreWindowInput::OnPointerMoved(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::PointerEventArgs ^ args)
{
    if (m_windowElement != nullptr) {
        m_windowElement->mouseEvent(HoloJs::MouseButtonEventType::Move,
                                    static_cast<int>(args->CurrentPoint->Position.X),
                                    static_cast<int>(args->CurrentPoint->Position.Y),
                                    getFirstMouseButtonPressed(args),
                                    getMouseButtons(args));
    }
}
