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

			void SetScriptCallback(JsValueRef scriptCallback) { m_scriptCallback = scriptCallback; }

		private:
			JsValueRef m_scriptCallback = JS_INVALID_REFERENCE;

			bool CreateScriptParametersListForMouse(
				const std::wstring& eventName,
				const std::wstring& actionType,
				Windows::UI::Core::PointerEventArgs^ args,
				JsValueRef (&outParams)[6]
			);


			void MouseDown(Windows::UI::Core::PointerEventArgs^ args);
			void MouseUp(Windows::UI::Core::PointerEventArgs^ args);
			void MouseMove(Windows::UI::Core::PointerEventArgs^ args);
			void MouseWheel(Windows::UI::Core::PointerEventArgs^ args);

			Windows::Foundation::EventRegistrationToken m_mouseDownToken;
			Windows::Foundation::EventRegistrationToken m_mouseUpToken;
			Windows::Foundation::EventRegistrationToken m_mouseWheelToken;
			Windows::Foundation::EventRegistrationToken m_mouseMoveToken;
		};
	}
}


