#include "pch.h"
#include "CanvasProjections.h"
#include "ExternalObject.h"
#include "ImageElement.h"
#include "RenderingContext2D.h"
#include "ScriptHostUtilities.h"
#include "ScriptResourceTracker.h"

using namespace HologramJS::Canvas;
using namespace HologramJS::Utilities;
using namespace HologramJS::API;
using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::Graphics::Canvas::Brushes;
using namespace Microsoft::Graphics::Canvas::Text;
using namespace Windows::UI;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Numerics;
using namespace std;

bool CanvasProjections::Initialize()
{
	// WebGL context projections
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"createContext2D", L"canvas2d", createContext2D));

	// Canvas 2D context projections
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"drawImage", L"canvas2d", drawImage));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"clearRect", L"canvas2d", clearRect));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"fillRect", L"canvas2d", fillRect));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"fillRectGradient", L"canvas2d", fillRectGradient));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"fillText", L"canvas2d", fillText));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"measureText", L"canvas2d", measureText));

	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"beginPath", L"canvas2d", beginPath));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"closePath", L"canvas2d", closePath));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"moveTo", L"canvas2d", moveTo));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"lineTo", L"canvas2d", lineTo));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"bezierCurveTo", L"canvas2d", bezierCurveTo));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"quadraticCurveTo", L"canvas2d", quadraticCurveTo));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"arc", L"canvas2d", arc));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"fill", L"canvas2d", fill));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"fillGradient", L"canvas2d", fillGradient));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"stroke", L"canvas2d", stroke));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"strokeGradient", L"canvas2d", strokeGradient));

	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"setLineStyle", L"canvas2d", setLineStyle));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"setGlobalOpacity", L"canvas2d", setGlobalOpacity));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"setTransform", L"canvas2d", setTransform));

	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"getImageData", L"canvas2d", getImageData));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"setWidth", L"canvas2d", setWidth));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"setHeight", L"canvas2d", setHeight));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"getWidth", L"canvas2d", getWidth));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"getHeight", L"canvas2d", getHeight));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"toDataURL", L"canvas2d", toDataURL));

	return true;
}

Color CanvasProjections::parseColor(JsValueRef objRef)
{
	JsValueRef redRef = ScriptHostUtilities::GetJsProperty(objRef, L"r");
	JsValueRef greenRef = ScriptHostUtilities::GetJsProperty(objRef, L"g");
	JsValueRef blueRef = ScriptHostUtilities::GetJsProperty(objRef, L"b");
	JsValueRef alphaRef = ScriptHostUtilities::GetJsProperty(objRef, L"a");

	Color color;
	color.A = static_cast<char>(ScriptHostUtilities::GLfloatFromJsRef(alphaRef) *
		255);  // Browser uses 0 - 1, UWP uses 0 - 255
	color.R = ScriptHostUtilities::GLintFromJsRef(redRef);
	color.G = ScriptHostUtilities::GLintFromJsRef(greenRef);
	color.B = ScriptHostUtilities::GLintFromJsRef(blueRef);

	return color;
}

Rect CanvasProjections::parseRect(JsValueRef objRef)
{
	JsValueRef xRef = ScriptHostUtilities::GetJsProperty(objRef, L"x");
	JsValueRef yRef = ScriptHostUtilities::GetJsProperty(objRef, L"y");
	JsValueRef widthRef = ScriptHostUtilities::GetJsProperty(objRef, L"width");
	JsValueRef heightRef = ScriptHostUtilities::GetJsProperty(objRef, L"height");

	Rect rect(static_cast<float>(ScriptHostUtilities::GLfloatFromJsRef(xRef)),
		static_cast<float>(ScriptHostUtilities::GLfloatFromJsRef(yRef)),
		static_cast<float>(ScriptHostUtilities::GLfloatFromJsRef(widthRef)),
		static_cast<float>(ScriptHostUtilities::GLfloatFromJsRef(heightRef)));

	return rect;
}

float2 CanvasProjections::parsePoint(JsValueRef objRef)
{
	JsValueRef xRef = ScriptHostUtilities::GetJsProperty(objRef, L"x");
	JsValueRef yRef = ScriptHostUtilities::GetJsProperty(objRef, L"y");

	return float2(static_cast<float>(ScriptHostUtilities::GLfloatFromJsRef(xRef)),
		static_cast<float>(ScriptHostUtilities::GLfloatFromJsRef(yRef)));
}

