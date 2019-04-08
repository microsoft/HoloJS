#include "webgl-projections.h"
#include "../host-interfaces.h"
#include "../include/holojs/private/canvas-render-context-2d.h"
#include "../include/holojs/private/image-interface.h"
#include "../include/holojs/private/script-host-utilities.h"
#include "../resource-management/resource-manager.h"
#include <vector>

#include "webgl-objects.h"
#include "webgl-rendering-context.h"

using namespace HoloJs;
using namespace HoloJs::ResourceManagement;
using namespace HoloJs::WebGL;
using namespace std;

HRESULT WebGLProjections::initialize()
{
    // WebGL context projections
    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"createContext", L"webgl", staticCreateContext, this, &m_createContext));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"getShaderPrecisionFormat", L"webgl", staticGetShaderPrecisionFormat, this, &m_getShaderPrecisionFormat));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"getExtension", L"webgl", staticGetExtension, this, &m_getExtension));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"getParameter", L"webgl", staticGetParameter, this, &m_getParameter));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"createRenderbuffer", L"webgl", staticCreateRenderbuffer, this, &m_createRenderbuffer));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"bindRenderbuffer", L"webgl", staticBindRenderbuffer, this, &m_bindRenderbuffer));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"renderbufferStorage", L"webgl", staticRenderbufferStorage, this, &m_renderbufferStorage));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"createFramebuffer", L"webgl", staticCreateFramebuffer, this, &m_createFramebuffer));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"bindFramebuffer", L"webgl", staticBindFramebuffer, this, &m_bindFramebuffer));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"framebufferRenderbuffer", L"webgl", staticFramebufferRenderbuffer, this, &m_framebufferRenderbuffer));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"framebufferTexture2D", L"webgl", staticFramebufferTexture2D, this, &m_framebufferTexture2D));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"createTexture", L"webgl", staticCreateTexture, this, &m_createTexture));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"deleteTexture", L"webgl", staticDeleteTexture, this, &m_deleteTexture));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"bindTexture", L"webgl", staticBindTexture, this, &m_bindTexture));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"texParameteri", L"webgl", staticTexParameteri, this, &m_texParameteri));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"texImage2D1", L"webgl", staticTexImage2D1, this, &m_texImage2D1));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"texImage2D2", L"webgl", staticTexImage2D2, this, &m_texImage2D2));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"texImage2D3", L"webgl", staticTexImage2D3, this, &m_texImage2D3));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"texImage2D4", L"webgl", staticTexImage2D4, this, &m_texImage2D4));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"activeTexture", L"webgl", staticActiveTexture, this, &m_activeTexture));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"generateMipmap", L"webgl", staticGenerateMipmap, this, &m_generateMipmap));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"pixelStorei", L"webgl", staticPixelStorei, this, &m_pixelStorei));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"clearDepth", L"webgl", staticClearDepth, this, &m_clearDepth));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"clearStencil", L"webgl", staticClearStencil, this, &m_clearStencil));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(L"enable", L"webgl", staticEnable, this, &m_enable));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(L"disable", L"webgl", staticDisable, this, &m_disable));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(L"depthFunc", L"webgl", staticDepthFunc, this, &m_depthFunc));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(L"depthMask", L"webgl", staticDepthMask, this, &m_depthMask));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"depthRange", L"webgl", staticDepthRange, this, &m_depthRange));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(L"frontFace", L"webgl", staticFrontFace, this, &m_frontFace));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(L"cullFace", L"webgl", staticCullFace, this, &m_cullFace));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"blendColor", L"webgl", staticBlendColor, this, &m_blendColor));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"blendEquation", L"webgl", staticBlendEquation, this, &m_blendEquation));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"blendEquationSeparate", L"webgl", staticBlendEquationSeparate, this, &m_blendEquationSeparate));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(L"blendFunc", L"webgl", staticBlendFunc, this, &m_blendFunc));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"blendFuncSeparate", L"webgl", staticBlendFuncSeparate, this, &m_blendFuncSeparate));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(L"scissor", L"webgl", staticScissor, this, &m_scissor));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(L"viewport", L"webgl", staticViewport, this, &m_viewport));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"clearColor", L"webgl", staticClearColor, this, &m_clearColor));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(L"clear", L"webgl", staticClear, this, &m_clear));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"createBuffer", L"webgl", staticCreateBuffer, this, &m_createBuffer));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"deleteBuffer", L"webgl", staticDeleteBuffer, this, &m_deleteBuffer));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"bindBuffer", L"webgl", staticBindBuffer, this, &m_bindBuffer));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"bufferData1", L"webgl", staticBufferData1, this, &m_bufferData1));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"bufferData2", L"webgl", staticBufferData2, this, &m_bufferData2));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"bufferSubData", L"webgl", staticBufferSubData, this, &m_bufferSubData));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(L"colorMask", L"webgl", staticColorMask, this, &m_colorMask));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"drawArrays", L"webgl", staticDrawArrays, this, &m_drawArrays));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"drawElements", L"webgl", staticDrawElements, this, &m_drawElements));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"enableVertexAttribArray", L"webgl", staticEnableVertexAttribArray, this, &m_enableVertexAttribArray));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"disableVertexAttribArray", L"webgl", staticDisableVertexAttribArray, this, &m_disableVertexAttribArray));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"vertexAttribPointer", L"webgl", staticVertexAttribPointer, this, &m_vertexAttribPointer));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"createProgram", L"webgl", staticCreateProgram, this, &m_createProgram));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"useProgram", L"webgl", staticUseProgram, this, &m_useProgram));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"validateProgram", L"webgl", staticValidateProgram, this, &m_validateProgram));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"linkProgram", L"webgl", staticLinkProgram, this, &m_linkProgram));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"getProgramParameter", L"webgl", staticGetProgramParameter, this, &m_getProgramParameter));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"getProgramInfoLog", L"webgl", staticGetProgramInfoLog, this, &m_getProgramInfoLog));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"deleteProgram", L"webgl", staticDeleteProgram, this, &m_deleteProgram));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"bindAttribLocation", L"webgl", staticBindAttribLocation, this, &m_bindAttribLocation));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"getActiveUniform", L"webgl", staticGetActiveUniform, this, &m_getActiveUniform));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"getActiveAttrib", L"webgl", staticGetActiveAttrib, this, &m_getActiveAttrib));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"getAttribLocation", L"webgl", staticGetAttribLocation, this, &m_getAttribLocation));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"createShader", L"webgl", staticCreateShader, this, &m_createShader));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"shaderSource", L"webgl", staticShaderSource, this, &m_shaderSource));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"compileShader", L"webgl", staticCompileShader, this, &m_compileShader));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"getShaderParameter", L"webgl", staticGetShaderParameter, this, &m_getShaderParameter));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"getShaderInfoLog", L"webgl", staticGetShaderInfoLog, this, &m_getShaderInfoLog));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"attachShader", L"webgl", staticAttachShader, this, &m_attachShader));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"deleteShader", L"webgl", staticDeleteShader, this, &m_deleteShader));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"getUniformLocation", L"webgl", staticGetUniformLocation, this, &m_getUniformLocation));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(L"uniform1f", L"webgl", staticUniform1f, this, &m_uniform1f));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"uniform1fv", L"webgl", staticUniform1fv, this, &m_uniform1fv));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(L"uniform1i", L"webgl", staticUniform1i, this, &m_uniform1i));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"uniform1iv", L"webgl", staticUniform1iv, this, &m_uniform1iv));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(L"uniform2f", L"webgl", staticUniform2f, this, &m_uniform2f));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"uniform2fv", L"webgl", staticUniform2fv, this, &m_uniform2fv));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(L"uniform2i", L"webgl", staticUniform2i, this, &m_uniform2i));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"uniform2iv", L"webgl", staticUniform2iv, this, &m_uniform2iv));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(L"uniform3f", L"webgl", staticUniform3f, this, &m_uniform3f));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"uniform3fv", L"webgl", staticUniform3fv, this, &m_uniform3fv));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(L"uniform3i", L"webgl", staticUniform3i, this, &m_uniform3i));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"uniform3iv", L"webgl", staticUniform3iv, this, &m_uniform3iv));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(L"uniform4f", L"webgl", staticUniform4f, this, &m_uniform4f));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"uniform4fv", L"webgl", staticUniform4fv, this, &m_uniform4fv));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(L"uniform4i", L"webgl", staticUniform4i, this, &m_uniform4i));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"uniform4iv", L"webgl", staticUniform4iv, this, &m_uniform4iv));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"uniformMatrix2fv", L"webgl", staticUniformMatrix2fv, this, &m_uniformMatrix2fv));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"uniformMatrix3fv", L"webgl", staticUniformMatrix3fv, this, &m_uniformMatrix3fv));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"uniformMatrix4fv", L"webgl", staticUniformMatrix4fv, this, &m_uniformMatrix4fv));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"stencilFunc", L"webgl", staticStencilFunc, this, &m_stencilFunc));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"stencilMask", L"webgl", staticStencilMask, this, &m_stencilMask));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(L"stencilOp", L"webgl", staticStencilOp, this, &m_stencilOp));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(L"lineWidth", L"webgl", staticLineWidth, this, &m_lineWidth));

    RETURN_IF_FAILED(
        ScriptHostUtilities::ProjectFunction(L"polygonOffset", L"webgl", staticPolygonOffset, this, &m_polygonOffset));

    return S_OK;
}

