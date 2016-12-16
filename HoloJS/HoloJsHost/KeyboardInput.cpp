#include "pch.h"
#include "KeyboardInput.h"

using namespace HologramJS::Input;
using namespace Windows::UI::Core;
using namespace std;


KeyboardInput::KeyboardInput()
{
}

bool
KeyboardInput::CreateScriptParametersListForKeyboard(
	const wstring& eventName,
	const wstring& actionType,
	KeyEventArgs^ args,
	vector<JsValueRef>& outParams
)
{
	outParams.resize(4);

	outParams[0] = m_scriptCallback;
	JsValueRef* eventNameParam = &outParams[1];
	JsValueRef* keyParam = &outParams[2];
	JsValueRef* actionParam = &outParams[3];

	RETURN_IF_JS_ERROR(JsPointerToString(eventName.c_str(), eventName.length(), eventNameParam));
	RETURN_IF_JS_ERROR(JsIntToNumber((int)args->VirtualKey, keyParam));
	RETURN_IF_JS_ERROR(JsPointerToString(actionType.c_str(), actionType.length(), actionParam));

	return true;
}


void
KeyboardInput::KeyUp(KeyEventArgs ^args)
{
	static std::wstring keyboardEventName(L"keyboard");
	static std::wstring keyActionType(L"up");

	if (m_scriptCallback != JS_INVALID_REFERENCE)
	{
		std::vector<JsValueRef> parameters;
		EXIT_IF_FALSE(CreateScriptParametersListForKeyboard(keyboardEventName, keyActionType, args, parameters));

		JsValueRef result;
		EXIT_IF_JS_ERROR(JsCallFunction(m_scriptCallback, parameters.data(), (unsigned short)parameters.size(), &result));
	}
}

void
KeyboardInput::KeyDown(KeyEventArgs ^args)
{
	static std::wstring keyboardEventName(L"keyboard");
	static std::wstring keyActionType(L"down");

	if (m_scriptCallback != JS_INVALID_REFERENCE)
	{
		std::vector<JsValueRef> parameters;
		EXIT_IF_FALSE(CreateScriptParametersListForKeyboard(keyboardEventName, keyActionType, args, parameters));

		JsValueRef result;
		EXIT_IF_JS_ERROR(JsCallFunction(m_scriptCallback, parameters.data(), (unsigned short)parameters.size(), &result));
	}
}