JsValueRef CHAKRA_CALLBACK CanvasProjections::createContext2D(
	JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
	return ScriptResourceTracker::ObjectToDirectExternal(new RenderingContext2D());
}

JsValueRef CHAKRA_CALLBACK CanvasProjections::drawImage(
	JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

	RenderingContext2D* context = ScriptResourceTracker::ExternalToObject<RenderingContext2D>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	auto imageElement = ScriptResourceTracker::ExternalToObject<API::ImageElement>(arguments[2]);
	Rect srcRect = parseRect(arguments[3]);
	Rect destRect = parseRect(arguments[4]);

	context->drawImage(imageElement, srcRect, destRect);
	return JS_INVALID_REFERENCE;
}

JsValueRef CHAKRA_CALLBACK CanvasProjections::clearRect(
	JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

	RenderingContext2D* context = ScriptResourceTracker::ExternalToObject<RenderingContext2D>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	Rect rect = parseRect(arguments[2]);
	context->clearRect(rect);
	return JS_INVALID_REFERENCE;
}

JsValueRef CHAKRA_CALLBACK CanvasProjections::fillRect(
	JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

	RenderingContext2D* context = ScriptResourceTracker::ExternalToObject<RenderingContext2D>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	Rect rect = parseRect(arguments[2]);
	Color color = parseColor(arguments[3]);

	context->fillRect(rect, color);
	return JS_INVALID_REFERENCE;
}

JsValueRef CHAKRA_CALLBACK CanvasProjections::fillRectGradient(
	JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

	RenderingContext2D* context = ScriptResourceTracker::ExternalToObject<RenderingContext2D>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	Rect rect = parseRect(arguments[2]);
	JsValueRef gradient = arguments[3];

	float2 start = parsePoint(ScriptHostUtilities::GetJsProperty(gradient, L"start"));
	float2 end = parsePoint(ScriptHostUtilities::GetJsProperty(gradient, L"end"));

	JsValueRef stopsRef = ScriptHostUtilities::GetJsProperty(gradient, L"stops");
	JsValueRef stopsLengthRef = ScriptHostUtilities::GetJsProperty(stopsRef, L"length");
	int stopsLength = ScriptHostUtilities::GLintFromJsRef(stopsLengthRef);

	auto stops = ref new Platform::Array<CanvasGradientStop>(stopsLength);
	for (int i = 0; i < stopsLength; i++) {
		JsValueRef stopRef = ScriptHostUtilities::GetJsProperty(stopsRef, std::to_wstring(i));
		JsValueRef positionRef = ScriptHostUtilities::GetJsProperty(stopRef, L"position");
		JsValueRef colorRef = ScriptHostUtilities::GetJsProperty(stopRef, L"color");

		stops[i].Position = static_cast<float>(ScriptHostUtilities::GLintFromJsRef(positionRef));
		stops[i].Color = parseColor(colorRef);
	}

	context->fillRectGradient(rect, start, end, stops);
	return JS_INVALID_REFERENCE;
}

JsValueRef CHAKRA_CALLBACK CanvasProjections::fillText(
	JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 7);

	RenderingContext2D* context = ScriptResourceTracker::ExternalToObject<RenderingContext2D>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	std::wstring text;
	ScriptHostUtilities::GetString(arguments[2], text);

	float2 point = parsePoint(arguments[3]);
	Color color = parseColor(arguments[4]);

	std::wstring fontFamily, fontWeight, fontStyle;
	int fontSize = ScriptHostUtilities::GLintFromJsRef(ScriptHostUtilities::GetJsProperty(arguments[5], L"size"));
	ScriptHostUtilities::GetString(ScriptHostUtilities::GetJsProperty(arguments[5], L"family"), fontFamily);
	ScriptHostUtilities::GetString(ScriptHostUtilities::GetJsProperty(arguments[5], L"weight"), fontWeight);
	ScriptHostUtilities::GetString(ScriptHostUtilities::GetJsProperty(arguments[5], L"style"), fontStyle);

	int alignment = ScriptHostUtilities::GLintFromJsRef(arguments[6]);

	context->fillText(text, point, color, fontSize, fontFamily, fontWeight, fontStyle, alignment);
	return JS_INVALID_REFERENCE;
}