JsValueRef WebGLProjections::createContext(JsValueRef* arguments, unsigned short argumentCount)
{
    auto newContext = new WebGLRenderingContext();
    newContext->isFrontFaceSwitched = isContextFrontFaceSwitched;
    return m_resourceManager->objectToDirectExternal(newContext, ObjectType::WebGLContext);
}

JsValueRef WebGLProjections::getShaderPrecisionFormat(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLenum shadertype = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
    GLenum precisiontype = ScriptHostUtilities::GLenumFromJsRef(arguments[3]);
    auto precisionFormat = context->getShaderPrecisionFormat(shadertype, precisiontype);

    // Project object to script; note that the projected object is opaque to the script
    auto returnObject = m_resourceManager->objectToExternal<WebGLShaderPrecisionFormat>(
        precisionFormat, ObjectType::WebGLShaderPrecisionFormat);

    JsValueRef rangeMinValue;
    RETURN_INVALID_REF_IF_JS_ERROR(JsIntToNumber(precisionFormat->rangeMin, &rangeMinValue));

    JsValueRef rangeMaxValue;
    RETURN_INVALID_REF_IF_JS_ERROR(JsIntToNumber(precisionFormat->rangeMax, &rangeMaxValue));

    JsValueRef precisionValue;
    RETURN_INVALID_REF_IF_JS_ERROR(JsIntToNumber(precisionFormat->precision, &precisionValue));

    // Add properties as required for WebGLShaderPrecisionFormat
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::SetJsProperty(returnObject, L"rangeMin", rangeMinValue));
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::SetJsProperty(returnObject, L"rangeMax", rangeMaxValue));
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::SetJsProperty(returnObject, L"precision", precisionValue));

    return returnObject;
}

JsValueRef WebGLProjections::getExtension(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    std::wstring name;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetString(arguments[2], name));

    JsValueRef nullRef;
    RETURN_INVALID_REF_IF_FAILED(JsGetNullValue(&nullRef));

    return nullRef;
}

JsValueRef WebGLProjections::getParameter(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLenum pname = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
    return context->getParameter(pname);
}

JsValueRef WebGLProjections::createRenderbuffer(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    auto returnObject = context->createRenderbuffer();
    return m_resourceManager->objectToExternal<WebGLRenderbuffer>(returnObject, ObjectType::WebGLRenderbuffer);
}

