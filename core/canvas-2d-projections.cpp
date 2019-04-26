#include "canvas-2d-projections.h"
#include "include/holojs/private/canvas-render-context-2d.h"
#include "include/holojs/private/error-handling.h"
#include "include/holojs/private/platform-interfaces.h"
#include "include/holojs/private/script-host-utilities.h"
#include "resource-management/resource-manager.h"

using namespace HoloJs;
using namespace HoloJs::ResourceManagement;
using namespace std;

long CanvasProjections::initialize()
{
    // WebGL context projections
    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"createContext2D", L"canvas2d", staticCreateContext2D, this, &m_createContext2D));

    // Canvas 2D context projections
    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"drawImage", L"canvas2d", staticDrawImage, this, &m_drawImage));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"clearRect", L"canvas2d", staticclearRect, this, &m_clearRect));
    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(L"fillRect", L"canvas2d", staticfillRect, this, &m_fillRect));
    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"fillRectGradient", L"canvas2d", staticfillRectGradient, this, &m_fillGradient));
    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"fillText", L"canvas2d", staticfillText, this, &m_fillText));
    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"measureText", L"canvas2d", staticmeasureText, this, &m_measureText));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"beginPath", L"canvas2d", staticbeginPath, this, &m_beginPath));
    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"closePath", L"canvas2d", staticclosePath, this, &m_closePath));
    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"moveTo", L"canvas2d", staticmoveTo, this, &m_moveTo));
    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"lineTo", L"canvas2d", staticlineTo, this, &m_lineTo));
    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"bezierCurveTo", L"canvas2d", staticbezierCurveTo, this, &m_bezierCurveTo));
    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"quadraticCurveTo", L"canvas2d", staticquadraticCurveTo, this, &m_quadraticCurveTo));
    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(L"arc", L"canvas2d", staticarc, this, &m_arc));
    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(L"fill", L"canvas2d", staticfill, this, &m_fill));
    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"fillGradient", L"canvas2d", staticfillGradient, this, &m_fillGradient));
    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"stroke", L"canvas2d", staticstroke, this, &m_stroke));
    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"strokeGradient", L"canvas2d", staticstrokeGradient, this, &m_strokeGradient));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"setLineStyle", L"canvas2d", staticsetLineStyle, this, &m_setLineStyle));
    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"setGlobalOpacity", L"canvas2d", staticsetGlobalOpacity, this, &m_setGlobalOpacity));
    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"setTransform", L"canvas2d", staticsetTransform, this, &m_setTransform));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"getImageData", L"canvas2d", staticgetImageData, this, &m_getImageData));
    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"setWidth", L"canvas2d", staticsetWidth, this, &m_setWidth));
    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"setHeight", L"canvas2d", staticsetHeight, this, &m_setHeight));
    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"getWidth", L"canvas2d", staticgetWidth, this, &m_getWidth));
    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"getHeight", L"canvas2d", staticgetHeight, this, &m_getHeight));
    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"toDataURL", L"canvas2d", statictoDataUrl, this, &m_toDataUrl));

    return true;
}

JsValueRef CanvasProjections::createContext2D(JsValueRef* arguments, unsigned short argumentCount)
{
    return m_resourceManager->objectToDirectExternal(getPlatform()->createCanvasRenderContext2D(), ObjectType::ICanvasRenderContext2D);
}

JsValueRef CanvasProjections::drawImage(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

    ICanvasRenderContext2D* context =
        m_resourceManager->externalToObject<ICanvasRenderContext2D>(arguments[1], ObjectType::ICanvasRenderContext2D);
    RETURN_INVALID_REF_IF_NULL(context);

    auto imageElement = m_resourceManager->externalToObject<HoloJs::IIMage>(arguments[2], ObjectType::IImage);
    RETURN_INVALID_REF_IF_NULL(imageElement);

    return context->drawImage(imageElement, arguments, argumentCount);
}

JsValueRef CanvasProjections::clearRect(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    ICanvasRenderContext2D* context =
        m_resourceManager->externalToObject<ICanvasRenderContext2D>(arguments[1], ObjectType::ICanvasRenderContext2D);
    RETURN_INVALID_REF_IF_NULL(context);

    return context->clearRect(arguments, argumentCount);
}

