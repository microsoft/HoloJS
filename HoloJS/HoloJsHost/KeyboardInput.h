#pragma once
namespace HologramJS
{
	namespace Input
	{
		class KeyboardInput
		{
		public:
			KeyboardInput();

			void KeyUp(Windows::UI::Core::KeyEventArgs ^args);
			void KeyDown(Windows::UI::Core::KeyEventArgs ^args);

			void SetScriptCallback(JsValueRef scriptCallback) { m_scriptCallback = scriptCallback; }

		private:
			JsValueRef m_scriptCallback = JS_INVALID_REFERENCE;

			bool CreateScriptParametersListForKeyboard(
				const std::wstring& eventName,
				const std::wstring& actionType,
				Windows::UI::Core::KeyEventArgs^ args,
				std::vector<JsValueRef>& outParams
			);
		};

	}
}

