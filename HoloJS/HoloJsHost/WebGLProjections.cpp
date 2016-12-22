#include "pch.h"
#include "VideoElement.h"
#include "ImageElement.h"
#include "WebGLObjects.h"
#include "RenderingContext2D.h"
#include "WebGLRenderingContext.h"
#include "WebGLProjections.h"

using namespace HologramJS::WebGL;
using namespace HologramJS::Utilities;
using namespace HologramJS::API;

bool
WebGLProjections::Initialize()
{
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"createContext", L"webgl", createContext));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"createContext2D", L"canvas2d", createContext2D));
	
	// WebGL context projections
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"getShaderPrecisionFormat", L"webgl", getShaderPrecisionFormat));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"getExtension", L"webgl", getExtension));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"getParameter", L"webgl", getParameter));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"createRenderbuffer", L"webgl", createRenderbuffer));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"bindRenderbuffer", L"webgl", bindRenderbuffer));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"renderbufferStorage", L"webgl", renderbufferStorage));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"createFramebuffer", L"webgl", createFramebuffer));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"bindFramebuffer", L"webgl", bindFramebuffer));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"framebufferRenderbuffer", L"webgl", framebufferRenderbuffer));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"framebufferTexture2D", L"webgl", framebufferTexture2D));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"createTexture", L"webgl", createTexture));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"bindTexture", L"webgl", bindTexture));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"texParameteri", L"webgl", texParameteri));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"texImage2D1", L"webgl", texImage2D1));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"texImage2D2", L"webgl", texImage2D2));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"texImage2D3", L"webgl", texImage2D3));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"texImage2D4", L"webgl", texImage2D4));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"activeTexture", L"webgl", activeTexture));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"generateMipmap", L"webgl", generateMipmap));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"pixelStorei", L"webgl", pixelStorei));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"clearDepth", L"webgl", clearDepth));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"clearStencil", L"webgl", clearStencil));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"enable", L"webgl", enable));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"disable", L"webgl", disable));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"depthFunc", L"webgl", depthFunc));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"depthMask", L"webgl", depthMask));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"depthRange", L"webgl", depthRange));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"frontFace", L"webgl", frontFace));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"cullFace", L"webgl", cullFace));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"blendColor", L"webgl", blendColor));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"blendEquation", L"webgl", blendEquation));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"blendEquationSeparate", L"webgl", blendEquationSeparate));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"blendFunc", L"webgl", blendFunc));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"blendFuncSeparate", L"webgl", blendFuncSeparate));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"scissor", L"webgl", scissor));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"viewport", L"webgl", viewport));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"clearColor", L"webgl", clearColor));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"clear", L"webgl", clear));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"createBuffer", L"webgl", createBuffer));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"deleteBuffer", L"webgl", deleteBuffer));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"bindBuffer", L"webgl", bindBuffer));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"bufferData1", L"webgl", bufferData1));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"bufferData2", L"webgl", bufferData2));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"colorMask", L"webgl", colorMask));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"drawArrays", L"webgl", drawArrays));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"drawElements", L"webgl", drawElements));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"enableVertexAttribArray", L"webgl", enableVertexAttribArray));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"disableVertexAttribArray", L"webgl", disableVertexAttribArray));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"vertexAttribPointer", L"webgl", vertexAttribPointer));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"createProgram", L"webgl", createProgram));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"useProgram", L"webgl", useProgram));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"validateProgram", L"webgl", validateProgram));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"linkProgram", L"webgl", linkProgram));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"getProgramParameter", L"webgl", getProgramParameter));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"getProgramInfoLog", L"webgl", getProgramInfoLog));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"deleteProgram", L"webgl", deleteProgram));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"bindAttribLocation", L"webgl", bindAttribLocation));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"getActiveUniform", L"webgl", getActiveUniform));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"getActiveAttrib", L"webgl", getActiveAttrib));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"getAttribLocation", L"webgl", getAttribLocation));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"createShader", L"webgl", createShader));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"shaderSource", L"webgl", shaderSource));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"compileShader", L"webgl", compileShader));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"getShaderParameter", L"webgl", getShaderParameter));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"getShaderInfoLog", L"webgl", getShaderInfoLog));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"attachShader", L"webgl", attachShader));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"deleteShader", L"webgl", deleteShader));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"getUniformLocation", L"webgl", getUniformLocation));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"uniform1f", L"webgl", uniform1f));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"uniform1fv", L"webgl", uniform1fv));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"uniform1i", L"webgl", uniform1i));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"uniform1iv", L"webgl", uniform1iv));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"uniform2f", L"webgl", uniform2f));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"uniform2fv", L"webgl", uniform2fv));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"uniform2i", L"webgl", uniform2i));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"uniform2iv", L"webgl", uniform2iv));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"uniform3f", L"webgl", uniform3f));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"uniform3fv", L"webgl", uniform3fv));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"uniform3i", L"webgl", uniform3i));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"uniform3iv", L"webgl", uniform3iv));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"uniform4f", L"webgl", uniform4f));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"uniform4fv", L"webgl", uniform4fv));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"uniform4i", L"webgl", uniform4i));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"uniform4iv", L"webgl", uniform4iv));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"uniformMatrix2fv", L"webgl", uniformMatrix2fv));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"uniformMatrix3fv", L"webgl", uniformMatrix3fv));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"uniformMatrix4fv", L"webgl", uniformMatrix4fv));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"stencilFunc", L"webgl", stencilFunc));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"stencilMask", L"webgl", stencilMask));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"stencilOp", L"webgl", stencilOp));

	// Canvas 2D context projections
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"drawImage1", L"canvas2d", drawImage1));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"drawImage2", L"canvas2d", drawImage2));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"drawImage3", L"canvas2d", drawImage3));
	RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"getImageData", L"canvas2d", getImageData));

	return true;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::createContext(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	return ScriptResourceTracker::ObjectToDirectExternal(new WebGLRenderingContext());
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::createContext2D(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	return ScriptResourceTracker::ObjectToDirectExternal(new RenderingContext2D());
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::getShaderPrecisionFormat(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLenum shadertype = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
	GLenum precisiontype = ScriptHostUtilities::GLenumFromJsRef(arguments[3]);
	auto returnObject = context->getShaderPrecisionFormat(shadertype, precisiontype);
	return ScriptResourceTracker::ObjectToExternal(returnObject);
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::getExtension(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	std::wstring name;
	RETURN_INVALID_REF_IF_FALSE(ScriptHostUtilities::GetString(arguments[2], name));
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::getParameter(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLenum pname = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
	return context->getParameter(pname);
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::createRenderbuffer(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	auto returnObject = context->createRenderbuffer();
	return ScriptResourceTracker::ObjectToExternal(returnObject);
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::bindRenderbuffer(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLenum target = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
	WebGLRenderbuffer* renderbuffer = ScriptResourceTracker::ExternalToObject<WebGLRenderbuffer>(arguments[3]);

	context->bindRenderbuffer(target, renderbuffer);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::renderbufferStorage(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 6);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLenum target = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
	GLenum internalformat = ScriptHostUtilities::GLenumFromJsRef(arguments[3]);
	GLsizei width = ScriptHostUtilities::GLsizeiFromJsRef(arguments[4]);
	GLsizei height = ScriptHostUtilities::GLsizeiFromJsRef(arguments[5]);
	context->renderbufferStorage(target, internalformat, width, height);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::createFramebuffer(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	auto returnObject = context->createFramebuffer();
	return ScriptResourceTracker::ObjectToExternal(returnObject);
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::bindFramebuffer(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLenum target = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
	WebGLFramebuffer* framebuffer = ScriptResourceTracker::ExternalToObject<WebGLFramebuffer>(arguments[3]);

	context->bindFramebuffer(target, framebuffer);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::framebufferRenderbuffer(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 6);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLenum target = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
	GLenum attachment = ScriptHostUtilities::GLenumFromJsRef(arguments[3]);
	GLenum renderbuffertarget = ScriptHostUtilities::GLenumFromJsRef(arguments[4]);
	WebGLRenderbuffer* renderbuffer = ScriptResourceTracker::ExternalToObject<WebGLRenderbuffer>(arguments[5]);
	RETURN_INVALID_REF_IF_NULL(renderbuffer);

	context->framebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::framebufferTexture2D(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 7);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLenum target = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
	GLenum attachment = ScriptHostUtilities::GLenumFromJsRef(arguments[3]);
	GLenum textarget = ScriptHostUtilities::GLenumFromJsRef(arguments[4]);
	WebGLTexture* texture = ScriptResourceTracker::ExternalToObject<WebGLTexture>(arguments[5]);
	RETURN_INVALID_REF_IF_NULL(texture);

	GLint level = ScriptHostUtilities::GLintFromJsRef(arguments[6]);
	context->framebufferTexture2D(target, attachment, textarget, texture, level);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::createTexture(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	auto returnObject = context->createTexture();
	return ScriptResourceTracker::ObjectToExternal(returnObject);
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::bindTexture(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLenum target = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
	WebGLTexture* texture = ScriptResourceTracker::ExternalToObject<WebGLTexture>(arguments[3]);

	context->bindTexture(target, texture);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::texParameteri(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLenum target = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
	GLenum pname = ScriptHostUtilities::GLenumFromJsRef(arguments[3]);
	GLint param = ScriptHostUtilities::GLintFromJsRef(arguments[4]);
	context->texParameteri(target, pname, param);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::texImage2D1(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 11);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLenum target = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
	GLint level = ScriptHostUtilities::GLintFromJsRef(arguments[3]);
	GLenum internalformat = ScriptHostUtilities::GLenumFromJsRef(arguments[4]);
	GLsizei width = ScriptHostUtilities::GLsizeiFromJsRef(arguments[5]);
	GLsizei height = ScriptHostUtilities::GLsizeiFromJsRef(arguments[6]);
	GLint border = ScriptHostUtilities::GLintFromJsRef(arguments[7]);
	GLenum format = ScriptHostUtilities::GLenumFromJsRef(arguments[8]);
	GLenum type = ScriptHostUtilities::GLenumFromJsRef(arguments[9]);

	JsValueType arrayRefType;
	RETURN_NULL_IF_JS_ERROR(JsGetValueType(arguments[10], &arrayRefType));
	if (arrayRefType != JsValueType::JsNull)
	{
		ChakraBytePtr pixels; unsigned int pixelsLength; int arrayElementSize; JsTypedArrayType arrayType;
        RETURN_NULL_IF_JS_ERROR(JsGetTypedArrayStorage(arguments[10], &pixels, &pixelsLength, &arrayType, &arrayElementSize));
		context->texImage2D(target, level, internalformat, width, height, border, format, type, pixels);
	}
	else
	{
		context->texImage2D(target, level, internalformat, width, height, border, format, type, nullptr);
	}
	
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::texImage2D2(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 10);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLenum target = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
	GLint level = ScriptHostUtilities::GLintFromJsRef(arguments[3]);
	GLenum internalformat = ScriptHostUtilities::GLenumFromJsRef(arguments[4]);

	GLsizei width = ScriptHostUtilities::GLsizeiFromJsRef(arguments[5]);
	GLsizei height = ScriptHostUtilities::GLsizeiFromJsRef(arguments[6]);

	GLenum format = ScriptHostUtilities::GLenumFromJsRef(arguments[7]);
	GLenum type = ScriptHostUtilities::GLenumFromJsRef(arguments[8]);

	auto imageElement = ScriptResourceTracker::ExternalToObject<API::ImageElement>(arguments[9]);
	RETURN_INVALID_REF_IF_NULL(imageElement);

	unsigned int imageBufferSize = 0;
	WICInProcPointer imageMemory = nullptr;
	unsigned int stride;
	RETURN_INVALID_REF_IF_FALSE(imageElement->GetPixelsPointer(&imageMemory, &imageBufferSize, &stride));
	
	context->texImage2D(target, level, internalformat, width, height, 0, format, type, imageMemory);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::texImage2D3(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 10);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLenum target = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
	GLint level = ScriptHostUtilities::GLintFromJsRef(arguments[3]);
	GLenum internalformat = ScriptHostUtilities::GLenumFromJsRef(arguments[4]);

	GLsizei width = ScriptHostUtilities::GLsizeiFromJsRef(arguments[5]);
	GLsizei height = ScriptHostUtilities::GLsizeiFromJsRef(arguments[6]);

	GLenum format = ScriptHostUtilities::GLenumFromJsRef(arguments[7]);
	GLenum type = ScriptHostUtilities::GLenumFromJsRef(arguments[8]);

	RenderingContext2D* context2D = ScriptResourceTracker::ExternalToObject<RenderingContext2D>(arguments[9]);
	RETURN_INVALID_REF_IF_NULL(context2D);

	auto pixels = context2D->getImageData(0, 0, width, height);

	context->texImage2D(target, level, internalformat, width, height, 0, format, type, pixels->Data);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::texImage2D4(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 10);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLenum target = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
	GLint level = ScriptHostUtilities::GLintFromJsRef(arguments[3]);
	GLenum internalformat = ScriptHostUtilities::GLenumFromJsRef(arguments[4]);

	GLsizei width = ScriptHostUtilities::GLsizeiFromJsRef(arguments[5]);
	GLsizei height = ScriptHostUtilities::GLsizeiFromJsRef(arguments[6]);

	GLenum format = ScriptHostUtilities::GLenumFromJsRef(arguments[7]);
	GLenum type = ScriptHostUtilities::GLenumFromJsRef(arguments[8]);

	auto videoElement= ScriptResourceTracker::ExternalToObject<VideoElement>(arguments[9]);
	RETURN_INVALID_REF_IF_NULL(videoElement);

	void* pixels;
	unsigned int pixelsSize;
    if (videoElement->IsNewFrameAvailable())
    {
        DWORD64 start = GetTickCount64();
        RETURN_INVALID_REF_IF_FALSE(videoElement->LockNextFrame(&pixels, &pixelsSize));
        std::wstring endString = std::to_wstring(GetTickCount64() - start) + L"\n";
        OutputDebugStringW(endString.c_str());
        context->texImage2DNoFlip(target, level, internalformat, width, height, 0, format, type, pixels);
        videoElement->UnlockFrame();
    }

	return JS_INVALID_REFERENCE;
}


JsValueRef
CHAKRA_CALLBACK
WebGLProjections::activeTexture(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLenum texture = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
	context->activeTexture(texture);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::generateMipmap(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLenum target = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
	context->generateMipmap(target);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::pixelStorei(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLenum pname = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
	GLint param = ScriptHostUtilities::GLintFromJsRef(arguments[3]);
	context->pixelStorei(pname, param);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::clearDepth(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLclampf depth = ScriptHostUtilities::GLclampfFromJsRef(arguments[2]);
	context->clearDepth(depth);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::clearStencil(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLint s = ScriptHostUtilities::GLintFromJsRef(arguments[2]);
	context->clearStencil(s);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::enable(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLenum cap = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
	context->enable(cap);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::disable(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLenum cap = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
	context->disable(cap);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::depthFunc(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLenum func = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
	context->depthFunc(func);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::depthMask(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLboolean flag = ScriptHostUtilities::GLbooleanFromJsRef(arguments[2]);
	context->depthMask(flag);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::depthRange(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLclampf zNear = ScriptHostUtilities::GLclampfFromJsRef(arguments[2]);
	GLclampf zFar = ScriptHostUtilities::GLclampfFromJsRef(arguments[3]);
	context->depthRange(zNear, zFar);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::frontFace(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLenum mode = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
	context->frontFace(mode);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::cullFace(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLenum mode = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
	context->cullFace(mode);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::blendColor(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 6);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLclampf red = ScriptHostUtilities::GLclampfFromJsRef(arguments[2]);
	GLclampf green = ScriptHostUtilities::GLclampfFromJsRef(arguments[3]);
	GLclampf blue = ScriptHostUtilities::GLclampfFromJsRef(arguments[4]);
	GLclampf alpha = ScriptHostUtilities::GLclampfFromJsRef(arguments[5]);
	context->blendColor(red, green, blue, alpha);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::blendEquation(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLenum mode = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
	context->blendEquation(mode);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::blendEquationSeparate(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLenum modeRGB = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
	GLenum modeAlpha = ScriptHostUtilities::GLenumFromJsRef(arguments[3]);
	context->blendEquationSeparate(modeRGB, modeAlpha);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::blendFunc(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLenum sfactor = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
	GLenum dfactor = ScriptHostUtilities::GLenumFromJsRef(arguments[3]);
	context->blendFunc(sfactor, dfactor);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::blendFuncSeparate(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 6);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLenum srcRGB = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
	GLenum dstRGB = ScriptHostUtilities::GLenumFromJsRef(arguments[3]);
	GLenum srcAlpha = ScriptHostUtilities::GLenumFromJsRef(arguments[4]);
	GLenum dstAlpha = ScriptHostUtilities::GLenumFromJsRef(arguments[5]);
	context->blendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::scissor(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 6);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLint x = ScriptHostUtilities::GLintFromJsRef(arguments[2]);
	GLint y = ScriptHostUtilities::GLintFromJsRef(arguments[3]);
	GLsizei width = ScriptHostUtilities::GLsizeiFromJsRef(arguments[4]);
	GLsizei height = ScriptHostUtilities::GLsizeiFromJsRef(arguments[5]);
	context->scissor(x, y, width, height);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::viewport(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 6);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLint x = ScriptHostUtilities::GLintFromJsRef(arguments[2]);
	GLint y = ScriptHostUtilities::GLintFromJsRef(arguments[3]);
	GLsizei width = ScriptHostUtilities::GLsizeiFromJsRef(arguments[4]);
	GLsizei height = ScriptHostUtilities::GLsizeiFromJsRef(arguments[5]);
	context->viewport(x, y, width, height);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::clearColor(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 6);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLclampf red = ScriptHostUtilities::GLclampfFromJsRef(arguments[2]);
	GLclampf green = ScriptHostUtilities::GLclampfFromJsRef(arguments[3]);
	GLclampf blue = ScriptHostUtilities::GLclampfFromJsRef(arguments[4]);
	GLclampf alpha = ScriptHostUtilities::GLclampfFromJsRef(arguments[5]);
	context->clearColor(red, green, blue, alpha);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::clear(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLbitfield mask = ScriptHostUtilities::GLbitfieldFromJsRef(arguments[2]);
	context->clear(mask);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::createBuffer(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	auto returnObject = context->createBuffer();
	return ScriptResourceTracker::ObjectToExternal(returnObject);
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::deleteBuffer(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLBuffer* buffer = ScriptResourceTracker::ExternalToObject<WebGLBuffer>(arguments[2]);

	context->deleteBuffer(buffer);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::bindBuffer(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLenum target = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);

	WebGLBuffer* buffer = ScriptResourceTracker::ExternalToObject<WebGLBuffer>(arguments[3]);

	context->bindBuffer(target, buffer);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::bufferData1(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLenum target = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
	GLsizeiptr size = ScriptHostUtilities::GLsizeiptrFromJsRef(arguments[3]);
	GLenum usage = ScriptHostUtilities::GLenumFromJsRef(arguments[4]);
	context->bufferData(target, size, nullptr, usage);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::bufferData2(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLenum target = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
	
	ChakraBytePtr data; unsigned int dataLength; int arrayElementSize; JsTypedArrayType arrayType;
	JsGetTypedArrayStorage(arguments[3], &data, &dataLength, &arrayType, &arrayElementSize);
	GLenum usage = ScriptHostUtilities::GLenumFromJsRef(arguments[4]);
	context->bufferData(target, dataLength, data, usage);
	return JS_INVALID_REFERENCE;
}


JsValueRef
CHAKRA_CALLBACK
WebGLProjections::colorMask(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 6);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLboolean red = ScriptHostUtilities::GLbooleanFromJsRef(arguments[2]);
	GLboolean green = ScriptHostUtilities::GLbooleanFromJsRef(arguments[3]);
	GLboolean blue = ScriptHostUtilities::GLbooleanFromJsRef(arguments[4]);
	GLboolean alpha = ScriptHostUtilities::GLbooleanFromJsRef(arguments[5]);
	context->colorMask(red, green, blue, alpha);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::drawArrays(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLenum mode = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
	GLint first = ScriptHostUtilities::GLintFromJsRef(arguments[3]);
	GLsizei count = ScriptHostUtilities::GLsizeiFromJsRef(arguments[4]);
	context->drawArrays(mode, first, count);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::drawElements(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 6);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLenum mode = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
	GLsizei count = ScriptHostUtilities::GLsizeiFromJsRef(arguments[3]);
	GLenum type = ScriptHostUtilities::GLenumFromJsRef(arguments[4]);
	GLint offset = ScriptHostUtilities::GLintFromJsRef(arguments[5]);
	context->drawElements(mode, count, type, offset);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::enableVertexAttribArray(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLuint index = ScriptHostUtilities::GLuintFromJsRef(arguments[2]);
	context->enableVertexAttribArray(index);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::disableVertexAttribArray(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLuint index = ScriptHostUtilities::GLuintFromJsRef(arguments[2]);
	context->disableVertexAttribArray(index);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::vertexAttribPointer(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 8);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
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

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::createProgram(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	auto returnObject = context->createProgram();
	return ScriptResourceTracker::ObjectToExternal(returnObject);
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::useProgram(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLProgram* program = ScriptResourceTracker::ExternalToObject<WebGLProgram>(arguments[2]);
	RETURN_INVALID_REF_IF_NULL(program);

	context->useProgram(program);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::validateProgram(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLProgram* program = ScriptResourceTracker::ExternalToObject<WebGLProgram>(arguments[2]);
	RETURN_INVALID_REF_IF_NULL(program);

	context->validateProgram(program);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::linkProgram(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLProgram* program = ScriptResourceTracker::ExternalToObject<WebGLProgram>(arguments[2]);
	RETURN_INVALID_REF_IF_NULL(program);

	context->linkProgram(program);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::getProgramParameter(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLProgram* program = ScriptResourceTracker::ExternalToObject<WebGLProgram>(arguments[2]);
	RETURN_INVALID_REF_IF_NULL(program);

	GLenum pname = ScriptHostUtilities::GLenumFromJsRef(arguments[3]);
	return context->getProgramParameter(program, pname);
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::getProgramInfoLog(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLProgram* program = ScriptResourceTracker::ExternalToObject<WebGLProgram>(arguments[2]);
	RETURN_INVALID_REF_IF_NULL(program);


	auto log = context->getProgramInfoLog(program);
	JsValueRef retValue;
	RETURN_NULL_IF_JS_ERROR(JsPointerToString(log.c_str(), log.length(), &retValue));

	return retValue;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::deleteProgram(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLProgram* program = ScriptResourceTracker::ExternalToObject<WebGLProgram>(arguments[2]);
	RETURN_INVALID_REF_IF_NULL(program);

	context->deleteProgram(program);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::bindAttribLocation(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLProgram* program = ScriptResourceTracker::ExternalToObject<WebGLProgram>(arguments[2]);
	RETURN_INVALID_REF_IF_NULL(program);

	GLuint index = ScriptHostUtilities::GLuintFromJsRef(arguments[3]);
	std::wstring name;
	RETURN_INVALID_REF_IF_FALSE(ScriptHostUtilities::GetString(arguments[4], name));
	context->bindAttribLocation(program, index, name);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::getActiveUniform(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLProgram* program = ScriptResourceTracker::ExternalToObject<WebGLProgram>(arguments[2]);
	RETURN_INVALID_REF_IF_NULL(program);

	GLuint index = ScriptHostUtilities::GLuintFromJsRef(arguments[3]);
	auto returnObject = context->getActiveUniform(program, index);
	return ScriptResourceTracker::ObjectToExternal(returnObject);
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::getActiveAttrib(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLProgram* program = ScriptResourceTracker::ExternalToObject<WebGLProgram>(arguments[2]);
	RETURN_INVALID_REF_IF_NULL(program);

	GLuint index = ScriptHostUtilities::GLuintFromJsRef(arguments[3]);
	auto returnObject = context->getActiveAttrib(program, index);
	return ScriptResourceTracker::ObjectToExternal(returnObject);
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::getAttribLocation(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLProgram* program = ScriptResourceTracker::ExternalToObject<WebGLProgram>(arguments[2]);
	RETURN_INVALID_REF_IF_NULL(program);

	std::wstring name;
	RETURN_INVALID_REF_IF_FALSE(ScriptHostUtilities::GetString(arguments[3], name));

	JsValueRef retValue;
	RETURN_NULL_IF_JS_ERROR(JsIntToNumber(context->getAttribLocation(program, name), &retValue));
	return retValue;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::createShader(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	GLenum type = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
	auto returnObject = context->createShader(type);
	return ScriptResourceTracker::ObjectToExternal(returnObject);
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::shaderSource(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLShader* shader = ScriptResourceTracker::ExternalToObject<WebGLShader>(arguments[2]);
	RETURN_INVALID_REF_IF_NULL(shader);

	std::wstring source;
	RETURN_INVALID_REF_IF_FALSE(ScriptHostUtilities::GetString(arguments[3], source));
	context->shaderSource(shader, source);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::compileShader(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLShader* shader = ScriptResourceTracker::ExternalToObject<WebGLShader>(arguments[2]);
	RETURN_INVALID_REF_IF_NULL(shader);

	context->compileShader(shader);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::getShaderParameter(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLShader* shader = ScriptResourceTracker::ExternalToObject<WebGLShader>(arguments[2]);
	RETURN_INVALID_REF_IF_NULL(shader);

	GLenum pname = ScriptHostUtilities::GLenumFromJsRef(arguments[3]);
	return context->getShaderParameter(shader, pname);
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::getShaderInfoLog(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLShader* shader = ScriptResourceTracker::ExternalToObject<WebGLShader>(arguments[2]);
	RETURN_INVALID_REF_IF_NULL(shader);

	auto log = context->getShaderInfoLog(shader);

	JsValueRef logValue;
	JsPointerToString(log.c_str(), log.length(), &logValue);

	return logValue;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::attachShader(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLProgram* program = ScriptResourceTracker::ExternalToObject<WebGLProgram>(arguments[2]);
	RETURN_INVALID_REF_IF_NULL(program);

	WebGLShader* shader = ScriptResourceTracker::ExternalToObject<WebGLShader>(arguments[3]);
	RETURN_INVALID_REF_IF_NULL(shader);

	context->attachShader(program, shader);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::deleteShader(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLShader* shader = ScriptResourceTracker::ExternalToObject<WebGLShader>(arguments[2]);
	RETURN_INVALID_REF_IF_NULL(shader);

	context->deleteShader(shader);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::getUniformLocation(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLProgram* program = ScriptResourceTracker::ExternalToObject<WebGLProgram>(arguments[2]);
	RETURN_INVALID_REF_IF_NULL(program);

	std::wstring name;
	RETURN_INVALID_REF_IF_FALSE(ScriptHostUtilities::GetString(arguments[3], name));
	auto returnObject = context->getUniformLocation(program, name);

	return ScriptResourceTracker::ObjectToExternal(returnObject);
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::uniform1f(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLUniformLocation* location = ScriptResourceTracker::ExternalToObject<WebGLUniformLocation>(arguments[2]);
	RETURN_INVALID_REF_IF_NULL(location);

	GLfloat x = ScriptHostUtilities::GLfloatFromJsRef(arguments[3]);
	context->uniform1f(location, x);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::uniform1fv(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLUniformLocation* location = ScriptResourceTracker::ExternalToObject<WebGLUniformLocation>(arguments[2]);
	RETURN_INVALID_REF_IF_NULL(location);

	ChakraBytePtr v; unsigned int vLength; int arrayElementSize; JsTypedArrayType arrayType;
	JsGetTypedArrayStorage(arguments[3], &v, &vLength, &arrayType, &arrayElementSize);
	context->uniform1fv(location, vLength, reinterpret_cast<const GLfloat*>(v));
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::uniform1i(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLUniformLocation* location = ScriptResourceTracker::ExternalToObject<WebGLUniformLocation>(arguments[2]);
	RETURN_INVALID_REF_IF_NULL(location);

	GLint x = ScriptHostUtilities::GLintFromJsRef(arguments[3]);
	context->uniform1i(location, x);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::uniform1iv(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLUniformLocation* location = ScriptResourceTracker::ExternalToObject<WebGLUniformLocation>(arguments[2]);
	RETURN_INVALID_REF_IF_NULL(location);

	ChakraBytePtr v; unsigned int vLength; int arrayElementSize; JsTypedArrayType arrayType;
	JsGetTypedArrayStorage(arguments[3], &v, &vLength, &arrayType, &arrayElementSize);
	context->uniform1iv(location, vLength, reinterpret_cast<const GLint*>(v));
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::uniform2f(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLUniformLocation* location = ScriptResourceTracker::ExternalToObject<WebGLUniformLocation>(arguments[2]);
	RETURN_INVALID_REF_IF_NULL(location);

	GLfloat x = ScriptHostUtilities::GLfloatFromJsRef(arguments[3]);
	GLfloat y = ScriptHostUtilities::GLfloatFromJsRef(arguments[4]);
	context->uniform2f(location, x, y);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::uniform2fv(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLUniformLocation* location = ScriptResourceTracker::ExternalToObject<WebGLUniformLocation>(arguments[2]);
	RETURN_INVALID_REF_IF_NULL(location);

	ChakraBytePtr v; unsigned int vLength; int arrayElementSize; JsTypedArrayType arrayType;
	JsGetTypedArrayStorage(arguments[3], &v, &vLength, &arrayType, &arrayElementSize);
	context->uniform2fv(location, vLength, reinterpret_cast<const GLfloat*>(v));
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::uniform2i(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLUniformLocation* location = ScriptResourceTracker::ExternalToObject<WebGLUniformLocation>(arguments[2]);
	RETURN_INVALID_REF_IF_NULL(location);

	GLint x = ScriptHostUtilities::GLintFromJsRef(arguments[3]);
	GLint y = ScriptHostUtilities::GLintFromJsRef(arguments[4]);
	context->uniform2i(location, x, y);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::uniform2iv(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLUniformLocation* location = ScriptResourceTracker::ExternalToObject<WebGLUniformLocation>(arguments[2]);
	RETURN_INVALID_REF_IF_NULL(location);

	ChakraBytePtr v; unsigned int vLength; int arrayElementSize; JsTypedArrayType arrayType;
	JsGetTypedArrayStorage(arguments[3], &v, &vLength, &arrayType, &arrayElementSize);
	context->uniform2iv(location, vLength, reinterpret_cast<const GLint*>(v));
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::uniform3f(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 6);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLUniformLocation* location = ScriptResourceTracker::ExternalToObject<WebGLUniformLocation>(arguments[2]);
	RETURN_INVALID_REF_IF_NULL(location);

	GLfloat x = ScriptHostUtilities::GLfloatFromJsRef(arguments[3]);
	GLfloat y = ScriptHostUtilities::GLfloatFromJsRef(arguments[4]);
	GLfloat z = ScriptHostUtilities::GLfloatFromJsRef(arguments[5]);
	context->uniform3f(location, x, y, z);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::uniform3fv(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLUniformLocation* location = ScriptResourceTracker::ExternalToObject<WebGLUniformLocation>(arguments[2]);
	RETURN_INVALID_REF_IF_NULL(location);

	ChakraBytePtr v; unsigned int vLength; int arrayElementSize; JsTypedArrayType arrayType;
	JsGetTypedArrayStorage(arguments[3], &v, &vLength, &arrayType, &arrayElementSize);
	context->uniform3fv(location, vLength, reinterpret_cast<const GLfloat*>(v));
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::uniform3i(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 6);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLUniformLocation* location = ScriptResourceTracker::ExternalToObject<WebGLUniformLocation>(arguments[2]);
	RETURN_INVALID_REF_IF_NULL(location);

	GLint x = ScriptHostUtilities::GLintFromJsRef(arguments[3]);
	GLint y = ScriptHostUtilities::GLintFromJsRef(arguments[4]);
	GLint z = ScriptHostUtilities::GLintFromJsRef(arguments[5]);
	context->uniform3i(location, x, y, z);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::uniform3iv(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLUniformLocation* location = ScriptResourceTracker::ExternalToObject<WebGLUniformLocation>(arguments[2]);
	RETURN_INVALID_REF_IF_NULL(location);

	ChakraBytePtr v; unsigned int vLength; int arrayElementSize; JsTypedArrayType arrayType;
	JsGetTypedArrayStorage(arguments[3], &v, &vLength, &arrayType, &arrayElementSize);
	context->uniform3iv(location, vLength, reinterpret_cast<const GLint*>(v));
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::uniform4f(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 7);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLUniformLocation* location = ScriptResourceTracker::ExternalToObject<WebGLUniformLocation>(arguments[2]);
	RETURN_INVALID_REF_IF_NULL(location);

	GLfloat x = ScriptHostUtilities::GLfloatFromJsRef(arguments[3]);
	GLfloat y = ScriptHostUtilities::GLfloatFromJsRef(arguments[4]);
	GLfloat z = ScriptHostUtilities::GLfloatFromJsRef(arguments[5]);
	GLfloat w = ScriptHostUtilities::GLfloatFromJsRef(arguments[6]);
	context->uniform4f(location, x, y, z, w);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::uniform4fv(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLUniformLocation* location = ScriptResourceTracker::ExternalToObject<WebGLUniformLocation>(arguments[2]);
	RETURN_INVALID_REF_IF_NULL(location);

	ChakraBytePtr v; unsigned int vLength; int arrayElementSize; JsTypedArrayType arrayType;
	JsGetTypedArrayStorage(arguments[3], &v, &vLength, &arrayType, &arrayElementSize);
	context->uniform4fv(location, vLength, reinterpret_cast<const GLfloat*>(v));
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::uniform4i(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 7);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLUniformLocation* location = ScriptResourceTracker::ExternalToObject<WebGLUniformLocation>(arguments[2]);
	RETURN_INVALID_REF_IF_NULL(location);

	GLint x = ScriptHostUtilities::GLintFromJsRef(arguments[3]);
	GLint y = ScriptHostUtilities::GLintFromJsRef(arguments[4]);
	GLint z = ScriptHostUtilities::GLintFromJsRef(arguments[5]);
	GLint w = ScriptHostUtilities::GLintFromJsRef(arguments[6]);
	context->uniform4i(location, x, y, z, w);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::uniform4iv(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLUniformLocation* location = ScriptResourceTracker::ExternalToObject<WebGLUniformLocation>(arguments[2]);
	RETURN_INVALID_REF_IF_NULL(location);

	ChakraBytePtr v; unsigned int vLength; int arrayElementSize; JsTypedArrayType arrayType;
	JsGetTypedArrayStorage(arguments[3], &v, &vLength, &arrayType, &arrayElementSize);
	context->uniform4iv(location, vLength, reinterpret_cast<const GLint*>(v));
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::uniformMatrix2fv(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLUniformLocation* location = ScriptResourceTracker::ExternalToObject<WebGLUniformLocation>(arguments[2]);
	RETURN_INVALID_REF_IF_NULL(location);

	GLboolean transpose = ScriptHostUtilities::GLbooleanFromJsRef(arguments[3]);
	ChakraBytePtr value; unsigned int valueLength; int arrayElementSize; JsTypedArrayType arrayType;
	JsGetTypedArrayStorage(arguments[4], &value, &valueLength, &arrayType, &arrayElementSize);
	context->uniformMatrix2fv(location, transpose, valueLength, reinterpret_cast<const GLfloat*>(value));
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::uniformMatrix3fv(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLUniformLocation* location = ScriptResourceTracker::ExternalToObject<WebGLUniformLocation>(arguments[2]);
	RETURN_INVALID_REF_IF_NULL(location);

	GLboolean transpose = ScriptHostUtilities::GLbooleanFromJsRef(arguments[3]);
	ChakraBytePtr value; unsigned int valueLength; int arrayElementSize; JsTypedArrayType arrayType;
	JsGetTypedArrayStorage(arguments[4], &value, &valueLength, &arrayType, &arrayElementSize);
	context->uniformMatrix3fv(location, transpose, valueLength, reinterpret_cast<const GLfloat*>(value));
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::uniformMatrix4fv(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

	WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	WebGLUniformLocation* location = ScriptResourceTracker::ExternalToObject<WebGLUniformLocation>(arguments[2]);
	RETURN_INVALID_REF_IF_NULL(location);

	GLboolean transpose = ScriptHostUtilities::GLbooleanFromJsRef(arguments[3]);
	ChakraBytePtr value; unsigned int valueLength; int arrayElementSize; JsTypedArrayType arrayType;
	JsGetTypedArrayStorage(arguments[4], &value, &valueLength, &arrayType, &arrayElementSize);
	context->uniformMatrix4fv(location, transpose, valueLength, reinterpret_cast<const GLfloat*>(value));
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::drawImage1(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 9);

	RenderingContext2D* context = ScriptResourceTracker::ExternalToObject<RenderingContext2D>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	int canvasWidth = ScriptHostUtilities::GLintFromJsRef(arguments[2]);
	int canvasHeight = ScriptHostUtilities::GLintFromJsRef(arguments[3]);
	context->SetSize(canvasWidth, canvasHeight);

	auto imageElement = ScriptResourceTracker::ExternalToObject<API::ImageElement>(arguments[4]);

	int imageWidth = ScriptHostUtilities::GLintFromJsRef(arguments[5]);
	int imageHeight = ScriptHostUtilities::GLintFromJsRef(arguments[6]);

	int dx = ScriptHostUtilities::GLintFromJsRef(arguments[7]);
	int dy = ScriptHostUtilities::GLintFromJsRef(arguments[8]);

	context->drawImage1(imageElement, imageWidth, imageHeight, dx, dy);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::drawImage2(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 11);

	RenderingContext2D* context = ScriptResourceTracker::ExternalToObject<RenderingContext2D>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

	int canvasWidth = ScriptHostUtilities::GLintFromJsRef(arguments[2]);
	int canvasHeight = ScriptHostUtilities::GLintFromJsRef(arguments[3]);
	context->SetSize(canvasWidth, canvasHeight);

	auto imageElement = ScriptResourceTracker::ExternalToObject<API::ImageElement>(arguments[4]);

	int imageWidth = ScriptHostUtilities::GLintFromJsRef(arguments[5]);
	int imageHeight = ScriptHostUtilities::GLintFromJsRef(arguments[6]);

	int dx = ScriptHostUtilities::GLintFromJsRef(arguments[7]);
	int dy = ScriptHostUtilities::GLintFromJsRef(arguments[8]);
	int dWidth = ScriptHostUtilities::GLintFromJsRef(arguments[9]);
	int dHeight = ScriptHostUtilities::GLintFromJsRef(arguments[10]);

	context->drawImage2(imageElement, imageWidth, imageHeight, dx, dx, dWidth, dHeight);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::drawImage3(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 15);

	RenderingContext2D* context = ScriptResourceTracker::ExternalToObject<RenderingContext2D>(arguments[1]);
	RETURN_INVALID_REF_IF_NULL(context);

    const auto canvasWidth = ScriptHostUtilities::GLintFromJsRef(arguments[2]);
	const auto canvasHeight = ScriptHostUtilities::GLintFromJsRef(arguments[3]);
	context->SetSize(canvasWidth, canvasHeight);

	auto imageElement = ScriptResourceTracker::ExternalToObject<API::ImageElement>(arguments[4]);

	int imageWidth = ScriptHostUtilities::GLintFromJsRef(arguments[5]);
	int imageHeight = ScriptHostUtilities::GLintFromJsRef(arguments[6]);

	int sx = ScriptHostUtilities::GLintFromJsRef(arguments[7]);
	int sy = ScriptHostUtilities::GLintFromJsRef(arguments[8]);
	int sWidth = ScriptHostUtilities::GLintFromJsRef(arguments[9]);
	int sHeight = ScriptHostUtilities::GLintFromJsRef(arguments[10]);

	int dx = ScriptHostUtilities::GLintFromJsRef(arguments[11]);
	int dy = ScriptHostUtilities::GLintFromJsRef(arguments[12]);
	int dWidth = ScriptHostUtilities::GLintFromJsRef(arguments[13]);
	int dHeight = ScriptHostUtilities::GLintFromJsRef(arguments[14]);

    
	context->drawImage3(imageElement, imageWidth, imageHeight, sx, sy, sWidth, sHeight, dx, dx, dWidth, dHeight);
	return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::getImageData(
	JsValueRef callee,
	bool isConstructCall,
	JsValueRef* arguments,
	unsigned short argumentCount,
	PVOID callbackData
)
{
	RETURN_INVALID_REF_IF_FALSE(argumentCount == 6);

	RenderingContext2D* context = ScriptResourceTracker::ExternalToObject<RenderingContext2D>(arguments[1]);
	
	RETURN_INVALID_REF_IF_NULL(context);

	int sx = ScriptHostUtilities::GLintFromJsRef(arguments[2]);
	int sy = ScriptHostUtilities::GLintFromJsRef(arguments[3]);
	int sw = ScriptHostUtilities::GLintFromJsRef(arguments[4]);
	int sh = ScriptHostUtilities::GLintFromJsRef(arguments[5]);

	// Create storage for the pixels in JS
	JsValueRef jsArray;
	if (context->OptimizedBufferAvailable())
	{
		RETURN_NULL_IF_JS_ERROR(JsCreateTypedArray(JsTypedArrayType::JsArrayTypeUint8Clamped, nullptr, 0, context->GetOptimizedBufferSize(), &jsArray));

		// Get a pointer to the newly allocated JS array
		ChakraBytePtr jsArrayPointer;
		JsTypedArrayType jsArrayType;
		unsigned int jsArrayLength;
		int jsArrayElementSize;
		RETURN_NULL_IF_JS_ERROR(JsGetTypedArrayStorage(jsArray, &jsArrayPointer, &jsArrayLength, &jsArrayType, &jsArrayElementSize));
		context->CopyOptimizedBitmapToBuffer(jsArrayPointer);
	}
	else
	{
		auto pixelsArray = context->getImageData(sx, sy, sw, sh);


		RETURN_NULL_IF_JS_ERROR(JsCreateTypedArray(JsTypedArrayType::JsArrayTypeUint8Clamped, nullptr, 0, pixelsArray->Length, &jsArray));

		// Get a pointer to the newly allocated JS array and copy the pixels there
		ChakraBytePtr jsArrayPointer;
		JsTypedArrayType jsArrayType;
		unsigned int jsArrayLength;
		int jsArrayElementSize;

		RETURN_NULL_IF_JS_ERROR(JsGetTypedArrayStorage(jsArray, &jsArrayPointer, &jsArrayLength, &jsArrayType, &jsArrayElementSize));

		CopyMemory(jsArrayPointer, pixelsArray->begin(), pixelsArray->Length);
	}

	return jsArray;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::stencilFunc(
    JsValueRef callee,
    bool isConstructCall,
    JsValueRef* arguments,
    unsigned short argumentCount,
    PVOID callbackData
)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

    WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
    RETURN_INVALID_REF_IF_NULL(context);

    GLenum func = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
    GLint ref = ScriptHostUtilities::GLintFromJsRef(arguments[3]);
    GLuint mask = ScriptHostUtilities::GLuintFromJsRef(arguments[4]);

    context->stencilFunc(func, ref, mask);

    return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::stencilMask(
    JsValueRef callee,
    bool isConstructCall,
    JsValueRef* arguments,
    unsigned short argumentCount,
    PVOID callbackData
)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
    RETURN_INVALID_REF_IF_NULL(context);

    GLuint mask = ScriptHostUtilities::GLuintFromJsRef(arguments[2]);
    context->stencilMask(mask);

    return JS_INVALID_REFERENCE;
}

JsValueRef
CHAKRA_CALLBACK
WebGLProjections::stencilOp(
    JsValueRef callee,
    bool isConstructCall,
    JsValueRef* arguments,
    unsigned short argumentCount,
    PVOID callbackData
)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 5);

    WebGLRenderingContext* context = ScriptResourceTracker::ExternalToObject<WebGLRenderingContext>(arguments[1]);
    RETURN_INVALID_REF_IF_NULL(context);

    GLenum  fail = ScriptHostUtilities::GLenumFromJsRef(arguments[2]);
    GLenum  zfail = ScriptHostUtilities::GLenumFromJsRef(arguments[3]);
    GLenum  zpass = ScriptHostUtilities::GLenumFromJsRef(arguments[4]);
    context->stencilOp(fail, zfail, zpass);

    return JS_INVALID_REFERENCE;
}