JsValueRef CanvasProjections::fillRect(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    ICanvasRenderContext2D* context =
        m_resourceManager->externalToObject<ICanvasRenderContext2D>(arguments[1], ObjectType::ICanvasRenderContext2D);
    RETURN_INVALID_REF_IF_NULL(context);

    return context->fillRect(arguments, argumentCount);
}

JsValueRef CanvasProjections::fillRectGradient(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    ICanvasRenderContext2D* context =
        m_resourceManager->externalToObject<ICanvasRenderContext2D>(arguments[1], ObjectType::ICanvasRenderContext2D);
    RETURN_INVALID_REF_IF_NULL(context);

    return context->fillRectGradient(arguments, argumentCount);
}

JsValueRef CanvasProjections::fillText(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 7);

    ICanvasRenderContext2D* context =
        m_resourceManager->externalToObject<ICanvasRenderContext2D>(arguments[1], ObjectType::ICanvasRenderContext2D);
    RETURN_INVALID_REF_IF_NULL(context);
    return context->fillText(arguments, argumentCount);
}

JsValueRef CanvasProjections::measureText(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

    ICanvasRenderContext2D* context =
        m_resourceManager->externalToObject<ICanvasRenderContext2D>(arguments[1], ObjectType::ICanvasRenderContext2D);
    RETURN_INVALID_REF_IF_NULL(context);
    return context->measureText(arguments, argumentCount);
}

JsValueRef CanvasProjections::beginPath(JsValueRef* arguments, unsigned short argumentCount)
{
    ICanvasRenderContext2D* context =
        m_resourceManager->externalToObject<ICanvasRenderContext2D>(arguments[1], ObjectType::ICanvasRenderContext2D);
    RETURN_INVALID_REF_IF_NULL(context);
    return context->beginPath();
}

JsValueRef CanvasProjections::closePath(JsValueRef* arguments, unsigned short argumentCount)
{
    ICanvasRenderContext2D* context =
        m_resourceManager->externalToObject<ICanvasRenderContext2D>(arguments[1], ObjectType::ICanvasRenderContext2D);
    RETURN_INVALID_REF_IF_NULL(context);
    return context->closePath();
}

JsValueRef CanvasProjections::moveTo(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    ICanvasRenderContext2D* context =
        m_resourceManager->externalToObject<ICanvasRenderContext2D>(arguments[1], ObjectType::ICanvasRenderContext2D);
    RETURN_INVALID_REF_IF_NULL(context);
    return context->moveTo(arguments, argumentCount);
}

JsValueRef CanvasProjections::lineTo(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    ICanvasRenderContext2D* context =
        m_resourceManager->externalToObject<ICanvasRenderContext2D>(arguments[1], ObjectType::ICanvasRenderContext2D);
    RETURN_INVALID_REF_IF_NULL(context);
    return context->lineTo(arguments, argumentCount);
}

JsValueRef CanvasProjections::bezierCurveTo(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

    ICanvasRenderContext2D* context =
        m_resourceManager->externalToObject<ICanvasRenderContext2D>(arguments[1], ObjectType::ICanvasRenderContext2D);
    RETURN_INVALID_REF_IF_NULL(context);
    return context->bezierCurveTo(arguments, argumentCount);
}

JsValueRef CanvasProjections::quadraticCurveTo(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    ICanvasRenderContext2D* context =
        m_resourceManager->externalToObject<ICanvasRenderContext2D>(arguments[1], ObjectType::ICanvasRenderContext2D);
    RETURN_INVALID_REF_IF_NULL(context);
    return context->quadraticCurveTo(arguments, argumentCount);
}

JsValueRef CanvasProjections::arc(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 7);

    ICanvasRenderContext2D* context =
        m_resourceManager->externalToObject<ICanvasRenderContext2D>(arguments[1], ObjectType::ICanvasRenderContext2D);
    RETURN_INVALID_REF_IF_NULL(context);
    return context->arc(arguments, argumentCount);
}

JsValueRef CanvasProjections::fill(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    ICanvasRenderContext2D* context =
        m_resourceManager->externalToObject<ICanvasRenderContext2D>(arguments[1], ObjectType::ICanvasRenderContext2D);
    RETURN_INVALID_REF_IF_NULL(context);
    return context->fill(arguments, argumentCount);
}

