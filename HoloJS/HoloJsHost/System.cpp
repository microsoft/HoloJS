#include "pch.h"
#include "System.h"


using namespace HologramJS::API;
using namespace HologramJS::Utilities;
using namespace concurrency;
using namespace std;

bool
System::Initialize()
{
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"log", L"system", logStatic, this, &m_logFunction));

	return true;
}

JsValueRef
System::log(
	JsValueRef callee,
	JsValueRef* arguments,
	unsigned short argumentCount
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

	const wchar_t* logString;
	size_t logStringLength;
	RETURN_INVALID_REF_IF_JS_ERROR(JsStringToPointer(arguments[1], &logString, &logStringLength));

	OutputDebugString(logString);

	return JS_INVALID_REFERENCE;
}