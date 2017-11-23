#pragma once
#include "ChakraForHoloJS.h"
#include "InputInterface.h"

namespace HologramJS {
namespace Input {
class KeyboardInput {
   public:
    KeyboardInput();
    ~KeyboardInput();

    void SetScriptCallback(JsValueRef scriptCallback) { m_scriptCallback = scriptCallback; }

    bool AddEventListener(const std::wstring& type);

    bool RemoveEventListener(const std::wstring& type);

   private:
    JsValueRef m_scriptCallback = JS_INVALID_REFERENCE;

    void CallbackScriptForKeyboardInput(KeyboardInputEventType type, Windows::UI::Core::KeyEventArgs ^ args);

    Windows::Foundation::EventRegistrationToken m_keyDownToken;
    Windows::Foundation::EventRegistrationToken m_keyUpToken;

    unsigned int m_inputRefCount = 0;
};

}  // namespace Input
}  // namespace HologramJS
