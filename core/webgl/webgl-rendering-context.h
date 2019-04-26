#pragma once

#include "../resource-management/external-object.h"
#include "../include/holojs/private/chakra.h"
#include "../include/holojs/private/image-interface.h"
#include "webgl-objects.h"

namespace HoloJs {
namespace WebGL {
class WebGLRenderingContext : public HoloJs::ResourceManagement::IRelease {
   public:
    WebGLRenderingContext() {}
	virtual ~WebGLRenderingContext() { Release(); }

    void Release() {}

    std::unique_ptr<WebGLFramebuffer> m_frameBuffer;
    std::unique_ptr<WebGLTexture> m_renderTexture;
    std::unique_ptr<WebGLRenderbuffer> m_renderBuffer;

    WebGLShaderPrecisionFormat* getShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype);

    // Object^ getExtension(Platform::String^ name);
    JsValueRef getParameter(GLenum pname);

#pragma region Frame and render buffers
    WebGLRenderbuffer* createRenderbuffer();
    void bindRenderbuffer(GLenum target, WebGLRenderbuffer* renderbuffer);
    void renderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);

    WebGLFramebuffer* createFramebuffer();
    void bindFramebuffer(GLenum target, WebGLFramebuffer* framebuffer);

    void framebufferRenderbuffer(GLenum target,
                                 GLenum attachment,
                                 GLenum renderbuffertarget,
                                 WebGLRenderbuffer* renderbuffer);

    void framebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, WebGLTexture* texture, GLint level);
#pragma endregion

#pragma region Textures
    WebGLTexture* createTexture();
    void deleteTexture(WebGLTexture* texture);

    void bindTexture(GLenum target, WebGLTexture* texture);

    void texParameteri(GLenum target, GLenum pname, GLint param);

    void texImage2D(GLenum target,
                    GLint level,
                    GLenum internalformat,
                    GLsizei width,
                    GLsizei height,
                    GLint border,
                    GLenum format,
                    GLenum type,
                    HoloJs::IIMage* image);

    void texImage2D(GLenum target,
                    GLint level,
                    GLenum internalformat,
                    GLsizei width,
                    GLsizei height,
                    GLint border,
                    GLenum format,
                    GLenum type,
                    void* pixels,
                    GLsizei stride = 0);

    void texImage2DNoFlip(GLenum target,
                          GLint level,
                          GLenum internalformat,
                          GLsizei width,
                          GLsizei height,
                          GLint border,
                          GLenum format,
                          GLenum type,
                          void* pixels);

    // void texImage2D(GLenum target, GLint level, GLenum internalformat, GLenum format, GLenum type,
    // WebGLRenderer::RenderingContext2D* context2D);

    void activeTexture(GLenum texture);
    void generateMipmap(GLenum target);
    void pixelStorei(GLenum pname, GLint param);
    void pixelStorei(GLenum pname, GLboolean param);
#pragma endregion

    void clearDepth(GLclampf depth);

    void clearStencil(GLint s);

    void enable(GLenum cap);
    void disable(GLenum cap);

    void depthFunc(GLenum func);

    void depthMask(GLboolean flag);
    void depthRange(GLclampf zNear, GLclampf zFar);

    void frontFace(GLenum mode);

    void cullFace(GLenum mode);

    void blendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);

    void blendEquation(GLenum mode);

    void blendEquationSeparate(GLenum modeRGB, GLenum modeAlpha);

    void blendFunc(GLenum sfactor, GLenum dfactor);

    void blendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);

    void scissor(GLint x, GLint y, GLsizei width, GLsizei height);

    void viewport(GLint x, GLint y, GLsizei width, GLsizei height);

    void clearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);

    void clear(GLbitfield mask);

    WebGLBuffer* createBuffer();
    void deleteBuffer(WebGLBuffer* buffer);

    void bindBuffer(GLenum target, WebGLBuffer* buffer);

    void bufferData(GLenum target, GLsizeiptr size, void* data, GLenum usage);

    void bufferSubData(GLenum target, GLsizeiptr offset, void* data, GLsizeiptr dataSize);

    void colorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);

    void drawArrays(GLenum mode, GLint first, GLsizei count);
    void drawElements(GLenum mode, GLsizei count, GLenum type, GLint offset);

    void enableVertexAttribArray(GLuint index);

    void disableVertexAttribArray(GLuint index);

    void vertexAttribPointer(GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLint offset);

    void stencilFunc(GLenum func, GLint ref, GLuint mask);
    void stencilMask(GLuint mask);
    void stencilOp(GLenum fail, GLenum zfail, GLenum zpass);

