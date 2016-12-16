#include "pch.h"
#include "MouseInput.h"

using namespace HologramJS::Input;
using namespace Windows::UI::Core;
using namespace std;

MouseInput::MouseInput()
{
}


MouseInput::~MouseInput()
{
}

int GetButtonFromArgs(PointerEventArgs^ args)
{
	auto pointProps = args->CurrentPoint->Properties;
	const bool left = pointProps->IsLeftButtonPressed;
	const bool middle = pointProps->IsMiddleButtonPressed;
	const bool right = pointProps->IsRightButtonPressed;
	if (left && !middle && !right)
	{
		return 1;
	}
	else if (left && middle && !right)
	{
		return 5;
	}
	else if (left && middle && right)
	{
		return 7;
	}
	else if (left && !middle && right)
	{
		return 3;
	}
	else if (!left && middle && right)
	{
		return 6;
	}
	else if (!left && middle && !right)
	{
		return 4;
	}
	else if (!left && !middle && right)
	{
		return 2;
	}
	else
	{
		return 0;
	}
}

bool
MouseInput::CreateScriptParametersListForMouse(
	const wstring& eventName,
	const wstring& actionType,
	PointerEventArgs^ args,
	vector<JsValueRef>& outParams
)
{
	outParams.resize(6);

	outParams[0] = m_scriptCallback;
	JsValueRef* eventNameParam = &outParams[1];
	JsValueRef* xParam = &outParams[2];
	JsValueRef* yParam = &outParams[3];
	JsValueRef* buttonParam = &outParams[4];
	JsValueRef* actionParam = &outParams[5];

	RETURN_IF_JS_ERROR(JsPointerToString(eventName.c_str(), eventName.length(), eventNameParam));
	RETURN_IF_JS_ERROR(JsDoubleToNumber(args->CurrentPoint->Position.X, xParam));
	RETURN_IF_JS_ERROR(JsDoubleToNumber(args->CurrentPoint->Position.Y, yParam));
	RETURN_IF_JS_ERROR(JsIntToNumber(GetButtonFromArgs(args), buttonParam));
	RETURN_IF_JS_ERROR(JsPointerToString(actionType.c_str(), actionType.length(), actionParam));

	return true;
}

void
MouseInput::MouseDown(PointerEventArgs^ args)
{
	static std::wstring mouseEventName(L"mouse");
	static std::wstring mouseActionType(L"mousedown");

	if (m_scriptCallback != JS_INVALID_REFERENCE)
	{
		std::vector<JsValueRef> parameters;
		CreateScriptParametersListForMouse(mouseEventName, mouseActionType, args, parameters);

		JsValueRef result;
		EXIT_IF_JS_ERROR(JsCallFunction(m_scriptCallback, parameters.data(), (unsigned short)parameters.size(), &result));
	}
}

void
MouseInput::MouseUp(PointerEventArgs^ args)
{
	static std::wstring mouseEventName(L"mouse");
	static std::wstring mouseActionType(L"mouseup");
	
	if (m_scriptCallback != JS_INVALID_REFERENCE)
	{
		std::vector<JsValueRef> parameters;
		EXIT_IF_FALSE(CreateScriptParametersListForMouse(mouseEventName, mouseActionType, args, parameters));

		JsValueRef result;
		EXIT_IF_JS_ERROR(JsCallFunction(m_scriptCallback, parameters.data(), (unsigned short)parameters.size(), &result));
	}
}

void
MouseInput::MouseMove(PointerEventArgs^ args)
{
	static std::wstring mouseEventName(L"mouse");
	static std::wstring mouseActionType(L"mousemove");

	if (m_scriptCallback != JS_INVALID_REFERENCE)
	{
		vector<JsValueRef> parameters;
		CreateScriptParametersListForMouse(mouseEventName, mouseActionType, args, parameters);

		JsValueRef result;
		EXIT_IF_JS_ERROR(JsCallFunction(m_scriptCallback, parameters.data(), (unsigned short)parameters.size(), &result));
	}
}

void
MouseInput::MouseWheel(PointerEventArgs^ args)
{
	static std::wstring mouseEventName(L"mouse");
	static std::wstring mouseActionType(L"mousewheel");

	if (m_scriptCallback != JS_INVALID_REFERENCE)
	{
		vector<JsValueRef> parameters;
		CreateScriptParametersListForMouse(mouseEventName, mouseActionType, args, parameters);

		JsValueRef result;
		EXIT_IF_JS_ERROR(JsCallFunction(m_scriptCallback, parameters.data(), (unsigned short)parameters.size(), &result));
	}
}
