#include "pch.h"
#include "WindowElement.h"

#include <WindowsNumerics.h>

using namespace HologramJS::API;
using namespace HologramJS::Utilities;

WindowElement::WindowElement()
{
}

WindowElement::~WindowElement()
{
	Close();
}

void
WindowElement::Close()
{
	if (m_callbackFunction != JS_INVALID_REFERENCE)
	{
		unsigned int refCount;
		JsRelease(m_callbackFunction, &refCount);
		m_callbackFunction = JS_INVALID_REFERENCE;
	}
}

bool
WindowElement::Initialize()
{
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"getWidth", L"window", getWidthStatic, this, &m_getWidthFunction));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"getHeight", L"window", getHeightStatic, this, &m_getHeightFunction));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"getBaseUrl", L"window", getBaseUrlStatic, this, &m_getBaseUrlFunction));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"setCallback", L"window", setCallbackStatic, this, &m_setCallbackFunction));

	RETURN_IF_FALSE(CreateViewMatrixStorageAndScriptProjection());

	return true;
}

JsValueRef
WindowElement::getBaseUrl(
	JsValueRef* arguments,
	unsigned short argumentCount
)
{
	JsValueRef baseUrlValue;
	RETURN_INVALID_REF_IF_JS_ERROR(JsPointerToString(m_baseUrl.c_str(), m_baseUrl.length(), &baseUrlValue));
	return baseUrlValue;
}

JsValueRef
WindowElement::setCallback(
	JsValueRef* arguments,
	unsigned short argumentCount
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

	JsValueRef callback = arguments[1];
	
	unsigned int refCount;
	RETURN_INVALID_REF_IF_JS_ERROR(JsAddRef(callback, &refCount));

	m_callbackFunction = callback;

	m_keyboardInput.SetScriptCallback(callback);
	m_mouseInput.SetScriptCallback(callback);

	return JS_INVALID_REFERENCE;
}


JsValueRef
WindowElement::getWidth(
	JsValueRef* arguments,
	unsigned short argumentCount
)
{
	JsValueRef widthValue;
	RETURN_INVALID_REF_IF_JS_ERROR(JsIntToNumber(m_width, &widthValue));
	return widthValue;
}


JsValueRef
WindowElement::getHeight(
	JsValueRef* arguments,
	unsigned short argumentCount
)
{
	JsValueRef heightValue;
	RETURN_NULL_IF_JS_ERROR(JsIntToNumber(m_height, &heightValue));
	return heightValue;
}

void
WindowElement::Resize(int width, int height)
{
	static std::wstring resizeEventName(L"resize");
	const bool shouldFireResize = (m_width != width || m_height != height);

	if (shouldFireResize)
	{
		m_width = width;
		m_height = height;

		if (m_callbackFunction != JS_INVALID_REFERENCE)
		{
			std::vector<JsValueRef> parameters(2);
			parameters[0] = m_callbackFunction;
			JsValueRef* eventNameParam = &parameters[1];

			EXIT_IF_JS_ERROR(JsPointerToString(resizeEventName.c_str(), resizeEventName.length(), eventNameParam));

			JsValueRef result;
			EXIT_IF_JS_ERROR(JsCallFunction(m_callbackFunction, parameters.data(), (unsigned short)parameters.size(), &result));
		}
	}
}

void
WindowElement::VSync(Windows::Foundation::Numerics::float4x4 viewMatrix)
{
	if (m_callbackFunction != JS_INVALID_REFERENCE)
	{
		if (m_viewMatrixStoragePointer != nullptr)
		{
			// Update the view matrix inside the JSVM
			CopyMemory(m_viewMatrixStoragePointer, &viewMatrix.m11, 16 * sizeof(float));
		}

		if (m_cameraPositionStoragePointer != nullptr)
		{
			// Inverse the view matrix in order to find back the translation equivalent to the position
			// in the last column.
			bool success = invert(viewMatrix, &m_inverseViewMatrix);
			if (success)
			{
				// Update the camera Position inside the JSVM
				CopyMemory(m_cameraPositionStoragePointer, &m_inverseViewMatrix.m41, 4 * sizeof(float));
			}
		}

		std::vector<JsValueRef> parameters(1);
		parameters[0] = m_callbackFunction;
		JsValueRef result;
		EXIT_IF_JS_ERROR(JsCallFunction(m_callbackFunction, parameters.data(), static_cast<unsigned short>(parameters.size()), &result));
	}
}

bool
WindowElement::CreateViewMatrixStorageAndScriptProjection()
{
	if (m_viewMatrixScriptProjection == JS_INVALID_REFERENCE)
	{
		RETURN_IF_JS_ERROR(JsCreateTypedArray(JsTypedArrayType::JsArrayTypeFloat32, nullptr, 0, 16, &m_viewMatrixScriptProjection));
		RETURN_IF_JS_ERROR(JsCreateTypedArray(JsTypedArrayType::JsArrayTypeFloat32, nullptr, 0, 4, &m_cameraPositionScriptProjection));

		JsValueRef globalObject;
		RETURN_IF_JS_ERROR(JsGetGlobalObject(&globalObject));

		JsValueRef nativeInterface;
		RETURN_IF_FALSE(ScriptHostUtilities::GetJsProperty(globalObject, L"nativeInterface", &nativeInterface));

		JsValueRef windowObject;
		RETURN_IF_FALSE(ScriptHostUtilities::GetJsProperty(nativeInterface, L"window", &windowObject));

		JsPropertyIdRef viewMatrixId;
		RETURN_IF_JS_ERROR(JsGetPropertyIdFromName(L"holographicViewMatrix", &viewMatrixId));
		RETURN_IF_JS_ERROR(JsSetProperty(windowObject, viewMatrixId, m_viewMatrixScriptProjection, true));

		JsPropertyIdRef cameraPositionId;
		RETURN_IF_JS_ERROR(JsGetPropertyIdFromName(L"holographicCameraPosition", &cameraPositionId));
		RETURN_IF_JS_ERROR(JsSetProperty(windowObject, cameraPositionId, m_cameraPositionScriptProjection, true));

		unsigned int bufferLength;
		JsTypedArrayType type;
		int elementSize;
		RETURN_IF_JS_ERROR(
			JsGetTypedArrayStorage(
				m_viewMatrixScriptProjection,
				(ChakraBytePtr*)&m_viewMatrixStoragePointer, &bufferLength, &type, &elementSize));
		RETURN_IF_JS_ERROR(
			JsGetTypedArrayStorage(
				m_cameraPositionScriptProjection,
				(ChakraBytePtr*)&m_cameraPositionStoragePointer, &bufferLength, &type, &elementSize));
	}

	return true;
}