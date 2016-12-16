#pragma once

namespace HologramJS
{
	namespace Input
	{
		class MouseInput
		{
		public:
			MouseInput();
			~MouseInput();

			void MouseDown(Windows::UI::Core::PointerEventArgs^ args);
			void MouseUp(Windows::UI::Core::PointerEventArgs^ args);
			void MouseMove(Windows::UI::Core::PointerEventArgs^ args);
			void MouseWheel(Windows::UI::Core::PointerEventArgs^ args);

			void SetScriptCallback(JsValueRef scriptCallback) { m_scriptCallback = scriptCallback; }

		private:
			JsValueRef m_scriptCallback = JS_INVALID_REFERENCE;

			bool CreateScriptParametersListForMouse(
				const std::wstring& eventName,
				const std::wstring& actionType,
				Windows::UI::Core::PointerEventArgs^ args,
				std::vector<JsValueRef>& outParams
			);

		};
	}
}


