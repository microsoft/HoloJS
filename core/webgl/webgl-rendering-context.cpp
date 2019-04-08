#include "webgl-rendering-context.h"
#include "../include/holojs/private/error-handling.h"
#include "../include/holojs/private/platform-interfaces.h"
#include "wincodec.h"
#include <GLES2/gl2ext.h>
#include <vector>

using namespace HoloJs::WebGL;
using namespace std;

WebGLTexture* WebGLRenderingContext::createTexture() { return new WebGLTexture(); }

void WebGLRenderingContext::deleteTexture(WebGLTexture* texture) { texture->Delete(); }

void WebGLRenderingContext::bindTexture(GLenum target, WebGLTexture* texture)
{
    if (texture == nullptr) {
        glBindTexture(target, 0);
    } else {
        glBindTexture(target, texture->id);
    }
}

void WebGLRenderingContext::texParameteri(GLenum target, GLenum pname, GLint param)
{
    glTexParameteri(target, pname, param);
}

void WebGLRenderingContext::texImage2D(GLenum target,
                                       GLint level,
                                       GLenum internalformat,
                                       GLsizei width,
                                       GLsizei height,
                                       GLint border,
                                       GLenum format,
                                       GLenum type,
                                       HoloJs::IIMage* image)
{
    EXIT_IF_TRUE(type != UNSIGNED_BYTE);
    static_assert(sizeof(WICPixelFormatGUID) == sizeof(IMAGE_FORMAT_GUID), "image format type mismatch");

    unsigned int imageBufferSize = 0;
    unsigned char* imageBuffer = nullptr;
    unsigned int stride;

    GUID imageDecodeFormat = (internalformat == GL_RGB ? GUID_WICPixelFormat24bppRGB : GUID_WICPixelFormat32bppRGBA);
    IMAGE_FORMAT_GUID holoJsImageFormat;
    memcpy(&holoJsImageFormat, &imageDecodeFormat, sizeof(imageDecodeFormat));

    HoloJs::ImageFlipRotation imageFlip =
        FlipYEnabled ? HoloJs::ImageFlipRotation::FlipY : HoloJs::ImageFlipRotation::None;
    EXIT_IF_FAILED(image->getImageData(holoJsImageFormat, &imageBuffer, imageBufferSize, stride, imageFlip));

    glTexImage2D(target, level, internalformat, width, height, border, format, type, imageBuffer);
}

void WebGLRenderingContext::texImage2D(GLenum target,
                                       GLint level,
                                       GLenum internalformat,
                                       GLsizei width,
                                       GLsizei height,
                                       GLint border,
                                       GLenum format,
                                       GLenum type,
                                       void* pixels,
                                       GLsizei stride)
{
    static std::vector<unsigned char> flipBuffer;
    if (type == UNSIGNED_BYTE) {
        if (FlipYEnabled && (stride != 0) && (pixels != nullptr)) {
            flipBuffer.reserve(height * stride);
            unsigned char* pixelsBuffer = reinterpret_cast<unsigned char*>(pixels);
            const size_t line_size = stride;
            const int half_height = height / 2;
            for (int i = 0; i < half_height; i++) {
                unsigned char* top_line_source = pixelsBuffer + i * line_size;
                unsigned char* bottom_line_source = pixelsBuffer + (height - i - 1) * line_size;
                unsigned char* top_line_destination = flipBuffer.data() + i * line_size;
                unsigned char* bottom_line_destination = flipBuffer.data() + (height - i - 1) * line_size;
                memcpy(bottom_line_destination, top_line_source, line_size);
                memcpy(top_line_destination, bottom_line_source, line_size);
            }

            glTexImage2D(target, level, internalformat, width, height, border, format, type, flipBuffer.data());
        } else {
            glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
        }

    } else {
        EXIT_IF_FAILED(HoloJs::Error);
    }
}

void WebGLRenderingContext::texImage2DNoFlip(GLenum target,
                                             GLint level,
                                             GLenum internalformat,
                                             GLsizei width,
                                             GLsizei height,
                                             GLint border,
                                             GLenum format,
                                             GLenum type,
                                             void* pixels)
{
    if (type == UNSIGNED_BYTE) {
        glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
    } else {
        EXIT_IF_FAILED(HoloJs::Error);
    }
}

