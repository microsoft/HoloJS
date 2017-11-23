#include "pch.h"
#include "KeyboardInput.h"
#include "ScriptErrorHandling.h"

PCWSTR g_supportedKeyboardEvents[] = {L"keydown", L"keyup"};

using namespace HologramJS::Input;
using namespace Windows::UI::Core;
using namespace std;
using namespace Windows::Foundation;

KeyboardInput::KeyboardInput() {}

KeyboardInput::~KeyboardInput()
{
    CoreWindow::GetForCurrentThread()->KeyDown -= m_keyDownToken;
    CoreWindow::GetForCurrentThread()->KeyUp -= m_keyUpToken;
}

bool KeyboardInput::AddEventListener(const wstring& type)
{
    for (int i = 0; i < ARRAYSIZE(g_supportedKeyboardEvents); i++) {
        if (type == g_supportedKeyboardEvents[i]) {
            m_inputRefCount++;

            if (m_inputRefCount == 1) {
                m_keyDownToken = CoreWindow::GetForCurrentThread()->KeyDown +=
                    ref new TypedEventHandler<CoreWindow ^, KeyEventArgs ^>(
                        [this](CoreWindow ^ sender, KeyEventArgs ^ args) {
                            CallbackScriptForKeyboardInput(KeyboardInputEventType::KeyDown, args);
                        });

                m_keyUpToken = CoreWindow::GetForCurrentThread()->KeyUp +=
                    ref new TypedEventHandler<CoreWindow ^, KeyEventArgs ^>(
                        [this](CoreWindow ^ sender, KeyEventArgs ^ args) {
                            CallbackScriptForKeyboardInput(KeyboardInputEventType::KeyUp, args);
                        });
            }

            return true;
        }
    }

    return false;
}

bool KeyboardInput::RemoveEventListener(const wstring& type)
{
    for (int i = 0; i < ARRAYSIZE(g_supportedKeyboardEvents); i++) {
        if (type == g_supportedKeyboardEvents[i]) {
            m_inputRefCount--;

            if (m_inputRefCount == 0) {
                CoreWindow::GetForCurrentThread()->KeyDown -= m_keyDownToken;
                CoreWindow::GetForCurrentThread()->KeyUp -= m_keyUpToken;
            }

            return true;
        }
    }

    return false;
}

void KeyboardInput::CallbackScriptForKeyboardInput(KeyboardInputEventType type, KeyEventArgs ^ args)
{
    EXIT_IF_TRUE(m_scriptCallback == JS_INVALID_REFERENCE);

    JsValueRef parameters[4];
    parameters[0] = m_scriptCallback;
    JsValueRef* eventTypeParam = &parameters[1];
    JsValueRef* keyParam = &parameters[2];
    JsValueRef* actionTypeParam = &parameters[3];

    EXIT_IF_JS_ERROR(JsIntToNumber(static_cast<int>(NativeToScriptInputType::Keyboard), eventTypeParam));
    EXIT_IF_JS_ERROR(JsIntToNumber((int)args->VirtualKey, keyParam));
    EXIT_IF_JS_ERROR(JsIntToNumber(static_cast<int>(type), actionTypeParam));

    JsValueRef result;
    HANDLE_EXCEPTION_IF_JS_ERROR(JsCallFunction(m_scriptCallback, parameters, ARRAYSIZE(parameters), &result));
}
