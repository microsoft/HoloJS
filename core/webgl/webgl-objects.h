#pragma once

#include "../resource-management/external-object.h"
#include "../include/holojs/private/chakra.h"
#include <string>
#include <GLES2/gl2.h>

namespace HoloJs {
namespace WebGL {
class WebGLShaderPrecisionFormat : public HoloJs::ResourceManagement::IRelease {
   public:
    GLint rangeMin;
    GLint rangeMax;
    GLint precision;
    WebGLShaderPrecisionFormat(GLint min, GLint max, GLint precision);

    virtual void Release();

    virtual ~WebGLShaderPrecisionFormat() {}
};

class WebGLRenderbuffer : public HoloJs::ResourceManagement::IRelease {
   public:
    WebGLRenderbuffer();
    virtual void Release();
	virtual ~WebGLRenderbuffer() { Release();  }
    void bindRenderbuffer(GLenum target);
    void framebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget);
    GLuint id;
};

class WebGLFramebuffer : public HoloJs::ResourceManagement::IRelease {
   public:
    WebGLFramebuffer();
    virtual void Release();
	virtual ~WebGLFramebuffer() { Release();  }
    void bindFramebuffer(GLenum target);
    GLuint id;
};

class WebGLTexture : public HoloJs::ResourceManagement::IRelease {
   public:
    WebGLTexture();
    virtual void Release();
	virtual ~WebGLTexture() { Release(); }
    void Delete() { Release(); }
    void framebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLint level);

    void Bind(GLenum target);
    GLuint id;

private:
    bool isReleased = false;
};

class WebGLBuffer : public HoloJs::ResourceManagement::IRelease {
   public:
    WebGLBuffer();
    virtual void Release();
	virtual ~WebGLBuffer() { Release(); }

    void Bind(GLenum target);
    GLuint id;
};

class WebGLShader : public HoloJs::ResourceManagement::IRelease {
   public:
    WebGLShader(GLenum type);
    virtual void Release();
	virtual ~WebGLShader() { Release(); }

    void SetSource(PCSTR source);
    void Compile();
    JsValueRef GetParameter(GLenum pname);
    std::wstring GetInfoLog();
    void Delete();

    GLuint id;
    GLenum type;

private:
    bool isReleased = false;
};

class WebGLActiveInfo : public HoloJs::ResourceManagement::IRelease {
   public:
    WebGLActiveInfo();
    void Release() {}
	virtual ~WebGLActiveInfo() { Release(); }
    std::wstring name;
    int size;
    int type;
};

class WebGLUniformLocation : public HoloJs::ResourceManagement::IRelease {
   public:
    WebGLUniformLocation();
    void Release() {}
	virtual ~WebGLUniformLocation() { Release(); }
    int location;
};

class WebGLProgram : public HoloJs::ResourceManagement::IRelease {
   public:
    WebGLProgram();
    virtual void Release();
	virtual ~WebGLProgram() { Release(); }

    void AttachShader(WebGLShader* shader);
    void BindAttribLocation(GLuint index, PCSTR name);
    void Link();
    JsValueRef GetParameter(GLenum pname);
    std::wstring GetInfoLog();
    void Delete();

    WebGLActiveInfo* GetActiveUniform(GLuint index);
    WebGLUniformLocation* GetUniformLocation(PCSTR name);

    void Validate();
    void Use();

    WebGLActiveInfo* GetActiveAttrib(GLuint index);
    GLint GetAttribLocation(PCSTR name);

    GLuint id;

private:
    bool isReleased = false;
};

class ANGLE_instanced_arrays {
   public:
    ANGLE_instanced_arrays();
    void drawArraysInstancedANGLE(GLenum mode, GLint first, GLsizei count, GLsizei primcount);
    void drawElementsInstancedANGLE(GLenum mode, GLsizei count, GLenum type, GLint indices, GLsizei primcount);
    void vertexAttribDivisorANGLE(GLuint index, GLuint divisor);
};
}  // namespace WebGL
}  // namespace HologramJS