void WebGLRenderingContext::activeTexture(GLenum texture) { glActiveTexture(texture); }

void WebGLRenderingContext::generateMipmap(GLenum target) { glGenerateMipmap(target); }

void WebGLRenderingContext::pixelStorei(GLenum pname, GLint param) { glPixelStorei(pname, param); }

void WebGLRenderingContext::pixelStorei(GLenum pname, GLboolean param)
{
    if (pname == UNPACK_FLIP_Y_WEBGL) {
        FlipYEnabled = (param == 0 ? false : true);
    }

    glPixelStorei(pname, param);
}

void WebGLRenderingContext::clearDepth(GLclampf depth) { glClearDepthf(depth); }

void WebGLRenderingContext::clearStencil(GLint s) { glClearStencil(s); }

void WebGLRenderingContext::enable(GLenum cap) { glEnable(cap); }

void WebGLRenderingContext::depthFunc(GLenum func) { glDepthFunc(func); }

void WebGLRenderingContext::depthMask(GLboolean flag) { glDepthMask(flag); }
void WebGLRenderingContext::depthRange(GLclampf zNear, GLclampf zFar) { glDepthRangef(zNear, zFar); }

void WebGLRenderingContext::frontFace(GLenum mode)
{
    if (isFrontFaceSwitched) {
		if (mode == GL_CCW) {
			glFrontFace(GL_CW);
		}
		else {
			glFrontFace(GL_CCW);
		}
	}
	else {
		glFrontFace(mode);
	}
}

void WebGLRenderingContext::cullFace(GLenum mode) { glCullFace(mode); }

void WebGLRenderingContext::blendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    glBlendColor(red, green, blue, alpha);
}

void WebGLRenderingContext::blendEquation(GLenum mode) { glBlendEquation(mode); }

void WebGLRenderingContext::blendEquationSeparate(GLenum modeRGB, GLenum modeAlpha)
{
    glBlendEquationSeparate(modeRGB, modeAlpha);
}

void WebGLRenderingContext::blendFunc(GLenum sfactor, GLenum dfactor) { glBlendFunc(sfactor, dfactor); }

void WebGLRenderingContext::blendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
{
    glBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
}

void WebGLRenderingContext::scissor(GLint x, GLint y, GLsizei width, GLsizei height) { glScissor(x, y, width, height); }

void WebGLRenderingContext::viewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    glViewport(x, y, width, height);
}

void WebGLRenderingContext::clearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    glClearColor(red, green, blue, alpha);
}

WebGLBuffer* WebGLRenderingContext::createBuffer() { return new WebGLBuffer(); }

void WebGLRenderingContext::deleteBuffer(WebGLBuffer* buffer)
{
    if (buffer != nullptr) {
        glDeleteBuffers(1, &buffer->id);
    }
}

void WebGLRenderingContext::bindBuffer(GLenum target, WebGLBuffer* buffer)
{
    if (buffer == nullptr) {
        glBindBuffer(target, 0);
    } else {
        buffer->Bind(target);
    }
}

void WebGLRenderingContext::bufferData(GLenum target, GLsizeiptr size, void* data, GLenum usage)
{
    glBufferData(target, size, data, usage);
}

void WebGLRenderingContext::bufferSubData(GLenum target, GLsizeiptr offset, void* data, GLsizeiptr dataSize)
{
    glBufferSubData(target, offset, dataSize, data);
}

void WebGLRenderingContext::disable(GLenum cap) { glDisable(cap); }

void WebGLRenderingContext::clear(GLbitfield mask) { glClear(mask); }

void WebGLRenderingContext::colorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
    glColorMask(red, green, blue, alpha);
}

WebGLProgram* WebGLRenderingContext::createProgram() { return new WebGLProgram(); }

WebGLShader* WebGLRenderingContext::createShader(GLenum type) { return new WebGLShader(type); }

