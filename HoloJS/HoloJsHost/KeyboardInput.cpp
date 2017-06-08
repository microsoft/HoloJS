#include "pch.h"
#include "KeyboardInput.h"

using namespace HologramJS::Input;
using namespace Windows::UI::Core;
using namespace std;
using namespace Windows::Foundation;

KeyboardInput::KeyboardInput()
{
	m_keyDownToken = CoreWindow::GetForCurrentThread()->KeyDown += ref new TypedEventHandler<CoreWindow ^, KeyEventArgs ^>(
		[this](CoreWindow ^sender, KeyEventArgs ^args)
	{
		this->KeyDown(args);
	});

	m_keyUpToken = CoreWindow::GetForCurrentThread()->KeyUp += ref new TypedEventHandler<CoreWindow ^, KeyEventArgs ^>(
		[this](CoreWindow ^sender, KeyEventArgs ^args)
	{
		this->KeyUp(args);
	});
}

KeyboardInput::~KeyboardInput()
{
	CoreWindow::GetForCurrentThread()->KeyDown -= m_keyDownToken;
	CoreWindow::GetForCurrentThread()->KeyUp -= m_keyUpToken;
}

bool
KeyboardInput::CreateScriptParametersListForKeyboard(
	const wstring& eventName,
	const wstring& actionType,
	KeyEventArgs^ args,
	JsValueRef (&outParams)[4]
)
{
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

	EXIT_IF_TRUE(m_scriptCallback == JS_INVALID_REFERENCE);
	
	JsValueRef parameters[4];
	EXIT_IF_FALSE(CreateScriptParametersListForKeyboard(keyboardEventName, keyActionType, args, parameters));

	JsValueRef result;
	EXIT_IF_JS_ERROR(JsCallFunction(m_scriptCallback, parameters, ARRAYSIZE(parameters), &result));
}

void
KeyboardInput::KeyDown(KeyEventArgs ^args)
{
	static std::wstring keyboardEventName(L"keyboard");
	static std::wstring keyActionType(L"down");

	EXIT_IF_TRUE(m_scriptCallback == JS_INVALID_REFERENCE);
	
	JsValueRef parameters[4];
	EXIT_IF_FALSE(CreateScriptParametersListForKeyboard(keyboardEventName, keyActionType, args, parameters));

	JsValueRef result;
	EXIT_IF_JS_ERROR(JsCallFunction(m_scriptCallback, parameters, ARRAYSIZE(parameters), &result));
}

