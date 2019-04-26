#include "../include/holojs/private/error-handling.h"
#include "webgl-objects.h"
#include <vector>

// OpenGL ES includes
#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

using namespace HoloJs::WebGL;
using namespace std;

WebGLShaderPrecisionFormat::WebGLShaderPrecisionFormat(GLint min, GLint max, GLint precision)
{
    this->precision = precision;
    this->rangeMax = max;
    this->rangeMin = min;
}

void WebGLShaderPrecisionFormat::Release() {}

WebGLTexture::WebGLTexture() { glGenTextures(1, &id); }

void WebGLTexture::Release()
{
    if (!isReleased) {
        glDeleteTextures(1, &id);
        isReleased = true;
    }
}

void WebGLTexture::framebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLint level)
{
    glFramebufferTexture2D(target, attachment, textarget, this->id, level);
}

void WebGLTexture::Bind(GLenum target) { glBindTexture(target, id); }

WebGLBuffer::WebGLBuffer() { glGenBuffers(1, &id); }

void WebGLBuffer::Release() { glDeleteBuffers(1, &id); }

void WebGLBuffer::Bind(GLenum target) { glBindBuffer(target, id); }

WebGLProgram::WebGLProgram() { id = glCreateProgram(); }

void WebGLProgram::Release()
{
    if (!isReleased) {
        glDeleteProgram(id);
        isReleased = true;
    }
}

void WebGLProgram::AttachShader(WebGLShader* shader) { glAttachShader(id, shader->id); }

void WebGLProgram::BindAttribLocation(GLuint index, PCSTR name) { glBindAttribLocation(id, index, name); }

void WebGLProgram::Link() { glLinkProgram(id); }

JsValueRef WebGLProgram::GetParameter(GLenum pname)
{
    if (pname == GL_DELETE_STATUS || pname == GL_LINK_STATUS || pname == GL_VALIDATE_STATUS) {
        GLint retValue;
        glGetProgramiv(id, pname, &retValue);

        JsValueRef retJsValue;
        RETURN_NULL_IF_JS_ERROR(JsBoolToBoolean(retValue == 0 ? GL_FALSE : GL_TRUE, &retJsValue));
        return retJsValue;
    } else if (pname == GL_ATTACHED_SHADERS || pname == GL_ACTIVE_ATTRIBUTES || pname == GL_ACTIVE_UNIFORMS) {
        GLint retValue;
        glGetProgramiv(id, pname, &retValue);

        JsValueRef retJsValue;
        RETURN_NULL_IF_JS_ERROR(JsIntToNumber(retValue, &retJsValue));
        return retJsValue;
    } else {
        return nullptr;
    }
}

void WebGLProgram::Delete() { Release(); }

std::wstring WebGLProgram::GetInfoLog()
{
    GLint logLength;
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength == 0) {
        return L"";
    } else {
        vector<char> logBuffer(logLength);
        GLint actualLength = 0;
        glGetProgramInfoLog(id, logLength, &actualLength, logBuffer.data());
        wstring logUnicode(logBuffer.begin(), logBuffer.end());
        return logUnicode;
    }
}

WebGLActiveInfo* WebGLProgram::GetActiveUniform(GLuint index)
{
    WebGLActiveInfo* retValue = new WebGLActiveInfo();

    GLint nameLength;
    glGetProgramiv(id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &nameLength);

    if (nameLength > 0) {
        vector<GLchar> nameBufferAscii(nameLength);
        GLsizei nameLengthActual = 0;
        GLint size;
        GLenum type;
        glGetActiveUniform(id, index, nameLength, &nameLengthActual, &size, &type, nameBufferAscii.data());

        wstring nameUnicode(nameBufferAscii.begin(), nameBufferAscii.end());
        retValue->name = std::move(nameUnicode);
        retValue->size = size;
        retValue->type = type;
    }

    return retValue;
}

WebGLUniformLocation* WebGLProgram::GetUniformLocation(PCSTR name)
{
    const auto location = glGetUniformLocation(id, name);

    if (location == -1) {
        return nullptr;
    } else {
        WebGLUniformLocation* retValue = new WebGLUniformLocation();
        retValue->location = location;
        return retValue;
    }
}