JsValueRef CHAKRA_CALLBACK CanvasProjections::measureText(
	JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

	RenderingContext2D* context = ScriptResourceTracker::ExternalToObject<RenderingContext2D>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	std::wstring text;
	ScriptHostUtilities::GetString(arguments[2], text);

	int fontSize = ScriptHostUtilities::GLintFromJsRef(arguments[3]);

	std::wstring fontFamily;
	ScriptHostUtilities::GetString(arguments[4], fontFamily);

	double r = context->measureText(text, fontSize, fontFamily);

	JsValueRef returnValue;
	RETURN_INVALID_REF_IF_JS_ERROR(JsDoubleToNumber(r, &returnValue));
	return returnValue;
}

JsValueRef CHAKRA_CALLBACK CanvasProjections::beginPath(
	JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
	RenderingContext2D* context = ScriptResourceTracker::ExternalToObject<RenderingContext2D>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	context->beginPath();
	return JS_INVALID_REFERENCE;
}

JsValueRef CHAKRA_CALLBACK CanvasProjections::closePath(
	JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
	RenderingContext2D* context = ScriptResourceTracker::ExternalToObject<RenderingContext2D>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	context->closePath();
	return JS_INVALID_REFERENCE;
}

JsValueRef CHAKRA_CALLBACK CanvasProjections::moveTo(
	JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

	RenderingContext2D* context = ScriptResourceTracker::ExternalToObject<RenderingContext2D>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	float2 point = parsePoint(arguments[2]);

	context->moveTo(point.x, point.y);
	return JS_INVALID_REFERENCE;
}

JsValueRef CHAKRA_CALLBACK CanvasProjections::lineTo(
	JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

	RenderingContext2D* context = ScriptResourceTracker::ExternalToObject<RenderingContext2D>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	float2 point = parsePoint(arguments[2]);

	context->lineTo(point.x, point.y);
	return JS_INVALID_REFERENCE;
}

JsValueRef CHAKRA_CALLBACK CanvasProjections::bezierCurveTo(
	JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

	RenderingContext2D* context = ScriptResourceTracker::ExternalToObject<RenderingContext2D>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	float2 c1 = parsePoint(arguments[2]);
	float2 c2 = parsePoint(arguments[3]);
	float2 point = parsePoint(arguments[4]);

	context->bezierCurveTo(c1.x, c1.y, c2.x, c2.y, point.x, point.y);
	return JS_INVALID_REFERENCE;
}

JsValueRef CHAKRA_CALLBACK CanvasProjections::quadraticCurveTo(
	JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

	RenderingContext2D* context = ScriptResourceTracker::ExternalToObject<RenderingContext2D>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	float2 c1 = parsePoint(arguments[2]);
	float2 point = parsePoint(arguments[3]);

	context->quadraticCurveTo(c1.x, c1.y, point.x, point.y);
	return JS_INVALID_REFERENCE;
}

JsValueRef CHAKRA_CALLBACK CanvasProjections::arc(
	JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 7);

	RenderingContext2D* context = ScriptResourceTracker::ExternalToObject<RenderingContext2D>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	float2 c = parsePoint(arguments[2]);
	float r = ScriptHostUtilities::GLfloatFromJsRef(arguments[3]);
	float angle1 = ScriptHostUtilities::GLfloatFromJsRef(arguments[4]);
	float angle2 = ScriptHostUtilities::GLfloatFromJsRef(arguments[5]);
	int counterCW = ScriptHostUtilities::GLintFromJsRef(arguments[6]);

	context->arc(c.x, c.y, r, angle1, angle2, counterCW);
	return JS_INVALID_REFERENCE;
}

JsValueRef CHAKRA_CALLBACK CanvasProjections::fill(
	JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

	RenderingContext2D* context = ScriptResourceTracker::ExternalToObject<RenderingContext2D>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	Color color = parseColor(arguments[2]);

	context->fill(color);
	return JS_INVALID_REFERENCE;
}

JsValueRef CHAKRA_CALLBACK CanvasProjections::fillGradient(
	JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

	RenderingContext2D* context = ScriptResourceTracker::ExternalToObject<RenderingContext2D>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	JsValueRef gradient = arguments[2];

	float2 start = parsePoint(ScriptHostUtilities::GetJsProperty(gradient, L"start"));
	float2 end = parsePoint(ScriptHostUtilities::GetJsProperty(gradient, L"end"));

	JsValueRef stopsRef = ScriptHostUtilities::GetJsProperty(gradient, L"stops");
	JsValueRef stopsLengthRef = ScriptHostUtilities::GetJsProperty(stopsRef, L"length");
	int stopsLength = ScriptHostUtilities::GLintFromJsRef(stopsLengthRef);

	auto stops = ref new Platform::Array<CanvasGradientStop>(stopsLength);
	for (int i = 0; i < stopsLength; i++) {
		JsValueRef stopRef = ScriptHostUtilities::GetJsProperty(stopsRef, std::to_wstring(i));
		JsValueRef positionRef = ScriptHostUtilities::GetJsProperty(stopRef, L"position");
		JsValueRef colorRef = ScriptHostUtilities::GetJsProperty(stopRef, L"color");

		stops[i].Position = static_cast<float>(ScriptHostUtilities::GLintFromJsRef(positionRef));
		stops[i].Color = parseColor(colorRef);
	}

	context->fillGradient(start, end, stops);
	return JS_INVALID_REFERENCE;
}