JsValueRef CanvasProjections::fillGradient(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    ICanvasRenderContext2D* context =
        m_resourceManager->externalToObject<ICanvasRenderContext2D>(arguments[1], ObjectType::ICanvasRenderContext2D);
    RETURN_INVALID_REF_IF_NULL(context);
    return context->fillGradient(arguments, argumentCount);
}

JsValueRef CanvasProjections::stroke(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    ICanvasRenderContext2D* context =
        m_resourceManager->externalToObject<ICanvasRenderContext2D>(arguments[1], ObjectType::ICanvasRenderContext2D);
    RETURN_INVALID_REF_IF_NULL(context);
    return context->stroke(arguments, argumentCount);
}

JsValueRef CanvasProjections::strokeGradient(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    ICanvasRenderContext2D* context =
        m_resourceManager->externalToObject<ICanvasRenderContext2D>(arguments[1], ObjectType::ICanvasRenderContext2D);
    RETURN_INVALID_REF_IF_NULL(context);
    return context->strokeGradient(arguments, argumentCount);
}

JsValueRef CanvasProjections::setTransform(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 8);

    ICanvasRenderContext2D* context =
        m_resourceManager->externalToObject<ICanvasRenderContext2D>(arguments[1], ObjectType::ICanvasRenderContext2D);
    RETURN_INVALID_REF_IF_NULL(context);
    return context->setTransform(arguments, argumentCount);
}

JsValueRef CanvasProjections::setLineStyle(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

    ICanvasRenderContext2D* context =
        m_resourceManager->externalToObject<ICanvasRenderContext2D>(arguments[1], ObjectType::ICanvasRenderContext2D);
    RETURN_INVALID_REF_IF_NULL(context);
    return context->setLineStyle(arguments, argumentCount);
}

JsValueRef CanvasProjections::setGlobalOpacity(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    ICanvasRenderContext2D* context =
        m_resourceManager->externalToObject<ICanvasRenderContext2D>(arguments[1], ObjectType::ICanvasRenderContext2D);
    RETURN_INVALID_REF_IF_NULL(context);
    return context->setGlobalOpacity(arguments, argumentCount);
}

JsValueRef CanvasProjections::getImageData(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    ICanvasRenderContext2D* context =
        m_resourceManager->externalToObject<ICanvasRenderContext2D>(arguments[1], ObjectType::ICanvasRenderContext2D);

    RETURN_INVALID_REF_IF_NULL(context);
    return context->getImageData(arguments, argumentCount);
}

JsValueRef CanvasProjections::getWidth(JsValueRef* arguments, unsigned short argumentCount)
{
    ICanvasRenderContext2D* context =
        m_resourceManager->externalToObject<ICanvasRenderContext2D>(arguments[1], ObjectType::ICanvasRenderContext2D);
    RETURN_INVALID_REF_IF_NULL(context);
    return context->getWidth(arguments, argumentCount);
}

JsValueRef CanvasProjections::setWidth(JsValueRef* arguments, unsigned short argumentCount)
{
    ICanvasRenderContext2D* context =
        m_resourceManager->externalToObject<ICanvasRenderContext2D>(arguments[1], ObjectType::ICanvasRenderContext2D);
    RETURN_INVALID_REF_IF_NULL(context);
    return context->setWidth(arguments, argumentCount);
}

JsValueRef CanvasProjections::getHeight(JsValueRef* arguments, unsigned short argumentCount)
{
    ICanvasRenderContext2D* context =
        m_resourceManager->externalToObject<ICanvasRenderContext2D>(arguments[1], ObjectType::ICanvasRenderContext2D);
    RETURN_INVALID_REF_IF_NULL(context);
    return context->getHeight(arguments, argumentCount);
}

JsValueRef CanvasProjections::setHeight(JsValueRef* arguments, unsigned short argumentCount)
{
    ICanvasRenderContext2D* context =
        m_resourceManager->externalToObject<ICanvasRenderContext2D>(arguments[1], ObjectType::ICanvasRenderContext2D);
    RETURN_INVALID_REF_IF_NULL(context);
    return context->setHeight(arguments, argumentCount);
}

JsValueRef CanvasProjections::toDataUrl(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3 || argumentCount == 4);

    ICanvasRenderContext2D* context =
        m_resourceManager->externalToObject<ICanvasRenderContext2D>(arguments[1], ObjectType::ICanvasRenderContext2D);
    RETURN_INVALID_REF_IF_NULL(context);
    return context->toDataUrl(arguments, argumentCount);
}