JsValueRef WebGLProjections::bindRenderbuffer(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLenum target = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
    WebGLRenderbuffer* renderbuffer =
        m_resourceManager->externalToObject<WebGLRenderbuffer>(arguments[3], ObjectType::WebGLRenderbuffer);

    context->bindRenderbuffer(target, renderbuffer);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::renderbufferStorage(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 6);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLenum target = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
    GLenum internalformat = ScriptHostUtilities::GLenumFromJsRef(arguments[3]);
    GLsizei width = ScriptHostUtilities::GLsizeiFromJsRef(arguments[4]);
    GLsizei height = ScriptHostUtilities::GLsizeiFromJsRef(arguments[5]);
    context->renderbufferStorage(target, internalformat, width, height);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::createFramebuffer(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    auto returnObject = context->createFramebuffer();
    return m_resourceManager->objectToExternal<WebGLFramebuffer>(returnObject, ObjectType::WebGLFramebuffer);
}

JsValueRef WebGLProjections::bindFramebuffer(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLenum target = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
    WebGLFramebuffer* framebuffer =
        m_resourceManager->externalToObject<WebGLFramebuffer>(arguments[3], ObjectType::WebGLFramebuffer);

    context->bindFramebuffer(target, framebuffer);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::framebufferRenderbuffer(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 6);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLenum target = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
    GLenum attachment = ScriptHostUtilities::GLenumFromJsRef(arguments[3]);
    GLenum renderbuffertarget = ScriptHostUtilities::GLenumFromJsRef(arguments[4]);
    WebGLRenderbuffer* renderbuffer =
        m_resourceManager->externalToObject<WebGLRenderbuffer>(arguments[5], ObjectType::WebGLRenderbuffer);
    RETURN_INVALID_REF_IF_NULL(renderbuffer);

    context->framebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::framebufferTexture2D(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 7);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLenum target = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
    GLenum attachment = ScriptHostUtilities::GLenumFromJsRef(arguments[3]);
    GLenum textarget = ScriptHostUtilities::GLenumFromJsRef(arguments[4]);
    WebGLTexture* texture = m_resourceManager->externalToObject<WebGLTexture>(arguments[5], ObjectType::WebGLTexture);
    RETURN_INVALID_REF_IF_NULL(texture);

    GLint level = ScriptHostUtilities::GLintFromJsRef(arguments[6]);
    context->framebufferTexture2D(target, attachment, textarget, texture, level);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::createTexture(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    auto returnObject = context->createTexture();
    return m_resourceManager->objectToExternal<WebGLTexture>(returnObject, ObjectType::WebGLTexture);
}

JsValueRef WebGLProjections::deleteTexture(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLTexture* texture = m_resourceManager->externalToObject<WebGLTexture>(arguments[2], ObjectType::WebGLTexture);

    context->deleteTexture(texture);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::bindTexture(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLenum target = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
    WebGLTexture* texture = m_resourceManager->externalToObject<WebGLTexture>(arguments[3], ObjectType::WebGLTexture);

    context->bindTexture(target, texture);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::texParameteri(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLenum target = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
    GLenum pname = ScriptHostUtilities::GLenumFromJsRef(arguments[3]);
    GLint param = ScriptHostUtilities::GLintFromJsRef(arguments[4]);
    context->texParameteri(target, pname, param);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::texImage2D1(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 11);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLenum target = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
    GLint level = ScriptHostUtilities::GLintFromJsRef(arguments[3]);
    GLenum internalformat = ScriptHostUtilities::GLenumFromJsRef(arguments[4]);
    GLsizei width = ScriptHostUtilities::GLsizeiFromJsRef(arguments[5]);
    GLsizei height = ScriptHostUtilities::GLsizeiFromJsRef(arguments[6]);
    GLint border = ScriptHostUtilities::GLintFromJsRef(arguments[7]);
    GLenum format = ScriptHostUtilities::GLenumFromJsRef(arguments[8]);
    GLenum type = ScriptHostUtilities::GLenumFromJsRef(arguments[9]);

    unsigned int imageStride = 0;  // unknown stride; flipY will not work

    JsValueType arrayRefType;
    RETURN_NULL_IF_JS_ERROR(JsGetValueType(arguments[10], &arrayRefType));
    if (arrayRefType != JsValueType::JsNull) {
        ChakraBytePtr pixels;
        unsigned int pixelsLength;
        int arrayElementSize;
        JsTypedArrayType arrayType;
        RETURN_NULL_IF_JS_ERROR(
            JsGetTypedArrayStorage(arguments[10], &pixels, &pixelsLength, &arrayType, &arrayElementSize));
        context->texImage2D(target, level, internalformat, width, height, border, format, type, pixels, imageStride);
    } else {
        context->texImage2D(target, level, internalformat, width, height, border, format, type, nullptr, imageStride);
    }

    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::texImage2D2(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 10);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLenum target = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
    GLint level = ScriptHostUtilities::GLintFromJsRef(arguments[3]);
    GLenum internalformat = ScriptHostUtilities::GLenumFromJsRef(arguments[4]);

    // Only RGB  and RGBA supported for now
    RETURN_INVALID_REF_IF_TRUE((internalformat == GL_RGBA) && (internalformat == GL_RGB));

    GLsizei width = ScriptHostUtilities::GLsizeiFromJsRef(arguments[5]);
    GLsizei height = ScriptHostUtilities::GLsizeiFromJsRef(arguments[6]);

    GLenum format = ScriptHostUtilities::GLenumFromJsRef(arguments[7]);
    GLenum type = ScriptHostUtilities::GLenumFromJsRef(arguments[8]);

    auto imageElement = m_resourceManager->externalToObject<HoloJs::IIMage>(arguments[9], ObjectType::IImage);
    RETURN_INVALID_REF_IF_NULL(imageElement);

    context->texImage2D(target, level, internalformat, width, height, 0, format, type, imageElement);

    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::texImage2D3(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 10);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLenum target = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
    GLint level = ScriptHostUtilities::GLintFromJsRef(arguments[3]);
    GLenum internalformat = ScriptHostUtilities::GLenumFromJsRef(arguments[4]);

    GLsizei width = ScriptHostUtilities::GLsizeiFromJsRef(arguments[5]);
    GLsizei height = ScriptHostUtilities::GLsizeiFromJsRef(arguments[6]);

    GLenum format = ScriptHostUtilities::GLenumFromJsRef(arguments[7]);
    GLenum type = ScriptHostUtilities::GLenumFromJsRef(arguments[8]);

    auto context2D = m_resourceManager->externalToObject<HoloJs::ICanvasRenderContext2D>(
        arguments[9], ObjectType::ICanvasRenderContext2D);
    RETURN_INVALID_REF_IF_NULL(context2D);

    unsigned int canvasStride;
    vector<unsigned char> canvasPixels;
    RETURN_INVALID_REF_IF_FAILED(context2D->getImageDataNative(0, 0, width, height, canvasStride, canvasPixels));

    context->texImage2D(
        target, level, internalformat, width, height, 0, format, type, canvasPixels.data(), canvasStride);

    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::texImage2D4(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 10);

    /*WebGLRenderingContext* context = m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1],
    ObjectType::WebGLContext); RETURN_INVALID_REF_IF_NULL(context);

    GLenum target = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
    GLint level = ScriptHostUtilities::GLintFromJsRef(arguments[3]);
    GLenum internalformat = ScriptHostUtilities::GLenumFromJsRef(arguments[4]);

    GLsizei width = ScriptHostUtilities::GLsizeiFromJsRef(arguments[5]);
    GLsizei height = ScriptHostUtilities::GLsizeiFromJsRef(arguments[6]);

    GLenum format = ScriptHostUtilities::GLenumFromJsRef(arguments[7]);
    GLenum type = ScriptHostUtilities::GLenumFromJsRef(arguments[8]);

    auto videoElement = m_resourceManager->externalToObject<VideoElement>(arguments[9]);
    RETURN_INVALID_REF_IF_NULL(videoElement);

    void* pixels;
    unsigned int pixelsSize;
    if (videoElement->IsNewFrameAvailable()) {
        DWORD64 start = GetTickCount64();
        RETURN_INVALID_REF_IF_FALSE(videoElement->LockNextFrame(&pixels, &pixelsSize));
        std::wstring endString = std::to_wstring(GetTickCount64() - start) + L"\n";
        OutputDebugStringW(endString.c_str());
        context->texImage2DNoFlip(target, level, internalformat, width, height, 0, format, type, pixels);
        videoElement->UnlockFrame();
    }*/

    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::activeTexture(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLenum texture = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
    context->activeTexture(texture);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::generateMipmap(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLenum target = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
    context->generateMipmap(target);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::pixelStorei(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLenum pname = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
    if (pname == UNPACK_FLIP_Y_WEBGL || pname == UNPACK_PREMULTIPLY_ALPHA_WEBGL) {
        GLboolean param = ScriptHostUtilities::GLbooleanFromJsRef(arguments[3]);
        context->pixelStorei(pname, param);
    } else {
        GLint param = ScriptHostUtilities::GLintFromJsRef(arguments[3]);
        context->pixelStorei(pname, param);
    }
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::clearDepth(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLclampf depth = ScriptHostUtilities::GLclampfFromJsRef(arguments[2]);
    context->clearDepth(depth);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::clearStencil(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLint s = ScriptHostUtilities::GLintFromJsRef(arguments[2]);
    context->clearStencil(s);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::enable(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLenum cap = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
    context->enable(cap);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::disable(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLenum cap = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
    context->disable(cap);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::depthFunc(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLenum func = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
    context->depthFunc(func);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::depthMask(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLboolean flag = ScriptHostUtilities::GLbooleanFromJsRef(arguments[2]);
    context->depthMask(flag);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::depthRange(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLclampf zNear = ScriptHostUtilities::GLclampfFromJsRef(arguments[2]);
    GLclampf zFar = ScriptHostUtilities::GLclampfFromJsRef(arguments[3]);
    context->depthRange(zNear, zFar);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::frontFace(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLenum mode = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
    context->frontFace(mode);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::cullFace(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLenum mode = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
    context->cullFace(mode);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::blendColor(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 6);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLclampf red = ScriptHostUtilities::GLclampfFromJsRef(arguments[2]);
    GLclampf green = ScriptHostUtilities::GLclampfFromJsRef(arguments[3]);
    GLclampf blue = ScriptHostUtilities::GLclampfFromJsRef(arguments[4]);
    GLclampf alpha = ScriptHostUtilities::GLclampfFromJsRef(arguments[5]);
    context->blendColor(red, green, blue, alpha);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::blendEquation(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLenum mode = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
    context->blendEquation(mode);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::blendEquationSeparate(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLenum modeRGB = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
    GLenum modeAlpha = ScriptHostUtilities::GLenumFromJsRef(arguments[3]);
    context->blendEquationSeparate(modeRGB, modeAlpha);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::blendFunc(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLenum sfactor = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
    GLenum dfactor = ScriptHostUtilities::GLenumFromJsRef(arguments[3]);
    context->blendFunc(sfactor, dfactor);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::blendFuncSeparate(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 6);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLenum srcRGB = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
    GLenum dstRGB = ScriptHostUtilities::GLenumFromJsRef(arguments[3]);
    GLenum srcAlpha = ScriptHostUtilities::GLenumFromJsRef(arguments[4]);
    GLenum dstAlpha = ScriptHostUtilities::GLenumFromJsRef(arguments[5]);
    context->blendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::scissor(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 6);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLint x = ScriptHostUtilities::GLintFromJsRef(arguments[2]);
    GLint y = ScriptHostUtilities::GLintFromJsRef(arguments[3]);
    GLsizei width = ScriptHostUtilities::GLsizeiFromJsRef(arguments[4]);
    GLsizei height = ScriptHostUtilities::GLsizeiFromJsRef(arguments[5]);
    context->scissor(x, y, width, height);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::viewport(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 6);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLint x = ScriptHostUtilities::GLintFromJsRef(arguments[2]);
    GLint y = ScriptHostUtilities::GLintFromJsRef(arguments[3]);
    GLsizei width = ScriptHostUtilities::GLsizeiFromJsRef(arguments[4]);
    GLsizei height = ScriptHostUtilities::GLsizeiFromJsRef(arguments[5]);
    context->viewport(x, y, width, height);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::clearColor(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 6);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLclampf red = ScriptHostUtilities::GLclampfFromJsRef(arguments[2]);
    GLclampf green = ScriptHostUtilities::GLclampfFromJsRef(arguments[3]);
    GLclampf blue = ScriptHostUtilities::GLclampfFromJsRef(arguments[4]);
    GLclampf alpha = ScriptHostUtilities::GLclampfFromJsRef(arguments[5]);
    context->clearColor(red, green, blue, alpha);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::clear(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLbitfield mask = ScriptHostUtilities::GLbitfieldFromJsRef(arguments[2]);
    context->clear(mask);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::createBuffer(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    auto returnObject = context->createBuffer();
    return m_resourceManager->objectToExternal<WebGLBuffer>(returnObject, ObjectType::WebGLBuffer);
}

JsValueRef WebGLProjections::deleteBuffer(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLBuffer* buffer = m_resourceManager->externalToObject<WebGLBuffer>(arguments[2], ObjectType::WebGLBuffer);

    context->deleteBuffer(buffer);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::bindBuffer(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLenum target = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);

    WebGLBuffer* buffer = m_resourceManager->externalToObject<WebGLBuffer>(arguments[3], ObjectType::WebGLBuffer);

    context->bindBuffer(target, buffer);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::bufferSubData(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLenum target = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
    GLsizeiptr offset = ScriptHostUtilities::GLsizeiptrFromJsRef(arguments[3]);

    JsValueType dataType;
    RETURN_INVALID_REF_IF_JS_ERROR(JsGetValueType(arguments[4], &dataType));
    RETURN_INVALID_REF_IF_FALSE(dataType == JsTypedArray || dataType == JsArrayBuffer);

    ChakraBytePtr data;
    unsigned int dataLength;
    if (dataType == JsTypedArray) {
        RETURN_INVALID_REF_IF_JS_ERROR(JsGetTypedArrayStorage(arguments[4], &data, &dataLength, nullptr, nullptr));
    } else {
        RETURN_INVALID_REF_IF_JS_ERROR(JsGetArrayBufferStorage(arguments[4], &data, &dataLength));
    }

    context->bufferSubData(target, offset, data, dataLength);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::bufferData1(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLenum target = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
    GLsizeiptr size = ScriptHostUtilities::GLsizeiptrFromJsRef(arguments[3]);
    GLenum usage = ScriptHostUtilities::GLenumFromJsRef(arguments[4]);
    context->bufferData(target, size, nullptr, usage);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::bufferData2(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLenum target = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);

    ChakraBytePtr data;
    unsigned int dataLength;
    int arrayElementSize;
    JsTypedArrayType arrayType;
    JsGetTypedArrayStorage(arguments[3], &data, &dataLength, &arrayType, &arrayElementSize);
    GLenum usage = ScriptHostUtilities::GLenumFromJsRef(arguments[4]);
    context->bufferData(target, dataLength, data, usage);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::colorMask(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 6);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLboolean red = ScriptHostUtilities::GLbooleanFromJsRef(arguments[2]);
    GLboolean green = ScriptHostUtilities::GLbooleanFromJsRef(arguments[3]);
    GLboolean blue = ScriptHostUtilities::GLbooleanFromJsRef(arguments[4]);
    GLboolean alpha = ScriptHostUtilities::GLbooleanFromJsRef(arguments[5]);
    context->colorMask(red, green, blue, alpha);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::drawArrays(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLenum mode = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
    GLint first = ScriptHostUtilities::GLintFromJsRef(arguments[3]);
    GLsizei count = ScriptHostUtilities::GLsizeiFromJsRef(arguments[4]);
    context->drawArrays(mode, first, count);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::drawElements(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 6);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLenum mode = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
    GLsizei count = ScriptHostUtilities::GLsizeiFromJsRef(arguments[3]);
    GLenum type = ScriptHostUtilities::GLenumFromJsRef(arguments[4]);
    GLint offset = ScriptHostUtilities::GLintFromJsRef(arguments[5]);
    context->drawElements(mode, count, type, offset);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::enableVertexAttribArray(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLuint index = ScriptHostUtilities::GLuintFromJsRef(arguments[2]);
    context->enableVertexAttribArray(index);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::disableVertexAttribArray(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLuint index = ScriptHostUtilities::GLuintFromJsRef(arguments[2]);
    context->disableVertexAttribArray(index);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::vertexAttribPointer(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 8);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLuint indx = ScriptHostUtilities::GLuintFromJsRef(arguments[2]);
    GLint size = ScriptHostUtilities::GLintFromJsRef(arguments[3]);
    GLenum type = ScriptHostUtilities::GLenumFromJsRef(arguments[4]);
    GLboolean normalized = ScriptHostUtilities::GLbooleanFromJsRef(arguments[5]);
    GLsizei stride = ScriptHostUtilities::GLsizeiFromJsRef(arguments[6]);
    GLint offset = ScriptHostUtilities::GLintFromJsRef(arguments[7]);
    context->vertexAttribPointer(indx, size, type, normalized, stride, offset);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::createProgram(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    auto returnObject = context->createProgram();
    return m_resourceManager->objectToExternal<WebGLProgram>(returnObject, ObjectType::WebGLProgram);
}

JsValueRef WebGLProjections::useProgram(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLProgram* program = m_resourceManager->externalToObject<WebGLProgram>(arguments[2], ObjectType::WebGLProgram);
    RETURN_INVALID_REF_IF_NULL(program);

    context->useProgram(program);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::validateProgram(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLProgram* program = m_resourceManager->externalToObject<WebGLProgram>(arguments[2], ObjectType::WebGLProgram);
    RETURN_INVALID_REF_IF_NULL(program);

    context->validateProgram(program);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::linkProgram(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLProgram* program = m_resourceManager->externalToObject<WebGLProgram>(arguments[2], ObjectType::WebGLProgram);
    RETURN_INVALID_REF_IF_NULL(program);

    context->linkProgram(program);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::getProgramParameter(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLProgram* program = m_resourceManager->externalToObject<WebGLProgram>(arguments[2], ObjectType::WebGLProgram);
    RETURN_INVALID_REF_IF_NULL(program);

    GLenum pname = ScriptHostUtilities::GLenumFromJsRef(arguments[3]);
    return context->getProgramParameter(program, pname);
}

JsValueRef WebGLProjections::getProgramInfoLog(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLProgram* program = m_resourceManager->externalToObject<WebGLProgram>(arguments[2], ObjectType::WebGLProgram);
    RETURN_INVALID_REF_IF_NULL(program);

    auto log = context->getProgramInfoLog(program);
    JsValueRef retValue;
    RETURN_NULL_IF_JS_ERROR(JsPointerToString(log.c_str(), log.length(), &retValue));

    return retValue;
}

JsValueRef WebGLProjections::deleteProgram(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLProgram* program = m_resourceManager->externalToObject<WebGLProgram>(arguments[2], ObjectType::WebGLProgram);
    RETURN_INVALID_REF_IF_NULL(program);

    context->deleteProgram(program);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::bindAttribLocation(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLProgram* program = m_resourceManager->externalToObject<WebGLProgram>(arguments[2], ObjectType::WebGLProgram);
    RETURN_INVALID_REF_IF_NULL(program);

    GLuint index = ScriptHostUtilities::GLuintFromJsRef(arguments[3]);
    std::wstring name;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetString(arguments[4], name));
    context->bindAttribLocation(program, index, name);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::getActiveUniform(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLProgram* program = m_resourceManager->externalToObject<WebGLProgram>(arguments[2], ObjectType::WebGLProgram);
    RETURN_INVALID_REF_IF_NULL(program);

    GLuint index = ScriptHostUtilities::GLuintFromJsRef(arguments[3]);
    auto activeUniform = context->getActiveUniform(program, index);

    // Project object to script; note that the projected object is opaque to the script
    auto returnObject =
        m_resourceManager->objectToExternal<WebGLActiveInfo>(activeUniform, ObjectType::WebGLActiveInfo);

    JsValueRef namePropertyValue;
    RETURN_INVALID_REF_IF_JS_ERROR(
        JsPointerToString(activeUniform->name.c_str(), wcslen(activeUniform->name.data()), &namePropertyValue));

    JsValueRef sizePropertyValue;
    RETURN_INVALID_REF_IF_JS_ERROR(JsIntToNumber(activeUniform->size, &sizePropertyValue));

    JsValueRef typePropertyValue;
    RETURN_INVALID_REF_IF_JS_ERROR(JsIntToNumber(activeUniform->type, &typePropertyValue));

    // Add properties for WebGLActiveInfo as required by spec
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::SetJsProperty(returnObject, L"name", namePropertyValue));
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::SetJsProperty(returnObject, L"size", sizePropertyValue));
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::SetJsProperty(returnObject, L"type", typePropertyValue));

    return returnObject;
}

JsValueRef WebGLProjections::getActiveAttrib(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLProgram* program = m_resourceManager->externalToObject<WebGLProgram>(arguments[2], ObjectType::WebGLProgram);
    RETURN_INVALID_REF_IF_NULL(program);

    GLuint index = ScriptHostUtilities::GLuintFromJsRef(arguments[3]);
    auto activeAttribute = context->getActiveAttrib(program, index);

    // Project object to script; note that the projected object is opaque to the script
    auto returnObject =
        m_resourceManager->objectToExternal<WebGLActiveInfo>(activeAttribute, ObjectType::WebGLActiveInfo);

    JsValueRef namePropertyValue;
    RETURN_INVALID_REF_IF_JS_ERROR(
        JsPointerToString(activeAttribute->name.c_str(), wcslen(activeAttribute->name.data()), &namePropertyValue));

    JsValueRef sizePropertyValue;
    RETURN_INVALID_REF_IF_JS_ERROR(JsIntToNumber(activeAttribute->size, &sizePropertyValue));

    JsValueRef typePropertyValue;
    RETURN_INVALID_REF_IF_JS_ERROR(JsIntToNumber(activeAttribute->type, &typePropertyValue));

    // Add properties for WebGLActiveInfo as required by spec
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::SetJsProperty(returnObject, L"name", namePropertyValue));
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::SetJsProperty(returnObject, L"size", sizePropertyValue));
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::SetJsProperty(returnObject, L"type", typePropertyValue));

    return returnObject;
}

JsValueRef WebGLProjections::getAttribLocation(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLProgram* program = m_resourceManager->externalToObject<WebGLProgram>(arguments[2], ObjectType::WebGLProgram);
    RETURN_INVALID_REF_IF_NULL(program);

    std::wstring name;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetString(arguments[3], name));

    JsValueRef retValue;
    RETURN_NULL_IF_JS_ERROR(JsIntToNumber(context->getAttribLocation(program, name), &retValue));
    return retValue;
}

JsValueRef WebGLProjections::createShader(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLenum type = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
    auto returnObject = context->createShader(type);
    return m_resourceManager->objectToExternal<WebGLShader>(returnObject, ObjectType::WebGLShader);
}

JsValueRef WebGLProjections::shaderSource(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLShader* shader = m_resourceManager->externalToObject<WebGLShader>(arguments[2], ObjectType::WebGLShader);
    RETURN_INVALID_REF_IF_NULL(shader);

    std::wstring source;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetString(arguments[3], source));
    context->shaderSource(shader, source);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::compileShader(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLShader* shader = m_resourceManager->externalToObject<WebGLShader>(arguments[2], ObjectType::WebGLShader);
    RETURN_INVALID_REF_IF_NULL(shader);

    context->compileShader(shader);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::getShaderParameter(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLShader* shader = m_resourceManager->externalToObject<WebGLShader>(arguments[2], ObjectType::WebGLShader);
    RETURN_INVALID_REF_IF_NULL(shader);

    GLenum pname = ScriptHostUtilities::GLenumFromJsRef(arguments[3]);
    return context->getShaderParameter(shader, pname);
}

JsValueRef WebGLProjections::getShaderInfoLog(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLShader* shader = m_resourceManager->externalToObject<WebGLShader>(arguments[2], ObjectType::WebGLShader);
    RETURN_INVALID_REF_IF_NULL(shader);

    auto log = context->getShaderInfoLog(shader);

    JsValueRef logValue;
    JsPointerToString(log.c_str(), log.length(), &logValue);

    return logValue;
}

JsValueRef WebGLProjections::attachShader(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLProgram* program = m_resourceManager->externalToObject<WebGLProgram>(arguments[2], ObjectType::WebGLProgram);
    RETURN_INVALID_REF_IF_NULL(program);

    WebGLShader* shader = m_resourceManager->externalToObject<WebGLShader>(arguments[3], ObjectType::WebGLShader);
    RETURN_INVALID_REF_IF_NULL(shader);

    context->attachShader(program, shader);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::deleteShader(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLShader* shader = m_resourceManager->externalToObject<WebGLShader>(arguments[2], ObjectType::WebGLShader);
    RETURN_INVALID_REF_IF_NULL(shader);

    context->deleteShader(shader);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::getUniformLocation(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLProgram* program = m_resourceManager->externalToObject<WebGLProgram>(arguments[2], ObjectType::WebGLProgram);
    RETURN_INVALID_REF_IF_NULL(program);

    std::wstring name;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetString(arguments[3], name));
    auto returnObject = context->getUniformLocation(program, name);

    return m_resourceManager->objectToExternal(returnObject, ObjectType::WebGLUniformLocation);
}

JsValueRef WebGLProjections::uniform1f(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLUniformLocation* location =
        m_resourceManager->externalToObject<WebGLUniformLocation>(arguments[2], ObjectType::WebGLUniformLocation);
    RETURN_INVALID_REF_IF_NULL(location);

    GLfloat x = ScriptHostUtilities::GLfloatFromJsRef(arguments[3]);
    context->uniform1f(location, x);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::uniform1fv(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLUniformLocation* location =
        m_resourceManager->externalToObject<WebGLUniformLocation>(arguments[2], ObjectType::WebGLUniformLocation);
    RETURN_INVALID_REF_IF_NULL(location);

    ChakraBytePtr v;
    unsigned int vLength;
    int arrayElementSize;
    JsTypedArrayType arrayType;
    JsGetTypedArrayStorage(arguments[3], &v, &vLength, &arrayType, &arrayElementSize);
    context->uniform1fv(location, vLength / arrayElementSize / 1, reinterpret_cast<const GLfloat*>(v));
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::uniform1i(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLUniformLocation* location =
        m_resourceManager->externalToObject<WebGLUniformLocation>(arguments[2], ObjectType::WebGLUniformLocation);
    RETURN_INVALID_REF_IF_NULL(location);

    auto value = arguments[3];
    bool boolValue;
    int intValue;

    // Sometimes uniform1i is called with a boolean value; convert it to 1|0 here
    if (JsNumberToInt(value, &intValue) == JsNoError) {
        context->uniform1i(location, intValue);
    } else if (JsBooleanToBool(value, &boolValue) == JsNoError) {
        context->uniform1i(location, boolValue ? 1 : 0);
    }

    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::uniform1iv(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLUniformLocation* location =
        m_resourceManager->externalToObject<WebGLUniformLocation>(arguments[2], ObjectType::WebGLUniformLocation);
    RETURN_INVALID_REF_IF_NULL(location);

    ChakraBytePtr v;
    unsigned int vLength;
    int arrayElementSize;
    JsTypedArrayType arrayType;
    JsGetTypedArrayStorage(arguments[3], &v, &vLength, &arrayType, &arrayElementSize);
    context->uniform1iv(location, vLength / arrayElementSize / 1, reinterpret_cast<const GLint*>(v));
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::uniform2f(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLUniformLocation* location =
        m_resourceManager->externalToObject<WebGLUniformLocation>(arguments[2], ObjectType::WebGLUniformLocation);
    RETURN_INVALID_REF_IF_NULL(location);

    GLfloat x = ScriptHostUtilities::GLfloatFromJsRef(arguments[3]);
    GLfloat y = ScriptHostUtilities::GLfloatFromJsRef(arguments[4]);
    context->uniform2f(location, x, y);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::uniform2fv(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLUniformLocation* location =
        m_resourceManager->externalToObject<WebGLUniformLocation>(arguments[2], ObjectType::WebGLUniformLocation);
    RETURN_INVALID_REF_IF_NULL(location);

    ChakraBytePtr v;
    unsigned int vLength;
    int arrayElementSize;
    JsTypedArrayType arrayType;
    JsGetTypedArrayStorage(arguments[3], &v, &vLength, &arrayType, &arrayElementSize);
    context->uniform2fv(location, vLength / arrayElementSize / 2, reinterpret_cast<const GLfloat*>(v));
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::uniform2i(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLUniformLocation* location =
        m_resourceManager->externalToObject<WebGLUniformLocation>(arguments[2], ObjectType::WebGLUniformLocation);
    RETURN_INVALID_REF_IF_NULL(location);

    GLint x = ScriptHostUtilities::GLintFromJsRef(arguments[3]);
    GLint y = ScriptHostUtilities::GLintFromJsRef(arguments[4]);
    context->uniform2i(location, x, y);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::uniform2iv(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLUniformLocation* location =
        m_resourceManager->externalToObject<WebGLUniformLocation>(arguments[2], ObjectType::WebGLUniformLocation);
    RETURN_INVALID_REF_IF_NULL(location);

    ChakraBytePtr v;
    unsigned int vLength;
    int arrayElementSize;
    JsTypedArrayType arrayType;
    JsGetTypedArrayStorage(arguments[3], &v, &vLength, &arrayType, &arrayElementSize);
    context->uniform2iv(location, vLength / arrayElementSize / 2, reinterpret_cast<const GLint*>(v));
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::uniform3f(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 6);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLUniformLocation* location =
        m_resourceManager->externalToObject<WebGLUniformLocation>(arguments[2], ObjectType::WebGLUniformLocation);
    RETURN_INVALID_REF_IF_NULL(location);

    GLfloat x = ScriptHostUtilities::GLfloatFromJsRef(arguments[3]);
    GLfloat y = ScriptHostUtilities::GLfloatFromJsRef(arguments[4]);
    GLfloat z = ScriptHostUtilities::GLfloatFromJsRef(arguments[5]);
    context->uniform3f(location, x, y, z);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::uniform3fv(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLUniformLocation* location =
        m_resourceManager->externalToObject<WebGLUniformLocation>(arguments[2], ObjectType::WebGLUniformLocation);
    RETURN_INVALID_REF_IF_NULL(location);

    ChakraBytePtr v;
    unsigned int vLength;
    int arrayElementSize;
    JsTypedArrayType arrayType;
    JsGetTypedArrayStorage(arguments[3], &v, &vLength, &arrayType, &arrayElementSize);
    context->uniform3fv(location, vLength / arrayElementSize / 3, reinterpret_cast<const GLfloat*>(v));
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::uniform3i(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 6);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLUniformLocation* location =
        m_resourceManager->externalToObject<WebGLUniformLocation>(arguments[2], ObjectType::WebGLUniformLocation);
    RETURN_INVALID_REF_IF_NULL(location);

    GLint x = ScriptHostUtilities::GLintFromJsRef(arguments[3]);
    GLint y = ScriptHostUtilities::GLintFromJsRef(arguments[4]);
    GLint z = ScriptHostUtilities::GLintFromJsRef(arguments[5]);
    context->uniform3i(location, x, y, z);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::uniform3iv(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLUniformLocation* location =
        m_resourceManager->externalToObject<WebGLUniformLocation>(arguments[2], ObjectType::WebGLUniformLocation);
    RETURN_INVALID_REF_IF_NULL(location);

    ChakraBytePtr v;
    unsigned int vLength;
    int arrayElementSize;
    JsTypedArrayType arrayType;
    JsGetTypedArrayStorage(arguments[3], &v, &vLength, &arrayType, &arrayElementSize);
    context->uniform3iv(location, vLength / arrayElementSize / 3, reinterpret_cast<const GLint*>(v));
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::uniform4f(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 7);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLUniformLocation* location =
        m_resourceManager->externalToObject<WebGLUniformLocation>(arguments[2], ObjectType::WebGLUniformLocation);
    RETURN_INVALID_REF_IF_NULL(location);

    GLfloat x = ScriptHostUtilities::GLfloatFromJsRef(arguments[3]);
    GLfloat y = ScriptHostUtilities::GLfloatFromJsRef(arguments[4]);
    GLfloat z = ScriptHostUtilities::GLfloatFromJsRef(arguments[5]);
    GLfloat w = ScriptHostUtilities::GLfloatFromJsRef(arguments[6]);
    context->uniform4f(location, x, y, z, w);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::uniform4fv(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLUniformLocation* location =
        m_resourceManager->externalToObject<WebGLUniformLocation>(arguments[2], ObjectType::WebGLUniformLocation);
    RETURN_INVALID_REF_IF_NULL(location);

    ChakraBytePtr v;
    unsigned int vLength;
    int arrayElementSize;
    JsTypedArrayType arrayType;
    JsGetTypedArrayStorage(arguments[3], &v, &vLength, &arrayType, &arrayElementSize);
    context->uniform4fv(location, vLength / arrayElementSize / 4, reinterpret_cast<const GLfloat*>(v));
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::uniform4i(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 7);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLUniformLocation* location =
        m_resourceManager->externalToObject<WebGLUniformLocation>(arguments[2], ObjectType::WebGLUniformLocation);
    RETURN_INVALID_REF_IF_NULL(location);

    GLint x = ScriptHostUtilities::GLintFromJsRef(arguments[3]);
    GLint y = ScriptHostUtilities::GLintFromJsRef(arguments[4]);
    GLint z = ScriptHostUtilities::GLintFromJsRef(arguments[5]);
    GLint w = ScriptHostUtilities::GLintFromJsRef(arguments[6]);
    context->uniform4i(location, x, y, z, w);
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::uniform4iv(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLUniformLocation* location =
        m_resourceManager->externalToObject<WebGLUniformLocation>(arguments[2], ObjectType::WebGLUniformLocation);
    RETURN_INVALID_REF_IF_NULL(location);

    ChakraBytePtr v;
    unsigned int vLength;
    int arrayElementSize;
    JsTypedArrayType arrayType;
    JsGetTypedArrayStorage(arguments[3], &v, &vLength, &arrayType, &arrayElementSize);
    context->uniform4iv(location, vLength / arrayElementSize / 4, reinterpret_cast<const GLint*>(v));
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::uniformMatrix2fv(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLUniformLocation* location =
        m_resourceManager->externalToObject<WebGLUniformLocation>(arguments[2], ObjectType::WebGLUniformLocation);
    RETURN_INVALID_REF_IF_NULL(location);

    GLboolean transpose = ScriptHostUtilities::GLbooleanFromJsRef(arguments[3]);
    ChakraBytePtr value;
    unsigned int valueLength;
    int arrayElementSize;
    JsTypedArrayType arrayType;
    JsGetTypedArrayStorage(arguments[4], &value, &valueLength, &arrayType, &arrayElementSize);
    context->uniformMatrix2fv(
        location, transpose, valueLength / arrayElementSize / 4, reinterpret_cast<const GLfloat*>(value));
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::uniformMatrix3fv(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLUniformLocation* location =
        m_resourceManager->externalToObject<WebGLUniformLocation>(arguments[2], ObjectType::WebGLUniformLocation);
    RETURN_INVALID_REF_IF_NULL(location);

    GLboolean transpose = ScriptHostUtilities::GLbooleanFromJsRef(arguments[3]);
    ChakraBytePtr value;
    unsigned int valueLength;
    int arrayElementSize;
    JsTypedArrayType arrayType;
    JsGetTypedArrayStorage(arguments[4], &value, &valueLength, &arrayType, &arrayElementSize);
    context->uniformMatrix3fv(
        location, transpose, valueLength / arrayElementSize / 9, reinterpret_cast<const GLfloat*>(value));
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::uniformMatrix4fv(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    WebGLUniformLocation* location =
        m_resourceManager->externalToObject<WebGLUniformLocation>(arguments[2], ObjectType::WebGLUniformLocation);
    RETURN_INVALID_REF_IF_NULL(location);

    GLboolean transpose = ScriptHostUtilities::GLbooleanFromJsRef(arguments[3]);
    ChakraBytePtr value;
    unsigned int valueLength;
    int arrayElementSize;
    JsTypedArrayType arrayType;
    JsGetTypedArrayStorage(arguments[4], &value, &valueLength, &arrayType, &arrayElementSize);
    context->uniformMatrix4fv(
        location, transpose, valueLength / arrayElementSize / 16, reinterpret_cast<const GLfloat*>(value));
    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::stencilFunc(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLenum func = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
    GLint ref = ScriptHostUtilities::GLintFromJsRef(arguments[3]);
    GLuint mask = ScriptHostUtilities::GLuintFromJsRef(arguments[4]);

    context->stencilFunc(func, ref, mask);

    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::stencilMask(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLuint mask = ScriptHostUtilities::GLuintFromJsRef(arguments[2]);
    context->stencilMask(mask);

    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::stencilOp(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLenum fail = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
    GLenum zfail = ScriptHostUtilities::GLenumFromJsRef(arguments[3]);
    GLenum zpass = ScriptHostUtilities::GLenumFromJsRef(arguments[4]);
    context->stencilOp(fail, zfail, zpass);

    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::lineWidth(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLfloat width = ScriptHostUtilities::GLfloatFromJsRef(arguments[2]);
    context->lineWidth(width);

    return JS_INVALID_REFERENCE;
}

JsValueRef WebGLProjections::polygonOffset(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    WebGLRenderingContext* context =
        m_resourceManager->externalToObject<WebGLRenderingContext>(arguments[1], ObjectType::WebGLContext);
    RETURN_INVALID_REF_IF_NULL(context);

    GLfloat factor = ScriptHostUtilities::GLfloatFromJsRef(arguments[2]);
    GLfloat units = ScriptHostUtilities::GLfloatFromJsRef(arguments[3]);
    context->polygonOffset(factor, units);

    return JS_INVALID_REFERENCE;
}