JsValueRef CHAKRA_CALLBACK CanvasProjections::stroke(
	JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

	RenderingContext2D* context = ScriptResourceTracker::ExternalToObject<RenderingContext2D>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	Color color = parseColor(arguments[2]);

	context->stroke(color);
	return JS_INVALID_REFERENCE;
}

JsValueRef CHAKRA_CALLBACK CanvasProjections::strokeGradient(
	JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

	RenderingContext2D* context = ScriptResourceTracker::ExternalToObject<RenderingContext2D>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	JsValueRef gradient = arguments[2];

	float2 start = parsePoint(ScriptHostUtilities::GetJsProperty(gradient, L"start"));
	float2 end = parsePoint(ScriptHostUtilities::GetJsProperty(gradient, L"end"));

	JsValueRef stopsRef = ScriptHostUtilities::GetJsProperty(gradient, L"stops");
	JsValueRef stopsLengthRef = ScriptHostUtilities::GetJsProperty(stopsRef, L"length");
	int stopsLength = ScriptHostUtilities::GLintFromJsRef(stopsLengthRef);

	auto stops = ref new Platform::Array<CanvasGradientStop>(stopsLength);
	for (int i = 0; i < stopsLength; i++) {
		JsValueRef stopRef = ScriptHostUtilities::GetJsProperty(stopsRef, std::to_wstring(i));
		JsValueRef positionRef = ScriptHostUtilities::GetJsProperty(stopRef, L"position");
		JsValueRef colorRef = ScriptHostUtilities::GetJsProperty(stopRef, L"color");

		stops[i].Position = static_cast<float>(ScriptHostUtilities::GLintFromJsRef(positionRef));
		stops[i].Color = parseColor(colorRef);
	}

	context->strokeGradient(start, end, stops);
	return JS_INVALID_REFERENCE;
}

JsValueRef CHAKRA_CALLBACK CanvasProjections::setTransform(
	JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 8);

	RenderingContext2D* context = ScriptResourceTracker::ExternalToObject<RenderingContext2D>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	float a = ScriptHostUtilities::GLfloatFromJsRef(arguments[2]);
	float b = ScriptHostUtilities::GLfloatFromJsRef(arguments[3]);
	float c = ScriptHostUtilities::GLfloatFromJsRef(arguments[4]);
	float d = ScriptHostUtilities::GLfloatFromJsRef(arguments[5]);
	float e = ScriptHostUtilities::GLfloatFromJsRef(arguments[6]);
	float f = ScriptHostUtilities::GLfloatFromJsRef(arguments[7]);

	context->setTransform(a, b, c, d, e, f);
	return JS_INVALID_REFERENCE;
}

JsValueRef CHAKRA_CALLBACK CanvasProjections::setLineStyle(
	JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

	RenderingContext2D* context = ScriptResourceTracker::ExternalToObject<RenderingContext2D>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	float lineWidth = ScriptHostUtilities::GLfloatFromJsRef(arguments[2]);
	int capMode = ScriptHostUtilities::GLintFromJsRef(arguments[3]); // 0 -> Flat, 1 -> Round, 2 -> Square
	int joinMode = ScriptHostUtilities::GLintFromJsRef(arguments[4]); // 1 -> Bevel, 0 -> Miter, 2 -> Round

	context->setLineStyle(lineWidth, (Geometry::CanvasCapStyle)capMode, (Geometry::CanvasLineJoin)joinMode);
	return JS_INVALID_REFERENCE;
}

JsValueRef CHAKRA_CALLBACK CanvasProjections::setGlobalOpacity(
	JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

	RenderingContext2D* context = ScriptResourceTracker::ExternalToObject<RenderingContext2D>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	float value = ScriptHostUtilities::GLfloatFromJsRef(arguments[2]);

	context->setGlobalOpacity(value);
	return JS_INVALID_REFERENCE;
}

