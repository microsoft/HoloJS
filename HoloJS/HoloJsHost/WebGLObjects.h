#pragma once

namespace HologramJS
{
	namespace WebGL
	{
		class WebGLShaderPrecisionFormat : public HologramJS::Utilities::IRelease
		{
		public:
			GLint rangeMin;
			GLint rangeMax;
			GLint precision;
			WebGLShaderPrecisionFormat(GLint min, GLint max, GLint precision);

			virtual void Release();

			virtual ~WebGLShaderPrecisionFormat() {}
		};

		class WebGLRenderbuffer : public HologramJS::Utilities::IRelease
		{
		public:
			WebGLRenderbuffer();
			virtual void Release();
			virtual ~WebGLRenderbuffer() {}
			void bindRenderbuffer(GLenum target);
			void framebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget);
			GLuint id;
		};

		class WebGLFramebuffer : public HologramJS::Utilities::IRelease
		{
		public:
			WebGLFramebuffer();
			virtual void Release();
			virtual ~WebGLFramebuffer() {}
			void bindFramebuffer(GLenum target);
			GLuint id;
		};

		class WebGLTexture : public HologramJS::Utilities::IRelease
		{
		public:
			WebGLTexture();
			virtual void Release();
			virtual ~WebGLTexture() {}
			void framebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLint level);

			void Bind(GLenum target);
			GLuint id;
		};

		class WebGLBuffer : public HologramJS::Utilities::IRelease
		{
		public:
			WebGLBuffer();
			virtual void Release();
			virtual ~WebGLBuffer() {}

			void Bind(GLenum target);
			GLuint id;
		};

		class WebGLShader : public HologramJS::Utilities::IRelease
		{
		public:
			WebGLShader(GLenum type);
			virtual void Release();
			virtual ~WebGLShader() {}

			void SetSource(PCSTR source);
			void Compile();
			JsValueRef GetParameter(GLenum pname);
			std::wstring GetInfoLog();
			void Delete();

			GLuint id;
			GLenum type;
		};

		class WebGLActiveInfo : public HologramJS::Utilities::IRelease
		{
		public:
			WebGLActiveInfo();
			void Release() {}
			virtual ~WebGLActiveInfo() {}
			std::wstring name;
			int size;
			int type;
		};

		class WebGLUniformLocation : public HologramJS::Utilities::IRelease
		{
		public:
			WebGLUniformLocation();
			void Release() {}
			virtual ~WebGLUniformLocation() {}
			int location;
		};

		class WebGLProgram : public HologramJS::Utilities::IRelease
		{
		public:
			WebGLProgram();
			virtual void Release();
			virtual ~WebGLProgram() {}

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
		};

		class ANGLE_instanced_arrays
		{
		public:
			ANGLE_instanced_arrays();
			void drawArraysInstancedANGLE(GLenum mode, GLint first, GLsizei count, GLsizei primcount);
			void drawElementsInstancedANGLE(GLenum mode, GLsizei count, GLenum type, GLint indices, GLsizei primcount);
			void vertexAttribDivisorANGLE(GLuint index, GLuint divisor);
		};
	}
}
