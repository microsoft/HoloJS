#include "pch.h"
#include "ScriptHostUtilities.h"

using namespace HologramJS::Utilities;
using namespace std;

bool
ScriptHostUtilities::GetJsProperty(
	JsValueRef& parentObject,
	const std::wstring& name,
	JsValueRef* outProperty
)
{
	JsPropertyIdRef propertyId;
	RETURN_IF_JS_ERROR(JsGetPropertyIdFromName(name.c_str(), &propertyId));

	// Create or get parent.name
	bool hasProperty;
	JsValueRef property;
	RETURN_IF_JS_ERROR(JsHasProperty(parentObject, propertyId, &hasProperty));
	if (!hasProperty)
	{
		RETURN_IF_JS_ERROR(JsCreateObject(&property));
		RETURN_IF_JS_ERROR(JsSetProperty(parentObject, propertyId, property, true));
	}
	else
	{
		RETURN_IF_JS_ERROR(JsGetProperty(parentObject, propertyId, &property));
	}

	*outProperty = property;

	return true;
}

bool
ScriptHostUtilities::ProjectFunction(
	const std::wstring& name,
	const std::wstring& namespaceName,
	JsNativeFunction nativeFunction,
	void* callbackState,
	JsValueRef* scriptFunction
)
{
	// Get the global object
	JsValueRef globalObject;

	RETURN_IF_JS_ERROR(JsGetGlobalObject(&globalObject));

	// Create or get global.nativeInterface
	JsValueRef nativeInterface;
	RETURN_IF_FALSE(GetJsProperty(globalObject, L"nativeInterface", &nativeInterface));

	// Create or get global.nativeInterface.[namespace]

	JsValueRef namespaceProperty;
	RETURN_IF_FALSE(GetJsProperty(nativeInterface, namespaceName, &namespaceProperty));

	JsPropertyIdRef functionPropertyId;
	RETURN_IF_JS_ERROR(JsGetPropertyIdFromName(name.c_str(), &functionPropertyId));
	RETURN_IF_JS_ERROR(JsCreateFunction(nativeFunction, callbackState, scriptFunction));
	RETURN_IF_JS_ERROR(JsSetProperty(namespaceProperty, functionPropertyId, *scriptFunction, true));

	return true;
}

bool
ScriptHostUtilities::GetString(
	JsValueRef value,
	wstring& outString
)
{
	PCWSTR rawString;
	size_t rawStringLength;
	RETURN_IF_JS_ERROR(JsStringToPointer(value, &rawString, &rawStringLength));

	outString.assign(rawString);

	return true;
}

GLclampf
ScriptHostUtilities::GLclampfFromJsRef(JsValueRef value)
{
	double doubleValue;
	if (JsNumberToDouble(value, &doubleValue) != JsNoError)
	{
		return 0;
	}

	return (GLclampf)doubleValue;
}

GLenum
ScriptHostUtilities::GLenumFromJsRef(JsValueRef value)
{
	int intValue;
	if (JsNumberToInt(value, &intValue) != JsNoError)
	{
		return 0;
	}

	return static_cast<unsigned int>(intValue);
}

GLint
ScriptHostUtilities::GLintFromJsRef(JsValueRef value)
{
	int intValue;
	if (JsNumberToInt(value, &intValue) != JsNoError)
	{
		return 0;
	}

	return intValue;
}

GLboolean
ScriptHostUtilities::GLbooleanFromJsRef(JsValueRef value)
{
	bool boolValue;
	if (JsBooleanToBool(value, &boolValue) != JsNoError)
	{
		return false;
	}

	return boolValue;
}

GLbitfield
ScriptHostUtilities::GLbitfieldFromJsRef(JsValueRef value)
{
	return static_cast<GLbitfield>(GLintFromJsRef(value));
}

GLsizeiptr
ScriptHostUtilities::GLsizeiptrFromJsRef(JsValueRef value)
{
	double doubleValue;
	if (JsNumberToDouble(value, &doubleValue) != JsNoError)
	{
		return 0;
	}

	return static_cast<GLsizeiptr>(doubleValue);
}

GLuint
ScriptHostUtilities::GLuintFromJsRef(JsValueRef value)
{
	return static_cast<GLuint>(GLintFromJsRef(value));
}

GLsizei
ScriptHostUtilities::GLsizeiFromJsRef(JsValueRef value)
{
	return GLintFromJsRef(value);
}

GLfloat
ScriptHostUtilities::GLfloatFromJsRef(JsValueRef value)
{
	double doubleValue;
	if (JsNumberToDouble(value, &doubleValue) != JsNoError)
	{
		return 0;
	}

	return static_cast<GLfloat>(doubleValue);
}