void WebGLRenderingContext::shaderSource(WebGLShader* shader, std::wstring& source)
{
    string sourceAnsi(source.begin(), source.end());
    shader->SetSource(sourceAnsi.c_str());
}

void WebGLRenderingContext::compileShader(WebGLShader* shader) { shader->Compile(); }

JsValueRef WebGLRenderingContext::getShaderParameter(WebGLShader* shader, GLenum pname)
{
    return shader->GetParameter(pname);
}

std::wstring WebGLRenderingContext::getShaderInfoLog(WebGLShader* shader) { return shader->GetInfoLog(); }

void WebGLRenderingContext::attachShader(WebGLProgram* program, WebGLShader* shader) { program->AttachShader(shader); }

void WebGLRenderingContext::bindAttribLocation(WebGLProgram* program, GLuint index, std::wstring& name)
{
    string ansiName(name.begin(), name.end());
    program->BindAttribLocation(index, ansiName.c_str());
}

void WebGLRenderingContext::enableVertexAttribArray(GLuint index) { glEnableVertexAttribArray(index); }

void WebGLRenderingContext::disableVertexAttribArray(GLuint index) { glDisableVertexAttribArray(index); }

void WebGLRenderingContext::linkProgram(WebGLProgram* program) { program->Link(); }

JsValueRef WebGLRenderingContext::getProgramParameter(WebGLProgram* program, GLenum pname)
{
    return program->GetParameter(pname);
}

std::wstring WebGLRenderingContext::getProgramInfoLog(WebGLProgram* program) { return program->GetInfoLog(); }

void WebGLRenderingContext::deleteProgram(WebGLProgram* program) { program->Delete(); }

void WebGLRenderingContext::deleteShader(WebGLShader* shader) { shader->Delete(); }

WebGLActiveInfo* WebGLRenderingContext::getActiveUniform(WebGLProgram* program, GLuint index)
{
    return program->GetActiveUniform(index);
}

WebGLUniformLocation* WebGLRenderingContext::getUniformLocation(WebGLProgram* program, std::wstring& name)
{
    string ansiName(name.begin(), name.end());
    return program->GetUniformLocation(ansiName.c_str());
}

WebGLActiveInfo* WebGLRenderingContext::getActiveAttrib(WebGLProgram* program, GLuint index)
{
    return program->GetActiveAttrib(index);
}

GLint WebGLRenderingContext::getAttribLocation(WebGLProgram* program, std::wstring& name)
{
    string ansiName(name.begin(), name.end());
    return program->GetAttribLocation(ansiName.c_str());
}

void WebGLRenderingContext::useProgram(WebGLProgram* program) { program->Use(); }

void WebGLRenderingContext::validateProgram(WebGLProgram* program) { program->Validate(); }

void WebGLRenderingContext::uniform1f(WebGLUniformLocation* location, GLfloat x) { glUniform1f(location->location, x); }

void WebGLRenderingContext::uniform1fv(WebGLUniformLocation* location, GLsizei count, const GLfloat* v)
{
    glUniform1fv(location->location, count, v);
}

void WebGLRenderingContext::uniform1i(WebGLUniformLocation* location, GLint x) { glUniform1i(location->location, x); }

void WebGLRenderingContext::uniform1iv(WebGLUniformLocation* location, GLsizei count, const GLint* v)
{
    glUniform1iv(location->location, count, v);
}

void WebGLRenderingContext::uniform2f(WebGLUniformLocation* location, GLfloat x, GLfloat y)
{
    glUniform2f(location->location, x, y);
}

void WebGLRenderingContext::uniform2fv(WebGLUniformLocation* location, GLsizei count, const GLfloat* v)
{
    glUniform2fv(location->location, count, (GLfloat*)v);
}

void WebGLRenderingContext::uniform2i(WebGLUniformLocation* location, GLint x, GLint y)
{
    glUniform2i(location->location, x, y);
}

void WebGLRenderingContext::uniform2iv(WebGLUniformLocation* location, GLsizei count, const GLint* v)
{
    glUniform2iv(location->location, count, v);
}

