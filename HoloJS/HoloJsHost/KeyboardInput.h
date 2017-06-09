#pragma once
#include "InputInterface.h"
namespace HologramJS
{
	namespace Input
	{
		class KeyboardInput
		{
		public:
			KeyboardInput();
			~KeyboardInput();

			void SetScriptCallback(JsValueRef scriptCallback) { m_scriptCallback = scriptCallback; }

		private:
			JsValueRef m_scriptCallback = JS_INVALID_REFERENCE;

			void CallbackScriptForKeyboardInput(
				KeyboardInputEventType type,
				Windows::UI::Core::KeyEventArgs^ args
			);

			Windows::Foundation::EventRegistrationToken m_keyDownToken;
			Windows::Foundation::EventRegistrationToken m_keyUpToken;
		};

	}
}