JsValueRef CHAKRA_CALLBACK CanvasProjections::getImageData(
	JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

	RenderingContext2D* context = ScriptResourceTracker::ExternalToObject<RenderingContext2D>(arguments[1]);

	RETURN_INVALID_REF_IF_NULL(context);

	Rect rect = parseRect(arguments[2]);

	// Create storage for the pixels in JS
	JsValueRef jsArray;
	if (context->OptimizedBufferAvailable()) {
		RETURN_NULL_IF_JS_ERROR(JsCreateTypedArray(
			JsTypedArrayType::JsArrayTypeUint8Clamped, nullptr, 0, context->GetOptimizedBufferSize(), &jsArray));

		// Get a pointer to the newly allocated JS array
		ChakraBytePtr jsArrayPointer;
		JsTypedArrayType jsArrayType;
		unsigned int jsArrayLength;
		int jsArrayElementSize;
		RETURN_NULL_IF_JS_ERROR(
			JsGetTypedArrayStorage(jsArray, &jsArrayPointer, &jsArrayLength, &jsArrayType, &jsArrayElementSize));
		context->CopyOptimizedBitmapToBuffer(jsArrayPointer);
	}
	else {
		unsigned int canvasStride;
		auto pixelsArray = context->getImageData(rect, &canvasStride);

		RETURN_NULL_IF_JS_ERROR(
			JsCreateTypedArray(JsTypedArrayType::JsArrayTypeUint8Clamped, nullptr, 0, pixelsArray->Length, &jsArray));

		// Get a pointer to the newly allocated JS array and copy the pixels there
		ChakraBytePtr jsArrayPointer;
		JsTypedArrayType jsArrayType;
		unsigned int jsArrayLength;
		int jsArrayElementSize;

		RETURN_NULL_IF_JS_ERROR(
			JsGetTypedArrayStorage(jsArray, &jsArrayPointer, &jsArrayLength, &jsArrayType, &jsArrayElementSize));

		CopyMemory(jsArrayPointer, pixelsArray->begin(), pixelsArray->Length);
	}

	return jsArray;
}

JsValueRef CHAKRA_CALLBACK CanvasProjections::getWidth(
	JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
	RenderingContext2D* context = ScriptResourceTracker::ExternalToObject<RenderingContext2D>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	JsValueRef output;
	JsIntToNumber(context->getWidth(), &output);
	return output;
}

JsValueRef CHAKRA_CALLBACK CanvasProjections::setWidth(
	JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
	RenderingContext2D* context = ScriptResourceTracker::ExternalToObject<RenderingContext2D>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	int width = ScriptHostUtilities::GLintFromJsRef(arguments[2]);
	context->setWidth(width);
	return JS_INVALID_REFERENCE;
}

JsValueRef CHAKRA_CALLBACK CanvasProjections::getHeight(
	JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
	RenderingContext2D* context = ScriptResourceTracker::ExternalToObject<RenderingContext2D>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	JsValueRef output;
	JsIntToNumber(context->getHeight(), &output);
	return output;
}

JsValueRef CHAKRA_CALLBACK CanvasProjections::setHeight(
	JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
	RenderingContext2D* context = ScriptResourceTracker::ExternalToObject<RenderingContext2D>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	int height = ScriptHostUtilities::GLintFromJsRef(arguments[2]);
	context->setHeight(height);
	return JS_INVALID_REFERENCE;
}

JsValueRef CHAKRA_CALLBACK CanvasProjections::toDataURL(
	JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3 || argumentCount == 4);

	RenderingContext2D* context = ScriptResourceTracker::ExternalToObject<RenderingContext2D>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	wstring type;
	RETURN_INVALID_REF_IF_FALSE(ScriptHostUtilities::GetString(arguments[2], type));

	double encoderOptions = 0.92f;
	if (argumentCount == 4) {
		JsValueType encoderOptionsType;
		RETURN_INVALID_REF_IF_JS_ERROR(JsGetValueType(arguments[3], &encoderOptionsType));

		if (encoderOptionsType != JsUndefined) {
			encoderOptions = ScriptHostUtilities::GLfloatFromJsRef(arguments[3]);
		}
	}

	wstring encodedImage;
	RETURN_INVALID_REF_IF_FALSE(context->toDataURL(type, encoderOptions, &encodedImage));

	JsValueRef result;
	RETURN_INVALID_REF_IF_JS_ERROR(JsPointerToString(encodedImage.c_str(), encodedImage.length(), &result));

	return result;
}