void WebGLRenderingContext::uniform3f(WebGLUniformLocation* location, GLfloat x, GLfloat y, GLfloat z)
{
    glUniform3f(location->location, x, y, z);
}

void WebGLRenderingContext::uniform3fv(WebGLUniformLocation* location, GLsizei count, const GLfloat* v)
{
    glUniform3fv(location->location, count, v);
}

void WebGLRenderingContext::uniform3i(WebGLUniformLocation* location, GLint x, GLint y, GLint z)
{
    glUniform3i(location->location, x, y, z);
}

void WebGLRenderingContext::uniform3iv(WebGLUniformLocation* location, GLsizei count, const GLint* v)
{
    glUniform3iv(location->location, count, v);
}

void WebGLRenderingContext::uniform4f(WebGLUniformLocation* location, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    glUniform4f(location->location, x, y, z, w);
}

void WebGLRenderingContext::uniform4fv(WebGLUniformLocation* location, GLsizei count, const GLfloat* v)
{
    glUniform4fv(location->location, count, v);
}

void WebGLRenderingContext::uniform4i(WebGLUniformLocation* location, GLint x, GLint y, GLint z, GLint w)
{
    glUniform4i(location->location, x, y, z, w);
}

void WebGLRenderingContext::uniform4iv(WebGLUniformLocation* location, GLsizei count, const GLint* v)
{
    glUniform4iv(location->location, count, v);
}

void WebGLRenderingContext::uniformMatrix2fv(WebGLUniformLocation* location,
                                             GLboolean transpose,
                                             GLsizei count,
                                             const GLfloat* value)
{
    glUniformMatrix2fv(location->location, count, transpose, value);
}

void WebGLRenderingContext::uniformMatrix3fv(WebGLUniformLocation* location,
                                             GLboolean transpose,
                                             GLsizei count,
                                             const GLfloat* value)
{
    glUniformMatrix3fv(location->location, count, transpose, value);
}

void WebGLRenderingContext::uniformMatrix4fv(WebGLUniformLocation* location,
                                             GLboolean transpose,
                                             GLsizei count,
                                             const GLfloat* value)
{
    glUniformMatrix4fv(location->location, count, transpose, value);
}

void WebGLRenderingContext::drawArrays(GLenum mode, GLint first, GLsizei count) { glDrawArrays(mode, first, count); }

void WebGLRenderingContext::vertexAttribPointer(
    GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLint offset)
{
    glVertexAttribPointer(
        indx, size, type, normalized, stride, reinterpret_cast<const void*>(static_cast<size_t>(offset)));
}

void WebGLRenderingContext::drawElements(GLenum mode, GLsizei count, GLenum type, GLint offset)
{
    glDrawElements(mode, count, type, reinterpret_cast<const void*>(static_cast<size_t>(offset)));
}

WebGLRenderbuffer* WebGLRenderingContext::createRenderbuffer() { return new WebGLRenderbuffer(); }

void WebGLRenderingContext::bindRenderbuffer(GLenum target, WebGLRenderbuffer* renderbuffer)
{
    if (renderbuffer == nullptr) {
        glBindRenderbuffer(target, 0);
    } else {
        renderbuffer->bindRenderbuffer(target);
    }
}

void WebGLRenderingContext::renderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
    if (internalformat = GL_DEPTH_STENCIL_OES) {
        glRenderbufferStorage(target, GL_DEPTH24_STENCIL8_OES, width, height);
    } else {
        glRenderbufferStorage(target, internalformat, width, height);
    }
}

WebGLFramebuffer* WebGLRenderingContext::createFramebuffer() { return new WebGLFramebuffer(); }

void WebGLRenderingContext::bindFramebuffer(GLenum target, WebGLFramebuffer* framebuffer)
{
    if (framebuffer == nullptr) {
        glBindFramebuffer(target, 0);
    } else {
        framebuffer->bindFramebuffer(target);
    }
}

