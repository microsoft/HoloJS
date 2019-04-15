#include "stdafx.h"
#include "holojs/private/error-handling.h"
#include "holojs/private/script-host-utilities.h"
#include "holojs/windows/IBufferOnMemory.h"
#include "include/holojs/windows/render-context-2d.h"
#include "win2d-c-interface.h"
#include <WindowsNumerics.h>
#include <wrl.h>
// Media Foundation
//#include <mfidl.h>
//#include <Mfreadwrite.h>
//#include <mfapi.h>

using namespace HoloJs;
using namespace HoloJs::WindowsPlatform;
using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::Graphics::Canvas::Brushes;
using namespace Microsoft::Graphics::Canvas::Text;
using namespace Windows::Graphics::DirectX;
using namespace Windows::UI;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Numerics;
using namespace Microsoft::WRL;
using namespace Windows::Security::Cryptography;
using namespace Windows::Storage::Streams;
using namespace std;

CanvasLinearGradientBrush ^
    getCanvasLinearGradientBrush(void* rendererUnknown,
                                 Microsoft::Graphics::Canvas::Brushes::CanvasGradientStop* stops,
                                 unsigned int stopCount) {
        void* brushPtr;
        RETURN_NULL_IF_FAILED(getCanvasLinearGradientBrush(rendererUnknown, stops, stopCount, &brushPtr));
        return safe_cast<CanvasLinearGradientBrush ^>(reinterpret_cast<Platform::Object ^>(brushPtr));
    }

    CanvasTextFormat
    ^
    getCanvasTextFormat() {
        void* formatRaw;
        RETURN_NULL_IF_FAILED(getCanvasTextFormat(&formatRaw));

        return safe_cast<CanvasTextFormat ^>(reinterpret_cast<Platform::Object ^>(formatRaw));
    }

	CanvasTextLayout ^ getCanvasTextLayout(CanvasDrawingSession^ session,
		const wchar_t* text,
		CanvasTextFormat^ canvasTextFormat,
		float width,
		float height)
	{
		void* sessionUnknown = reinterpret_cast<IInspectable*>(session);
		void* canvasTextFormatUnknown = reinterpret_cast<IInspectable*>(canvasTextFormat);

		void* canvasTextLayoutUnknown = nullptr;
		RETURN_NULL_IF_FAILED(getCanvasTextLayout(sessionUnknown, text, canvasTextFormatUnknown, width, height, &canvasTextLayoutUnknown));
		return safe_cast<CanvasTextLayout ^>(reinterpret_cast<Platform::Object ^>(canvasTextLayoutUnknown));
	}

	Geometry::CanvasStrokeStyle^ getCanvasStrokeStyle()
	{
		void* canvasStrokeStyleUnknown;
		RETURN_NULL_IF_FAILED(getCanvasStrokeStyle(&canvasStrokeStyleUnknown));
		return safe_cast<Geometry::CanvasStrokeStyle ^>(reinterpret_cast<Platform::Object ^>(canvasStrokeStyleUnknown));
	}

	Geometry::CanvasGeometry ^ canvasGeometryCreatePath(Geometry::CanvasPathBuilder^ path)
	{
		void* pathUnknown = reinterpret_cast<IInspectable*>(path);
		void *canvasGeometryUnknown;
		RETURN_NULL_IF_FAILED(canvasGeometryCreatePath(pathUnknown, &canvasGeometryUnknown));
		return safe_cast<Geometry::CanvasGeometry ^>(reinterpret_cast<Platform::Object ^>(canvasGeometryUnknown));
	}

HRESULT parseColor(JsValueRef objRef, Color& color)
{
    JsValueRef redRef;
    RETURN_IF_FAILED(ScriptHostUtilities::GetJsProperty(objRef, L"r", &redRef));

    JsValueRef greenRef;
    RETURN_IF_FAILED(ScriptHostUtilities::GetJsProperty(objRef, L"g", &greenRef));

    JsValueRef blueRef;
    RETURN_IF_FAILED(ScriptHostUtilities::GetJsProperty(objRef, L"b", &blueRef));

    JsValueRef alphaRef;
    RETURN_IF_FAILED(ScriptHostUtilities::GetJsProperty(objRef, L"a", &alphaRef));

    color.A = static_cast<char>(ScriptHostUtilities::GLfloatFromJsRef(alphaRef) *
                                255);  // Browser uses 0 - 1, UWP uses 0 - 255
    color.R = ScriptHostUtilities::GLintFromJsRef(redRef);
    color.G = ScriptHostUtilities::GLintFromJsRef(greenRef);
    color.B = ScriptHostUtilities::GLintFromJsRef(blueRef);

    return S_OK;
}

HRESULT parseRect(JsValueRef objRef, Rect& rect)
{
    JsValueRef xRef;
    RETURN_IF_FAILED(ScriptHostUtilities::GetJsProperty(objRef, L"x", &xRef));

    JsValueRef yRef;
    RETURN_IF_FAILED(ScriptHostUtilities::GetJsProperty(objRef, L"y", &yRef));

    JsValueRef widthRef;
    RETURN_IF_FAILED(ScriptHostUtilities::GetJsProperty(objRef, L"width", &widthRef));

    JsValueRef heightRef;
    RETURN_IF_FAILED(ScriptHostUtilities::GetJsProperty(objRef, L"height", &heightRef));

    rect = Rect(static_cast<float>(ScriptHostUtilities::GLfloatFromJsRef(xRef)),
                static_cast<float>(ScriptHostUtilities::GLfloatFromJsRef(yRef)),
                static_cast<float>(ScriptHostUtilities::GLfloatFromJsRef(widthRef)),
                static_cast<float>(ScriptHostUtilities::GLfloatFromJsRef(heightRef)));

    return S_OK;
}