WebGLActiveInfo* WebGLProgram::GetActiveAttrib(GLuint index)
{
    WebGLActiveInfo* retValue = new WebGLActiveInfo();

    GLint nameLength;
    glGetProgramiv(id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &nameLength);

    if (nameLength > 0) {
        vector<GLchar> nameBufferAscii(nameLength);
        GLsizei nameLengthActual = 0;
        GLint size;
        GLenum type;
        glGetActiveAttrib(id, index, nameLength, &nameLengthActual, &size, &type, nameBufferAscii.data());

        wstring nameUnicode(nameBufferAscii.begin(), nameBufferAscii.end());
        retValue->name = std::move(nameUnicode);
        retValue->size = size;
        retValue->type = type;
    }

    return retValue;
}

void WebGLProgram::Validate() { glValidateProgram(id); }

void WebGLProgram::Use() { glUseProgram(id); }

GLint WebGLProgram::GetAttribLocation(PCSTR name) { return glGetAttribLocation(id, name); }

WebGLShader::WebGLShader(GLenum type)
{
    this->type = type;
    id = glCreateShader(type);
}

void WebGLShader::Release()
{
    if (!isReleased) {
        glDeleteShader(id);
        isReleased = true;
    }
}

void WebGLShader::SetSource(PCSTR source)
{
    PCSTR sourceArray[] = {source};
    int lengthArray[] = {static_cast<int>(strlen(source))};
    glShaderSource(id, 1, sourceArray, lengthArray);
}

void WebGLShader::Compile() { glCompileShader(id); }

JsValueRef WebGLShader::GetParameter(GLenum pname)
{
    if (pname == GL_DELETE_STATUS || pname == GL_COMPILE_STATUS) {
        GLint retValue;
        glGetShaderiv(id, pname, &retValue);

        JsValueRef retJsValue;
        RETURN_NULL_IF_JS_ERROR(JsBoolToBoolean(retValue == 0 ? GL_FALSE : GL_TRUE, &retJsValue));
        return retJsValue;
    } else if (pname == GL_SHADER_TYPE) {
        GLint retValue;
        glGetShaderiv(id, pname, &retValue);

        JsValueRef retJsValue;
        RETURN_NULL_IF_JS_ERROR(JsIntToNumber(retValue, &retJsValue));
        return retJsValue;
    } else {
        return nullptr;
    }
}

std::wstring WebGLShader::GetInfoLog()
{
    GLint logLength;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength == 0) {
        return L"";
    } else {
        vector<char> logBuffer(logLength);
        GLint actualLength = 0;
        glGetShaderInfoLog(id, logLength, &actualLength, logBuffer.data());
        wstring logUnicode(logBuffer.begin(), logBuffer.end());
        return logUnicode;
    }
}

void WebGLShader::Delete() { Release(); }

WebGLActiveInfo::WebGLActiveInfo() {}

WebGLUniformLocation::WebGLUniformLocation() {}

ANGLE_instanced_arrays::ANGLE_instanced_arrays() {}

void ANGLE_instanced_arrays::drawArraysInstancedANGLE(GLenum mode, GLint first, GLsizei count, GLsizei primcount)
{
    glDrawArraysInstancedANGLE(mode, first, count, primcount);
}

void ANGLE_instanced_arrays::drawElementsInstancedANGLE(
    GLenum mode, GLsizei count, GLenum type, GLint indices, GLsizei primcount)
{
    glDrawElementsInstancedANGLE(mode, count, type, reinterpret_cast<const void*>(static_cast<size_t>(indices)), primcount);
}

void ANGLE_instanced_arrays::vertexAttribDivisorANGLE(GLuint index, GLuint divisor)
{
    glVertexAttribDivisorANGLE(index, divisor);
}

WebGLRenderbuffer::WebGLRenderbuffer() { glGenRenderbuffers(1, &id); }

void WebGLRenderbuffer::Release() { glDeleteRenderbuffers(1, &id); }

void WebGLRenderbuffer::bindRenderbuffer(GLenum target) { glBindRenderbuffer(target, this->id); }

void WebGLRenderbuffer::framebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget)
{
    glFramebufferRenderbuffer(target, attachment, renderbuffertarget, id);
}

WebGLFramebuffer::WebGLFramebuffer() { glGenFramebuffers(1, &id); }

void WebGLFramebuffer::Release() { glDeleteFramebuffers(1, &id); }

void WebGLFramebuffer::bindFramebuffer(GLenum target) { glBindFramebuffer(target, this->id); }