void WebGLRenderingContext::framebufferRenderbuffer(GLenum target,
                                                    GLenum attachment,
                                                    GLenum renderbuffertarget,
                                                    WebGLRenderbuffer* renderbuffer)
{
    if (renderbuffer == nullptr) {
        glFramebufferRenderbuffer(target, attachment, renderbuffertarget, 0);
    } else {
        renderbuffer->framebufferRenderbuffer(target, attachment, renderbuffertarget);
    }
}

void WebGLRenderingContext::framebufferTexture2D(
    GLenum target, GLenum attachment, GLenum textarget, WebGLTexture* texture, GLint level)
{
    if (texture == nullptr) {
        glFramebufferTexture2D(target, attachment, textarget, 0, level);
    } else {
        texture->framebufferTexture2D(target, attachment, textarget, level);
    }
}

JsValueRef WebGLRenderingContext::getParameter(GLenum pname)
{
    static int intParameters[] = {GL_MAX_VERTEX_ATTRIBS,
                                  GL_MAX_VERTEX_UNIFORM_VECTORS,
                                  GL_MAX_VARYING_VECTORS,
                                  GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
                                  GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
                                  GL_MAX_TEXTURE_IMAGE_UNITS,
                                  GL_MAX_FRAGMENT_UNIFORM_VECTORS,
                                  GL_MAX_TEXTURE_SIZE,
                                  GL_MAX_CUBE_MAP_TEXTURE_SIZE,
                                  GL_MAX_RENDERBUFFER_SIZE};

    bool isIntParameter = false;
    for (auto param : intParameters) {
        if (param == pname) {
            isIntParameter = true;
            break;
        }
    }

    if (isIntParameter) {
        GLint retValue;
        glGetIntegerv(pname, &retValue);
        JsValueRef retJsValue;
        RETURN_NULL_IF_JS_ERROR(JsIntToNumber(retValue, &retJsValue));
        return retJsValue;
    } else if (pname == UNPACK_FLIP_Y_WEBGL) {
        JsValueRef retJsValue;
        RETURN_NULL_IF_JS_ERROR(JsBoolToBoolean(FlipYEnabled, &retJsValue));
        return retJsValue;
    } else if (pname == UNPACK_PREMULTIPLY_ALPHA_WEBGL) {
        JsValueRef retJsValue;
        RETURN_NULL_IF_JS_ERROR(JsBoolToBoolean(false, &retJsValue));
        return retJsValue;
    } else if (pname == GL_VIEWPORT) {
        JsValueRef retArray;
        RETURN_NULL_IF_JS_ERROR(JsCreateTypedArray(JsTypedArrayType::JsArrayTypeInt32, nullptr, 0, 4, &retArray));
        JsTypedArrayType type;
        int elementSize;
        ChakraBytePtr buffer;
        unsigned int bufferLength;
        RETURN_NULL_IF_JS_ERROR(JsGetTypedArrayStorage(retArray, &buffer, &bufferLength, &type, &elementSize));

        glGetIntegerv(GL_VIEWPORT, reinterpret_cast<GLint*>(buffer));

        return retArray;
    } else if (pname == GL_VERSION) {
        wstring versionString = L"WebGL 1.0";
        JsValueRef retJsValue;
        JsPointerToString(versionString.c_str(), versionString.length(), &retJsValue);
        return retJsValue;
    } else {
        return nullptr;
    }
}

WebGLShaderPrecisionFormat* WebGLRenderingContext::getShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype)
{
    GLint range[2];
    GLint precision;
    glGetShaderPrecisionFormat(shadertype, precisiontype, range, &precision);
    return new WebGLShaderPrecisionFormat(range[0], range[1], precision);
}

void WebGLRenderingContext::stencilFunc(GLenum func, GLint ref, GLuint mask) { glStencilFunc(func, ref, mask); }

void WebGLRenderingContext::stencilMask(GLuint mask) { glStencilMask(mask); }

void WebGLRenderingContext::stencilOp(GLenum fail, GLenum zfail, GLenum zpass) { glStencilOp(fail, zfail, zpass); }

void WebGLRenderingContext::lineWidth(GLfloat width) { glLineWidth(width); }

void WebGLRenderingContext::polygonOffset(GLfloat factor, GLfloat units) { glPolygonOffset(factor, units); }