HRESULT parsePoint(JsValueRef objRef, float2& point)
{
    JsValueRef xRef;
    RETURN_IF_FAILED(ScriptHostUtilities::GetJsProperty(objRef, L"x", &xRef));

    JsValueRef yRef;
    RETURN_IF_FAILED(ScriptHostUtilities::GetJsProperty(objRef, L"y", &yRef));

    point = float2(ScriptHostUtilities::GLfloatFromJsRef(xRef), ScriptHostUtilities::GLfloatFromJsRef(yRef));

    return S_OK;
}

float2 transformPoint(const float3x2& mat, float2 pt)
{
    return float2(mat.m11 * pt.x + mat.m21 * pt.y + mat.m31, mat.m12 * pt.x + mat.m22 * pt.y + mat.m32);
}

void RenderContext2D::cleanup()
{
    if (m_session) {
        m_session = nullptr;
    }

    if (m_renderer) {
        m_renderer = nullptr;
    }

    if (m_device) {
        m_device = nullptr;
    }

    m_isInitialized = false;
}

HRESULT RenderContext2D::initialize()
{
    if (m_width == 0 || m_height == 0) {
        cleanup();
        return S_OK;
    }

    void* sharedDevicePtr;
    RETURN_IF_FAILED(getSharedDevice(&sharedDevicePtr));

    m_device =
        safe_cast<Microsoft::Graphics::Canvas::CanvasDevice ^>(reinterpret_cast<Platform::Object ^>(sharedDevicePtr));

    RETURN_IF_FAILED(getRenderTarget(sharedDevicePtr,
                                     static_cast<float>(m_width),
                                     static_cast<float>(m_height),
                                     m_dpi,
                                     m_nativePixelFormat,
                                     CanvasAlphaMode::Premultiplied,
                                     &m_rendererPtr));

    m_renderer = safe_cast<Microsoft::Graphics::Canvas::CanvasRenderTarget ^>(
        reinterpret_cast<Platform::Object ^>(m_rendererPtr));

    m_session = m_renderer->CreateDrawingSession();
    m_session->Blend = CanvasBlend::SourceOver;

    m_isInitialized = true;

    return S_OK;
}

JsValueRef RenderContext2D::clearRect(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    RETURN_INVALID_REF_IF_FALSE(m_isInitialized);

    Rect rect;
    RETURN_INVALID_REF_IF_FAILED(parseRect(arguments[2], rect));

    // Save the previous blend value
    const CanvasBlend blend = m_session->Blend;

    m_session->Blend = CanvasBlend::Copy;

    Color color;
    color.A = 0;
    color.R = 0;
    color.G = 0;
    color.B = 0;

    m_session->FillRectangle(rect, color);

    m_session->Blend = blend;

    RETURN_INVALID_REF_IF_FALSE(m_isInitialized);
    return JS_INVALID_REFERENCE;
}

JsValueRef RenderContext2D::fillRect(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);
    RETURN_INVALID_REF_IF_FALSE(m_isInitialized);

    Rect rect;
    RETURN_INVALID_REF_IF_FAILED(parseRect(arguments[2], rect));

    Color color;
    RETURN_INVALID_REF_IF_FAILED(parseColor(arguments[3], color));

    Color cm = color;
    cm.A = (unsigned char)(m_globalOpacity * cm.A);
    m_session->FillRectangle(rect, cm);

    return JS_INVALID_REFERENCE;
}

JsValueRef RenderContext2D::fillRectGradient(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);
    RETURN_INVALID_REF_IF_FALSE(m_isInitialized);

    Rect rect;
    RETURN_INVALID_REF_IF_FAILED(parseRect(arguments[2], rect));

    JsValueRef gradient = arguments[3];

    JsValueRef startRef;
    float2 start;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetJsProperty(gradient, L"start", &startRef));
    RETURN_INVALID_REF_IF_FAILED(parsePoint(startRef, start));

    JsValueRef endRef;
    float2 end;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetJsProperty(gradient, L"end", &endRef));
    RETURN_INVALID_REF_IF_FAILED(parsePoint(endRef, end));

    JsValueRef stopsRef;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetJsProperty(gradient, L"stops", &stopsRef));

    JsValueRef stopsLengthRef;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetJsProperty(stopsRef, L"length", &stopsLengthRef));
    const int stopsLength = ScriptHostUtilities::GLintFromJsRef(stopsLengthRef);

    auto stops = ref new Platform::Array<CanvasGradientStop>(stopsLength);
    for (int i = 0; i < stopsLength; i++) {
        JsValueRef stopRef;
        RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetJsProperty(stopsRef, std::to_wstring(i), &stopRef));

        JsValueRef positionRef;
        RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetJsProperty(stopRef, L"position", &positionRef));

        JsValueRef colorRef;
        RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetJsProperty(stopRef, L"color", &colorRef));

        stops[i].Position = static_cast<float>(ScriptHostUtilities::GLintFromJsRef(positionRef));
        RETURN_INVALID_REF_IF_FAILED(parseColor(colorRef, stops[i].Color));
    }

    auto brush = getCanvasLinearGradientBrush(m_rendererPtr, stops->Data, stopsLength);
    RETURN_INVALID_REF_IF_NULL(brush);

    brush->StartPoint = start;
    brush->EndPoint = end;
    brush->Opacity = m_globalOpacity;
    m_session->FillRectangle(rect, brush);

    return JS_INVALID_REFERENCE;
}