#pragma region Program
    WebGLProgram* createProgram();
    void useProgram(WebGLProgram* program);
    void validateProgram(WebGLProgram* program);
    void linkProgram(WebGLProgram* program);
    JsValueRef getProgramParameter(WebGLProgram* program, GLenum pname);
    std::wstring getProgramInfoLog(WebGLProgram* program);
    void deleteProgram(WebGLProgram* program);
    void bindAttribLocation(WebGLProgram* program, GLuint index, std::wstring& name);
    WebGLActiveInfo* getActiveUniform(WebGLProgram* program, GLuint index);
    WebGLActiveInfo* getActiveAttrib(WebGLProgram* program, GLuint index);
    GLint getAttribLocation(WebGLProgram* program, std::wstring& name);
#pragma endregion

#pragma region Shaders
    WebGLShader* createShader(GLenum type);
    void shaderSource(WebGLShader* shader, std::wstring& source);
    void compileShader(WebGLShader* shader);
    JsValueRef getShaderParameter(WebGLShader* shader, GLenum pname);
    std::wstring getShaderInfoLog(WebGLShader* shader);
    void attachShader(WebGLProgram* program, WebGLShader* shader);
    void deleteShader(WebGLShader* shader);
#pragma endregion

#pragma region Uniforms

    WebGLUniformLocation* getUniformLocation(WebGLProgram* program, std::wstring& name);

    void uniform1f(WebGLUniformLocation* location, GLfloat x);

    void uniform1fv(WebGLUniformLocation* location, GLsizei count, const GLfloat* v);

    void uniform1i(WebGLUniformLocation* location, GLint x);

    void uniform1iv(WebGLUniformLocation* location, GLsizei count, const GLint* v);

    void uniform2f(WebGLUniformLocation* location, GLfloat x, GLfloat y);

    void uniform2fv(WebGLUniformLocation* location, GLsizei count, const GLfloat* v);

    void uniform2i(WebGLUniformLocation* location, GLint x, GLint y);

    void uniform2iv(WebGLUniformLocation* location, GLsizei count, const GLint* v);

    void uniform3f(WebGLUniformLocation* location, GLfloat x, GLfloat y, GLfloat z);

    void uniform3fv(WebGLUniformLocation* location, GLsizei count, const GLfloat* v);

    void uniform3i(WebGLUniformLocation* location, GLint x, GLint y, GLint z);

    void uniform3iv(WebGLUniformLocation* location, GLsizei count, const GLint* v);

    void uniform4f(WebGLUniformLocation* location, GLfloat x, GLfloat y, GLfloat z, GLfloat w);

    void uniform4fv(WebGLUniformLocation* location, GLsizei count, const GLfloat* v);

    void uniform4i(WebGLUniformLocation* location, GLint x, GLint y, GLint z, GLint w);

    void uniform4iv(WebGLUniformLocation* location, GLsizei count, const GLint* v);

    void uniformMatrix2fv(WebGLUniformLocation* location, GLboolean transpose, GLsizei count, const GLfloat* value);

    void uniformMatrix3fv(WebGLUniformLocation* location, GLboolean transpose, GLsizei count, const GLfloat* value);

    void uniformMatrix4fv(WebGLUniformLocation* location, GLboolean transpose, GLsizei count, const GLfloat* value);
#pragma endregion

#pragma region Lines
    void lineWidth(GLfloat width);
#pragma endregion

    void polygonOffset(GLfloat factor, GLfloat units);

    bool isFrontFaceSwitched = false;

   private:
    const GLenum UNSIGNED_BYTE = 0x1401;
    const GLenum UNPACK_FLIP_Y_WEBGL = 0x9240;
    const GLenum UNPACK_PREMULTIPLY_ALPHA_WEBGL = 0x9241;

    bool FlipYEnabled = false;
};
}  // namespace WebGL
}  // namespace HologramJS
