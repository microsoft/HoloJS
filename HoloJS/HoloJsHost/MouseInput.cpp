#include "pch.h"
#include "MouseInput.h"

using namespace HologramJS::Input;
using namespace Windows::UI::Core;
using namespace std;
using namespace Windows::Foundation;

enum class MouseButtons : int
{
	NoButton = 0,
	LeftButton = 1,
	RightButton = 2,
	MiddleButton = 4
};

inline MouseButtons &
operator|=(MouseButtons & __x, MouseButtons __y)
{
	__x = static_cast<MouseButtons>(static_cast<int>(__x) | static_cast<int>(__y));
	return __x;
}

MouseInput::MouseInput()
{
	m_mouseDownToken = CoreWindow::GetForCurrentThread()->PointerPressed += ref new TypedEventHandler<CoreWindow ^, PointerEventArgs ^>(
		[this](CoreWindow ^sender, PointerEventArgs ^args)
	{
		this->MouseDown(args);
	});

	m_mouseUpToken = CoreWindow::GetForCurrentThread()->PointerReleased += ref new TypedEventHandler<CoreWindow ^, PointerEventArgs ^>(
		[this](CoreWindow ^sender, PointerEventArgs ^args)
	{
		this->MouseUp(args);
	});

	m_mouseWheelToken = CoreWindow::GetForCurrentThread()->PointerWheelChanged += ref new TypedEventHandler<CoreWindow ^, PointerEventArgs ^>(
		[this](CoreWindow ^sender, PointerEventArgs ^args)
	{
		this->MouseWheel(args);
	});

	m_mouseMoveToken = CoreWindow::GetForCurrentThread()->PointerMoved += ref new TypedEventHandler<CoreWindow ^, PointerEventArgs ^>(
		[this](CoreWindow ^sender, PointerEventArgs ^args)
	{
		this->MouseMove(args);
	});
}


MouseInput::~MouseInput()
{
	CoreWindow::GetForCurrentThread()->PointerPressed -= m_mouseDownToken;
	CoreWindow::GetForCurrentThread()->PointerReleased -= m_mouseUpToken;
	CoreWindow::GetForCurrentThread()->PointerWheelChanged -= m_mouseWheelToken;
	CoreWindow::GetForCurrentThread()->PointerMoved -= m_mouseMoveToken;
}

MouseButtons GetButtonFromArgs(PointerEventArgs^ args)
{
	auto returnValue = MouseButtons::NoButton;

	const auto pointProps = args->CurrentPoint->Properties;

	if (pointProps->IsLeftButtonPressed)
	{
		returnValue |= MouseButtons::LeftButton;
	}

	if (pointProps->IsMiddleButtonPressed)
	{
		returnValue |= MouseButtons::MiddleButton;
	}

	if (pointProps->IsRightButtonPressed)
	{
		returnValue |= MouseButtons::RightButton;
	}

	return returnValue;
}

bool
MouseInput::CreateScriptParametersListForMouse(
	const wstring& eventName,
	const wstring& actionType,
	PointerEventArgs^ args,
	JsValueRef (&outParams)[6]
)
{
	outParams[0] = m_scriptCallback;
	JsValueRef* eventNameParam = &outParams[1];
	JsValueRef* xParam = &outParams[2];
	JsValueRef* yParam = &outParams[3];
	JsValueRef* buttonParam = &outParams[4];
	JsValueRef* actionParam = &outParams[5];

	RETURN_IF_JS_ERROR(JsPointerToString(eventName.c_str(), eventName.length(), eventNameParam));
	RETURN_IF_JS_ERROR(JsDoubleToNumber(args->CurrentPoint->Position.X, xParam));
	RETURN_IF_JS_ERROR(JsDoubleToNumber(args->CurrentPoint->Position.Y, yParam));
	RETURN_IF_JS_ERROR(JsIntToNumber(static_cast<int>(GetButtonFromArgs(args)), buttonParam));
	RETURN_IF_JS_ERROR(JsPointerToString(actionType.c_str(), actionType.length(), actionParam));

	return true;
}

void
MouseInput::MouseDown(PointerEventArgs^ args)
{
	static std::wstring mouseEventName(L"mouse");
	static std::wstring mouseActionType(L"mousedown");

	EXIT_IF_TRUE(m_scriptCallback == JS_INVALID_REFERENCE);

	JsValueRef parameters[6];
	CreateScriptParametersListForMouse(mouseEventName, mouseActionType, args, parameters);

	JsValueRef result;
	EXIT_IF_JS_ERROR(JsCallFunction(m_scriptCallback, parameters, ARRAYSIZE(parameters), &result));
}

void
MouseInput::MouseUp(PointerEventArgs^ args)
{
	static std::wstring mouseEventName(L"mouse");
	static std::wstring mouseActionType(L"mouseup");
	
	EXIT_IF_TRUE(m_scriptCallback == JS_INVALID_REFERENCE);

	JsValueRef parameters[6];
	EXIT_IF_FALSE(CreateScriptParametersListForMouse(mouseEventName, mouseActionType, args, parameters));

	JsValueRef result;
	EXIT_IF_JS_ERROR(JsCallFunction(m_scriptCallback, parameters, ARRAYSIZE(parameters), &result));
}

void
MouseInput::MouseMove(PointerEventArgs^ args)
{
	static std::wstring mouseEventName(L"mouse");
	static std::wstring mouseActionType(L"mousemove");

	EXIT_IF_TRUE(m_scriptCallback == JS_INVALID_REFERENCE);

	JsValueRef parameters[6];
	CreateScriptParametersListForMouse(mouseEventName, mouseActionType, args, parameters);

	JsValueRef result;
	EXIT_IF_JS_ERROR(JsCallFunction(m_scriptCallback, parameters, ARRAYSIZE(parameters), &result));
}

void
MouseInput::MouseWheel(PointerEventArgs^ args)
{
	static std::wstring mouseEventName(L"mouse");
	static std::wstring mouseActionType(L"mousewheel");

	EXIT_IF_TRUE(m_scriptCallback == JS_INVALID_REFERENCE);
	
	JsValueRef parameters[6];
	CreateScriptParametersListForMouse(mouseEventName, mouseActionType, args, parameters);

	JsValueRef result;
	EXIT_IF_JS_ERROR(JsCallFunction(m_scriptCallback, parameters, ARRAYSIZE(parameters), &result));
}