JsValueRef RenderContext2D::fillText(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 7);
    RETURN_INVALID_REF_IF_FALSE(m_isInitialized);

    std::wstring text;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetString(arguments[2], text));

    float2 point;
    RETURN_INVALID_REF_IF_FAILED(parsePoint(arguments[3], point));

    Color color;
    RETURN_INVALID_REF_IF_FAILED(parseColor(arguments[4], color));

    std::wstring fontFamily, fontWeight, fontStyle;

    JsValueRef sizeRef;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetJsProperty(arguments[5], L"size", &sizeRef));
    int fontSize = ScriptHostUtilities::GLintFromJsRef(sizeRef);

    JsValueRef familyRef;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetJsProperty(arguments[5], L"family", &familyRef));
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetString(familyRef, fontFamily));

    JsValueRef weightRef;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetJsProperty(arguments[5], L"weight", &weightRef));
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetString(weightRef, fontWeight));

    JsValueRef styleRef;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetJsProperty(arguments[5], L"style", &styleRef));
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetString(styleRef, fontStyle));

    const int alignment = ScriptHostUtilities::GLintFromJsRef(arguments[6]);

    auto format = getCanvasTextFormat();
    RETURN_INVALID_REF_IF_NULL(format);

    format->FontFamily = ref new Platform::String(fontFamily.c_str());
    if (fontWeight == L"bold") {
        format->FontWeight = Windows::UI::Text::FontWeights::Bold;
    }
    if (fontStyle == L"italic") {
        format->FontStyle = Windows::UI::Text::FontStyle::Italic;
    }
    format->FontSize = static_cast<float>(fontSize);
    if (alignment == 0) {
        format->HorizontalAlignment = CanvasHorizontalAlignment::Left;
    }
    if (alignment == 1) {
        format->HorizontalAlignment = CanvasHorizontalAlignment::Center;
    }
    if (alignment == 2) {
        format->HorizontalAlignment = CanvasHorizontalAlignment::Right;
    }
    format->VerticalAlignment = CanvasVerticalAlignment::Bottom;
    format->LineSpacingMode = CanvasLineSpacingMode::Proportional;
    format->LineSpacing = 1;

    Color cm = color;
    cm.A = (unsigned char)(m_globalOpacity * cm.A);
    m_session->DrawText(ref new Platform::String(text.c_str()), point, cm, format);

    return JS_INVALID_REFERENCE;
}

JsValueRef RenderContext2D::measureText(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);
    RETURN_INVALID_REF_IF_FALSE(m_isInitialized);

    std::wstring text;
    ScriptHostUtilities::GetString(arguments[2], text);

    const int fontSize = ScriptHostUtilities::GLintFromJsRef(arguments[3]);

    std::wstring fontFamily;
    ScriptHostUtilities::GetString(arguments[4], fontFamily);

    auto format = getCanvasTextFormat();
    RETURN_INVALID_REF_IF_NULL(format);

    format->FontFamily = ref new Platform::String(fontFamily.c_str());
    format->FontSize = static_cast<float>(fontSize);
    format->WordWrapping = Microsoft::Graphics::Canvas::Text::CanvasWordWrapping::NoWrap;
	CanvasTextLayout ^ layout = getCanvasTextLayout(m_session, text.c_str(), format, 0.0, 0.0);

    JsValueRef returnValue;
    RETURN_INVALID_REF_IF_JS_ERROR(JsDoubleToNumber(layout->DrawBounds.Width, &returnValue));
    return returnValue;
}

JsValueRef RenderContext2D::beginPath()
{
    RETURN_INVALID_REF_IF_FALSE(m_isInitialized);

    void* sessionUnknown = reinterpret_cast<IInspectable*>(m_session);
    void* pathBuilderUnknown;
    RETURN_INVALID_REF_IF_FAILED(getCanvasPathBuilder(sessionUnknown, &pathBuilderUnknown));
    m_pathBuilder = safe_cast<Geometry::CanvasPathBuilder ^>(reinterpret_cast<Platform::Object ^>(pathBuilderUnknown));
    m_figurePresent = false;

    return JS_INVALID_REFERENCE;
}

JsValueRef RenderContext2D::closePath()
{
    RETURN_INVALID_REF_IF_FALSE(m_isInitialized);

    if (m_pathBuilder != nullptr && m_figurePresent) {
        m_pathBuilder->EndFigure(Geometry::CanvasFigureLoop::Closed);
        m_figurePresent = false;
    }

    return JS_INVALID_REFERENCE;
}

JsValueRef RenderContext2D::getWidth(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(m_isInitialized);

    JsValueRef output;
    JsIntToNumber(static_cast<int>(m_width), &output);
    return output;
}

JsValueRef RenderContext2D::setWidth(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    int width = ScriptHostUtilities::GLintFromJsRef(arguments[2]);
    if (width != m_width) {
        m_width = width;
        if (m_height != 0) {
            RETURN_INVALID_REF_IF_FAILED(initialize());
        }
    }

    return JS_INVALID_REFERENCE;
}

JsValueRef RenderContext2D::getHeight(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(m_isInitialized);

    JsValueRef output;
    JsIntToNumber(m_height, &output);
    return output;
}

JsValueRef RenderContext2D::setHeight(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    int height = ScriptHostUtilities::GLintFromJsRef(arguments[2]);
    if (height != m_height) {
        m_height = height;
        RETURN_INVALID_REF_IF_FAILED(initialize());
    }

    return JS_INVALID_REFERENCE;
}

JsValueRef RenderContext2D::moveTo(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);
    RETURN_INVALID_REF_IF_FALSE(m_isInitialized);

    float2 point;
    RETURN_INVALID_REF_IF_FAILED(parsePoint(arguments[2], point));

    moveToInternal(point.x, point.y);

    return JS_INVALID_REFERENCE;
}

JsValueRef RenderContext2D::lineTo(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);
    RETURN_INVALID_REF_IF_FALSE(m_isInitialized);

    float2 point;
    RETURN_INVALID_REF_IF_FAILED(parsePoint(arguments[2], point));

    if (m_pathBuilder == nullptr) {
        beginPath();
    }

    if (!m_figurePresent) {
        moveToInternal(point.x, point.y);
    } else {
        m_pathBuilder->AddLine(transformPoint(m_session->Transform, float2(point.x, point.y)));
    }

    return JS_INVALID_REFERENCE;
}

