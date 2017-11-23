#pragma once
#include "ChakraForHoloJS.h"
#include "InputInterface.h"

namespace HologramJS {
namespace Input {
class MouseInput {
   public:
    MouseInput();
    ~MouseInput();

    void SetScriptCallback(JsValueRef scriptCallback) { m_scriptCallback = scriptCallback; }

    bool AddEventListener(const std::wstring& type);

    bool RemoveEventListener(const std::wstring& type);

   private:
    JsValueRef m_scriptCallback = JS_INVALID_REFERENCE;

    void CallbackScriptForMouseInput(MouseInputEventType type, Windows::UI::Core::PointerEventArgs ^ args);

    Windows::Foundation::EventRegistrationToken m_mouseDownToken;
    Windows::Foundation::EventRegistrationToken m_mouseUpToken;
    Windows::Foundation::EventRegistrationToken m_mouseWheelToken;
    Windows::Foundation::EventRegistrationToken m_mouseMoveToken;

    unsigned int m_inputRefCount = 0;
};
}  // namespace Input
}  // namespace HologramJS