JsValueRef RenderContext2D::bezierCurveTo(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);
    RETURN_INVALID_REF_IF_FALSE(m_isInitialized);

    float2 c1;
    RETURN_INVALID_REF_IF_FAILED(parsePoint(arguments[2], c1));

    float2 c2;
    RETURN_INVALID_REF_IF_FAILED(parsePoint(arguments[3], c2));

    float2 point;
    RETURN_INVALID_REF_IF_FAILED(parsePoint(arguments[4], point));

    if (m_pathBuilder == nullptr) {
        beginPath();
    }

    if (!m_figurePresent) {
        moveToInternal(point.x, point.y);
    } else {
        m_pathBuilder->AddCubicBezier(transformPoint(m_session->Transform, float2(c1.x, c1.y)),
                                      transformPoint(m_session->Transform, float2(c2.x, c2.y)),
                                      transformPoint(m_session->Transform, float2(point.x, point.y)));
    }

    return JS_INVALID_REFERENCE;
}

JsValueRef RenderContext2D::quadraticCurveTo(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);
    RETURN_INVALID_REF_IF_FALSE(m_isInitialized);

    float2 c1;
    RETURN_INVALID_REF_IF_FAILED(parsePoint(arguments[2], c1));

    float2 point;
    RETURN_INVALID_REF_IF_FAILED(parsePoint(arguments[3], point));

    if (m_pathBuilder == nullptr) {
        beginPath();
    }
    if (!m_figurePresent) {
        moveToInternal(point.x, point.y);
    } else {
        m_pathBuilder->AddQuadraticBezier(transformPoint(m_session->Transform, float2(c1.x, c1.y)),
                                          transformPoint(m_session->Transform, float2(point.x, point.y)));
    }

    return JS_INVALID_REFERENCE;
}

JsValueRef RenderContext2D::arc(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 7);
    RETURN_INVALID_REF_IF_FALSE(m_isInitialized);

    float2 c;
    RETURN_INVALID_REF_IF_FAILED(parsePoint(arguments[2], c));

    const float r = ScriptHostUtilities::GLfloatFromJsRef(arguments[3]);
    const float startAngle = ScriptHostUtilities::GLfloatFromJsRef(arguments[4]);
    const float endAngle = ScriptHostUtilities::GLfloatFromJsRef(arguments[5]);
    const int counterCW = ScriptHostUtilities::GLintFromJsRef(arguments[6]);

    if (m_pathBuilder == nullptr) {
        beginPath();
    }
    if (!m_figurePresent) {
        moveToInternal(c.x, c.y);
    }

    const float tr = r * m_session->Transform.m11;
    m_pathBuilder->AddArc(
        transformPoint(m_session->Transform, float2(c.x, c.y)), tr, tr, startAngle, endAngle - startAngle);

    return JS_INVALID_REFERENCE;
}

JsValueRef RenderContext2D::fill(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);
    RETURN_INVALID_REF_IF_FALSE(m_isInitialized);

    Color color;
    RETURN_INVALID_REF_IF_FAILED(parseColor(arguments[2], color));

    if (m_pathBuilder == nullptr) {
        return JS_INVALID_REFERENCE;
    }

    if (m_figurePresent) {
        m_pathBuilder->EndFigure(Geometry::CanvasFigureLoop::Open);
        m_figurePresent = false;
    }

    float3x2 tmp = m_session->Transform;
    m_session->Transform = float3x2::identity();

    Color cm = color;
    cm.A = (unsigned char)(m_globalOpacity * cm.A);

    Geometry::CanvasGeometry ^ geometry = canvasGeometryCreatePath(m_pathBuilder);
	RETURN_INVALID_REF_IF_NULL(geometry);

    m_session->FillGeometry(geometry, cm);

    m_session->Transform = tmp;

    beginPath();
    m_pathBuilder->AddGeometry(geometry);

    return JS_INVALID_REFERENCE;
}

JsValueRef RenderContext2D::fillGradient(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);
    RETURN_INVALID_REF_IF_FALSE(m_isInitialized);

    JsValueRef gradient = arguments[2];

    JsValueRef startRef;
    float2 start;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetJsProperty(gradient, L"start", &startRef));
    RETURN_INVALID_REF_IF_FAILED(parsePoint(startRef, start));

    JsValueRef endRef;
    float2 end;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetJsProperty(gradient, L"end", &endRef));
    RETURN_INVALID_REF_IF_FAILED(parsePoint(endRef, end));

    JsValueRef stopsRef;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetJsProperty(gradient, L"stops", &stopsRef));

    JsValueRef stopsLengthRef;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetJsProperty(stopsRef, L"length", &stopsLengthRef));
    const int stopsLength = ScriptHostUtilities::GLintFromJsRef(stopsLengthRef);

    auto stops = ref new Platform::Array<CanvasGradientStop>(stopsLength);
    for (int i = 0; i < stopsLength; i++) {
        JsValueRef stopRef;
        RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetJsProperty(stopsRef, std::to_wstring(i), &stopRef));

        JsValueRef positionRef;
        RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetJsProperty(stopRef, L"position", &positionRef));

        JsValueRef colorRef;
        RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetJsProperty(stopRef, L"color", &colorRef));

        stops[i].Position = static_cast<float>(ScriptHostUtilities::GLintFromJsRef(positionRef));
        RETURN_INVALID_REF_IF_FAILED(parseColor(colorRef, stops[i].Color));
    }

    if (m_pathBuilder == nullptr) {
        return JS_INVALID_REFERENCE;
    }

    if (m_figurePresent) {
        m_pathBuilder->EndFigure(Geometry::CanvasFigureLoop::Open);
        m_figurePresent = false;
    }

    float3x2 tmp = m_session->Transform;
    m_session->Transform = float3x2::identity();

    Geometry::CanvasGeometry ^ geometry = canvasGeometryCreatePath(m_pathBuilder);
	RETURN_INVALID_REF_IF_NULL(geometry);

    auto brush = getCanvasLinearGradientBrush(m_rendererPtr, stops->Data, stopsLength);
    RETURN_INVALID_REF_IF_NULL(brush);
    brush->StartPoint = start;
    brush->EndPoint = end;
    m_session->FillGeometry(geometry, brush);

    m_session->Transform = tmp;

    beginPath();
    m_pathBuilder->AddGeometry(geometry);

    return JS_INVALID_REFERENCE;
}

JsValueRef RenderContext2D::stroke(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);
    RETURN_INVALID_REF_IF_FALSE(m_isInitialized);

    Color color;
    RETURN_INVALID_REF_IF_FAILED(parseColor(arguments[2], color));

    if (m_pathBuilder == nullptr) {
        return JS_INVALID_REFERENCE;
    }

    if (m_figurePresent) {
        m_pathBuilder->EndFigure(Geometry::CanvasFigureLoop::Open);
        m_figurePresent = false;
    }

    float3x2 tmp = m_session->Transform;
    m_session->Transform = float3x2::identity();

    Geometry::CanvasGeometry ^ geometry = canvasGeometryCreatePath(m_pathBuilder);
	RETURN_INVALID_REF_IF_NULL(geometry);

    Geometry::CanvasStrokeStyle ^ style = getCanvasStrokeStyle();
	RETURN_INVALID_REF_IF_NULL(style);
    style->StartCap = m_capStyle;
    style->EndCap = m_capStyle;
    style->LineJoin = m_joinStyle;

    Color cm = color;
    cm.A = (unsigned char)(m_globalOpacity * cm.A);

    m_session->DrawGeometry(geometry, cm, m_lineWidth * tmp.m11, style);

    m_session->Transform = tmp;

    beginPath();
    m_pathBuilder->AddGeometry(geometry);

    return JS_INVALID_REFERENCE;
}

JsValueRef RenderContext2D::strokeGradient(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);
    RETURN_INVALID_REF_IF_FALSE(m_isInitialized);

    JsValueRef gradient = arguments[2];

    JsValueRef startRef;
    float2 start;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetJsProperty(gradient, L"start", &startRef));
    RETURN_INVALID_REF_IF_FAILED(parsePoint(startRef, start));

    JsValueRef endRef;
    float2 end;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetJsProperty(gradient, L"end", &endRef));
    RETURN_INVALID_REF_IF_FAILED(parsePoint(endRef, end));

    JsValueRef stopsRef;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetJsProperty(gradient, L"stops", &stopsRef));

    JsValueRef stopsLengthRef;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetJsProperty(stopsRef, L"length", &stopsLengthRef));
    const int stopsLength = ScriptHostUtilities::GLintFromJsRef(stopsLengthRef);

    auto stops = ref new Platform::Array<CanvasGradientStop>(stopsLength);
    for (int i = 0; i < stopsLength; i++) {
        JsValueRef stopRef;
        RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetJsProperty(stopsRef, std::to_wstring(i), &stopRef));

        JsValueRef positionRef;
        RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetJsProperty(stopRef, L"position", &positionRef));

        JsValueRef colorRef;
        RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetJsProperty(stopRef, L"color", &colorRef));

        stops[i].Position = static_cast<float>(ScriptHostUtilities::GLintFromJsRef(positionRef));
        RETURN_INVALID_REF_IF_FAILED(parseColor(colorRef, stops[i].Color));
    }

    if (m_pathBuilder == nullptr) {
        return JS_INVALID_REFERENCE;
    }

    if (m_figurePresent) {
        m_pathBuilder->EndFigure(Geometry::CanvasFigureLoop::Open);
        m_figurePresent = false;
    }

    float3x2 tmp = m_session->Transform;
    m_session->Transform = float3x2::identity();

    Geometry::CanvasGeometry ^ geometry = canvasGeometryCreatePath(m_pathBuilder);
	RETURN_INVALID_REF_IF_NULL(geometry);

    auto brush = getCanvasLinearGradientBrush(m_rendererPtr, stops->Data, stopsLength);
    RETURN_INVALID_REF_IF_NULL(brush);

    brush->StartPoint = start;
    brush->EndPoint = end;
    m_session->DrawGeometry(geometry, brush);

    m_session->Transform = tmp;

    beginPath();
    m_pathBuilder->AddGeometry(geometry);

    return JS_INVALID_REFERENCE;
}

JsValueRef RenderContext2D::setTransform(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 8);
    RETURN_INVALID_REF_IF_FALSE(m_isInitialized);

    const float a = ScriptHostUtilities::GLfloatFromJsRef(arguments[2]);
    const float b = ScriptHostUtilities::GLfloatFromJsRef(arguments[3]);
    const float c = ScriptHostUtilities::GLfloatFromJsRef(arguments[4]);
    const float d = ScriptHostUtilities::GLfloatFromJsRef(arguments[5]);
    const float e = ScriptHostUtilities::GLfloatFromJsRef(arguments[6]);
    const float f = ScriptHostUtilities::GLfloatFromJsRef(arguments[7]);

    m_session->Transform = float3x2(a, b, c, d, e, f);

    return JS_INVALID_REFERENCE;
}

JsValueRef RenderContext2D::setLineStyle(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);
    RETURN_INVALID_REF_IF_FALSE(m_isInitialized);

    m_lineWidth = ScriptHostUtilities::GLfloatFromJsRef(arguments[2]);

    // 0 -> Flat, 1 -> Round, 2 -> Square
    m_capStyle = static_cast<Geometry::CanvasCapStyle>(ScriptHostUtilities::GLintFromJsRef(arguments[3]));

    // 1 -> Bevel, 0 -> Miter, 2 -> Round
    m_joinStyle = static_cast<Geometry::CanvasLineJoin>(ScriptHostUtilities::GLintFromJsRef(arguments[4]));

    return JS_INVALID_REFERENCE;
}

JsValueRef RenderContext2D::setGlobalOpacity(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);
    RETURN_INVALID_REF_IF_FALSE(m_isInitialized);

    m_globalOpacity = ScriptHostUtilities::GLfloatFromJsRef(arguments[2]);

    return JS_INVALID_REFERENCE;
}

void RenderContext2D::moveToInternal(float x, float y)
{
    if (m_pathBuilder == nullptr) {
        beginPath();
    }

    if (m_figurePresent) {
        m_pathBuilder->EndFigure(Geometry::CanvasFigureLoop::Open);
    }

    m_pathBuilder->BeginFigure(transformPoint(m_session->Transform, float2(x, y)));
    m_figurePresent = true;
}

JsValueRef RenderContext2D::getImageData(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    RETURN_INVALID_REF_IF_FALSE(m_isInitialized);

    m_session->Flush();

    Rect rect;
    RETURN_INVALID_REF_IF_FAILED(parseRect(arguments[2], rect));

    RETURN_INVALID_REF_IF_TRUE(rect.Width == 0 || rect.Height == 0);

    unsigned int canvasStride;
    vector<unsigned char> canvasPixelData;
    RETURN_INVALID_REF_IF_FAILED(getImageDataNative(static_cast<unsigned int>(rect.X),
                                                    static_cast<unsigned int>(rect.Y),
                                                    static_cast<unsigned int>(rect.Width),
                                                    static_cast<unsigned int>(rect.Height),
                                                    canvasStride,
                                                    canvasPixelData));

    JsValueRef jsArray;
    RETURN_INVALID_REF_IF_JS_ERROR(JsCreateTypedArray(JsTypedArrayType::JsArrayTypeUint8Clamped,
                                                      nullptr,
                                                      0,
                                                      static_cast<unsigned int>(canvasPixelData.size()),
                                                      &jsArray));

    // Get a pointer to the newly allocated JS array and copy the pixels there
    ChakraBytePtr jsArrayPointer;
    JsTypedArrayType jsArrayType;
    unsigned int jsArrayLength;
    int jsArrayElementSize;

    RETURN_INVALID_REF_IF_JS_ERROR(
        JsGetTypedArrayStorage(jsArray, &jsArrayPointer, &jsArrayLength, &jsArrayType, &jsArrayElementSize));

    CopyMemory(jsArrayPointer, canvasPixelData.data(), canvasPixelData.size());

    return jsArray;
}

JsValueRef RenderContext2D::toDataUrl(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3 || argumentCount == 4);
    RETURN_INVALID_REF_IF_FALSE(m_isInitialized);

    wstring encodingTypeString;
    RETURN_INVALID_REF_IF_FALSE(ScriptHostUtilities::GetString(arguments[2], encodingTypeString));
    const auto encodingType = getEncodingFromMimeType(encodingTypeString);

    // Only PNG and Jpeg are supported
    RETURN_INVALID_REF_IF_TRUE(encodingType == EncodingType::Unknown);

    float encoderOptions = 0.92f;
    if (argumentCount == 4) {
        JsValueType encoderOptionsType;
        RETURN_INVALID_REF_IF_JS_ERROR(JsGetValueType(arguments[3], &encoderOptionsType));

        if (encoderOptionsType != JsUndefined) {
            encoderOptions = ScriptHostUtilities::GLfloatFromJsRef(arguments[3]);
        }
    }

    m_session->Flush();

    // Convert from 32bpp RGBA to 24bpp BGR
    std::vector<byte> bgrPixels;
    if (encodingType == EncodingType::JPEG) {
        getImageDataBGRFlipY(bgrPixels);
    } else {
        getImageDataBGRAUnPremultiplyFlipY(bgrPixels);
    }

    ComPtr<IWICImagingFactory> imagingFactory = NULL;
    RETURN_INVALID_REF_IF_FAILED(CoCreateInstance(CLSID_WICImagingFactory,
                                                  NULL,
                                                  CLSCTX_INPROC_SERVER,
                                                  IID_IWICImagingFactory,
                                                  (LPVOID*)imagingFactory.ReleaseAndGetAddressOf()));

    // Create a memory stream to hold the encoded image, then wrap a WIC stream around it
    ComPtr<IStream> memoryStream;
    RETURN_INVALID_REF_IF_FAILED(
        ::CreateStreamOnHGlobal(nullptr, true /*delete on release*/, memoryStream.ReleaseAndGetAddressOf()));
    ComPtr<IWICStream> imageStream;
    RETURN_INVALID_REF_IF_FAILED(imagingFactory->CreateStream(imageStream.ReleaseAndGetAddressOf()));
    RETURN_INVALID_REF_IF_FAILED(imageStream->InitializeFromIStream(memoryStream.Get()));

    // Create the encoder corresponding to the requested type
    ComPtr<IWICBitmapEncoder> encoder = NULL;
    RETURN_INVALID_REF_IF_FAILED(imagingFactory->CreateEncoder(
        encodingType == EncodingType::JPEG ? GUID_ContainerFormatJpeg : GUID_ContainerFormatPng,
        NULL,
        encoder.ReleaseAndGetAddressOf()));

    RETURN_INVALID_REF_IF_FAILED(encoder->Initialize(imageStream.Get(), WICBitmapEncoderNoCache));

    ComPtr<IWICBitmapFrameEncode> bitmapFrame = NULL;
    ComPtr<IPropertyBag2> propertyBag = NULL;
    RETURN_INVALID_REF_IF_FAILED(
        encoder->CreateNewFrame(bitmapFrame.ReleaseAndGetAddressOf(), propertyBag.ReleaseAndGetAddressOf()));

    RETURN_INVALID_REF_IF_FAILED(initializeEncodingPropertyBag(propertyBag.Get(), encodingType, encoderOptions));

    // Initialize the encoder
    RETURN_INVALID_REF_IF_FAILED(bitmapFrame->Initialize(propertyBag.Get()));
    RETURN_INVALID_REF_IF_FAILED(bitmapFrame->SetSize(static_cast<unsigned int>(m_renderer->Size.Width),
                                                      static_cast<unsigned int>(m_renderer->Size.Height)));

    // Set the input format to the encoder and make sure the format is acceptable
    if (encodingType == EncodingType::JPEG) {
        WICPixelFormatGUID formatGUID = GUID_WICPixelFormat24bppBGR;
        RETURN_INVALID_REF_IF_FAILED(bitmapFrame->SetPixelFormat(&formatGUID));
        RETURN_INVALID_REF_IF_FALSE(IsEqualGUID(formatGUID, GUID_WICPixelFormat24bppBGR));
    } else {  // PNG
        WICPixelFormatGUID formatGUID = GUID_WICPixelFormat32bppBGRA;
        RETURN_INVALID_REF_IF_FAILED(bitmapFrame->SetPixelFormat(&formatGUID));
        RETURN_INVALID_REF_IF_FALSE(IsEqualGUID(formatGUID, GUID_WICPixelFormat32bppBGRA));
    }

    // Write the canvas pixels to the encoder
    auto height = static_cast<unsigned int>(m_renderer->Size.Height);
    auto stride = static_cast<unsigned int>(bgrPixels.size() / m_renderer->Size.Height);
    RETURN_INVALID_REF_IF_FAILED(
        bitmapFrame->WritePixels(height, stride, static_cast<unsigned int>(bgrPixels.size()), bgrPixels.data()));

    // Finalize the encoding operation
    RETURN_INVALID_REF_IF_FAILED(bitmapFrame->Commit());
    RETURN_INVALID_REF_IF_FAILED(encoder->Commit());

    // Get the encoded memory block from the stream
    // TODO: when we can use GlobalLock/Unlock (RS2+), access the memoryStream directly
    vector<byte> encodedMemoryBlock;
    RETURN_INVALID_REF_IF_FAILED(getDataFromStream(imagingFactory.Get(), memoryStream.Get(), encodedMemoryBlock));

    std::wstring encodedImage;
    RETURN_INVALID_REF_IF_FAILED(getDataUrlFromEncodedImage(encodedMemoryBlock, encodingTypeString, encodedImage));

    JsValueRef result;
    RETURN_INVALID_REF_IF_JS_ERROR(JsPointerToString(encodedImage.c_str(), encodedImage.length(), &result));

    return result;
}

JsValueRef RenderContext2D::drawImage(HoloJs::IIMage* image, JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);
    RETURN_INVALID_REF_IF_FALSE(m_isInitialized);

    Rect srcRect;
    RETURN_INVALID_REF_IF_FAILED(parseRect(arguments[3], srcRect));

    Rect destRect;
    RETURN_INVALID_REF_IF_FAILED(parseRect(arguments[4], destRect));

    unsigned int imageBufferSize = 0;
    WICInProcPointer imageMemory = nullptr;
    unsigned int stride;

    HoloJs::IMAGE_FORMAT_GUID imageFormat;
    RETURN_INVALID_REF_IF_FALSE(sizeof(GUID) == sizeof(HoloJs::IMAGE_FORMAT_GUID));
    memcpy(&imageFormat, &GUID_WICPixelFormat32bppRGBA, sizeof(imageFormat));

    RETURN_INVALID_REF_IF_FAILED(
        image->getImageData(imageFormat, &imageMemory, imageBufferSize, stride, HoloJs::ImageFlipRotation::None));

    void* canvasBitmapPtr;
    RETURN_INVALID_REF_IF_FAILED(createCanvasBitmapFromBytes(m_rendererPtr,
                                                             imageBufferSize,
                                                             reinterpret_cast<unsigned char*>(imageMemory),
                                                             static_cast<int>(image->getWidth()),
                                                             static_cast<int>(image->getHeight()),
                                                             m_nativePixelFormat,
                                                             &canvasBitmapPtr));
    auto canvasBitmap = safe_cast<CanvasBitmap ^>(reinterpret_cast<Platform::Object ^>(canvasBitmapPtr));

    m_session->DrawImage(canvasBitmap, destRect, srcRect, m_globalOpacity);

    return JS_INVALID_REFERENCE;
}

void RenderContext2D::getImageDataBGRAUnPremultiplyFlipY(vector<byte>& bgrPixels)
{
    // Get the raw pixels from the underlying canvas
    auto canvasPixels = m_renderer->GetPixelBytes();
    auto canvasPixelsLength = canvasPixels->Length;
    auto canvasPixelData = canvasPixels->begin();

    const int dest_bpp = 4;
    bgrPixels.resize((canvasPixelsLength * dest_bpp) / m_bpp);

    // Convert from 32bpp RGBA to 24bpp BGR and flip vertical
    for (unsigned int row_index = 0; row_index < m_height; row_index++) {
        for (unsigned int column_index = 0; column_index < m_width; column_index++) {
            const auto destination_row_offset = row_index * m_width * dest_bpp;
            const auto source_row_offset = (m_height - row_index - 1) * m_width * m_bpp;

            unsigned char alpha = canvasPixelData[source_row_offset + column_index * m_bpp + 3];
            const unsigned char s = alpha > 0 ? static_cast<unsigned char>(255.0f / alpha) : 0;

            bgrPixels[destination_row_offset + column_index * dest_bpp + 0] =
                canvasPixelData[source_row_offset + column_index * m_bpp + 2] * s;
            bgrPixels[destination_row_offset + column_index * dest_bpp + 1] =
                canvasPixelData[source_row_offset + column_index * m_bpp + 1] * s;
            bgrPixels[destination_row_offset + column_index * dest_bpp + 2] =
                canvasPixelData[source_row_offset + column_index * m_bpp + 0] * s;
            bgrPixels[destination_row_offset + column_index * dest_bpp + 3] = alpha;
        }
    }
}

void RenderContext2D::getImageDataBGRFlipY(vector<byte>& bgrPixels)
{
    // Get the raw pixels from the underlying canvas
    auto canvasPixels = m_renderer->GetPixelBytes();
    auto canvasPixelsLength = canvasPixels->Length;
    auto canvasPixelData = canvasPixels->begin();

    const int dest_bpp = 3;
    bgrPixels.resize((canvasPixelsLength * dest_bpp) / m_bpp);

    // Convert from 32bpp RGBA to 24bpp BGR and flip vertical
    for (unsigned int row_index = 0; row_index < m_height; row_index++) {
        for (unsigned int column_index = 0; column_index < m_width; column_index++) {
            const auto destination_row_offset = row_index * m_width * dest_bpp;
            const auto source_row_offset = (m_height - row_index - 1) * m_width * m_bpp;

            bgrPixels[destination_row_offset + column_index * dest_bpp + 0] =
                canvasPixelData[source_row_offset + column_index * m_bpp + 2];
            bgrPixels[destination_row_offset + column_index * dest_bpp + 1] =
                canvasPixelData[source_row_offset + column_index * m_bpp + 1];
            bgrPixels[destination_row_offset + column_index * dest_bpp + 2] =
                canvasPixelData[source_row_offset + column_index * m_bpp + 0];
        }
    }
}

EncodingType RenderContext2D::getEncodingFromMimeType(const wstring& type)
{
    if (_wcsicmp(type.c_str(), L"image/png") == 0) {
        return EncodingType::PNG;
    } else if (_wcsicmp(type.c_str(), L"image/jpeg") == 0) {
        return EncodingType::JPEG;
    } else {
        return EncodingType::Unknown;
    }
}

HRESULT RenderContext2D::getDataFromStream(IWICImagingFactory* imagingFactory, IStream* stream, vector<byte>& data)
{
    // Figure out long is the encoded stream
    LARGE_INTEGER seekSize;
    seekSize.QuadPart = 0;
    ULARGE_INTEGER streamLengthLongLong;
    RETURN_IF_FAILED(stream->Seek(seekSize, STREAM_SEEK_CUR, &streamLengthLongLong));

    RETURN_IF_TRUE(streamLengthLongLong.QuadPart > MAXUINT32);
    const unsigned int streamLengthUInt = static_cast<unsigned int>(streamLengthLongLong.QuadPart);

    // Allocate a memory block to copy the encoded image stream to
    data.resize(streamLengthUInt);

    // Create a WIC stream over the memory block
    ComPtr<IWICStream> byteAccessWicStream = NULL;
    RETURN_IF_FAILED(imagingFactory->CreateStream(byteAccessWicStream.ReleaseAndGetAddressOf()));
    RETURN_IF_FAILED(byteAccessWicStream->InitializeFromMemory(data.data(), streamLengthUInt));

    // Get the vanilla stream from the WIC stream
    ComPtr<IStream> byteAccessStream;
    RETURN_IF_FAILED(byteAccessWicStream.As(&byteAccessStream));

    // Copy the encoded image stream to the byte accessible stream
    RETURN_IF_FAILED(stream->Seek(seekSize, STREAM_SEEK_SET, nullptr));
    RETURN_IF_FAILED(stream->CopyTo(byteAccessStream.Get(), streamLengthLongLong, nullptr, nullptr));

    return S_OK;
}

HRESULT RenderContext2D::getDataUrlFromEncodedImage(vector<byte>& imageData,
                                                    const wstring& mimeType,
                                                    wstring& encodedImage)
{
    // Create an IBuffer over the image memory block
    Microsoft::WRL::ComPtr<HoloJs::BufferOnMemory> imageBuffer;
    Details::MakeAndInitialize<HoloJs::BufferOnMemory>(
        &imageBuffer, imageData.data(), static_cast<unsigned int>(imageData.size()));
    auto iinspectable = (IInspectable*)reinterpret_cast<IInspectable*>(imageBuffer.Get());
    IBuffer ^ imageIBuffer = reinterpret_cast<IBuffer ^>(iinspectable);

    // base64 encode the resulting image
    auto encodedImagePlatString = CryptographicBuffer::EncodeToBase64String(imageIBuffer);

    // Create the dataURL
    encodedImage.reserve(encodedImagePlatString->Length() + 128);
    encodedImage.assign(L"data:");
    encodedImage.append(mimeType);
    encodedImage.append(L";base64,");
    encodedImage.append(encodedImagePlatString->Data());

    return S_OK;
}

HRESULT RenderContext2D::initializeEncodingPropertyBag(IPropertyBag2* propertyBag,
                                                       EncodingType encodingType,
                                                       float encoderOptions)
{
    // If encoding to JPEG, set the image quality to what the caller requested;
    // PNG does not support image quality settings
    if (encodingType == EncodingType::JPEG) {
        PROPBAG2 qualityOption = {0};
        wchar_t imageQualityOptionName[] = L"ImageQuality";
        qualityOption.pstrName = imageQualityOptionName;
        VARIANT qualityValue;
        VariantInit(&qualityValue);
        qualityValue.vt = VT_R4;
        qualityValue.fltVal = encoderOptions;
        RETURN_IF_FAILED(propertyBag->Write(1, &qualityOption, &qualityValue));
    }

    return S_OK;
}

HRESULT RenderContext2D::getImageDataNative(unsigned int x,
                                            unsigned int y,
                                            unsigned int width,
                                            unsigned int height,
                                            unsigned int& stride,
                                            std::vector<unsigned char>& data)
{
    RETURN_IF_FALSE(m_isInitialized);

    m_session->Flush();

    stride = static_cast<unsigned int>(width) * m_bpp;
    auto pixels = m_renderer->GetPixelBytes(x, y, width, height);

    // Un-premultiply alpha
    auto pixelsPointer = pixels->Data;
    for (unsigned int row_index = 0; row_index < m_height; row_index++) {
        const auto rowOffset = row_index * stride;
        for (unsigned int column_offset = 0; column_offset < stride; column_offset += m_bpp) {
            const auto ROffset = rowOffset + column_offset;
            const auto GOffset = rowOffset + column_offset + 1;
            const auto BOffset = rowOffset + column_offset + 2;
            const auto AOffset = rowOffset + column_offset + 3;

            auto alpha = pixelsPointer[AOffset] > 0 ? 255.0 / pixelsPointer[AOffset] : 0.0;
            pixelsPointer[ROffset] = (unsigned char)(alpha * pixelsPointer[ROffset]);
            pixelsPointer[GOffset] = (unsigned char)(alpha * pixelsPointer[GOffset]);
            pixelsPointer[BOffset] = (unsigned char)(alpha * pixelsPointer[BOffset]);
        }
    }

    data.resize(pixels->Length);

    CopyMemory(data.data(), pixels->begin(), pixels->Length);

    return S_OK;
}