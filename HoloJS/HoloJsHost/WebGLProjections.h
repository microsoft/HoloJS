#pragma once

namespace HologramJS
{
	namespace WebGL
	{
		class WebGLProjections
		{
		public:

			static bool Initialize();

		private:

			static JsValueRef CHAKRA_CALLBACK createContext(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK createContext2D(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			// WebGL context functions
			static JsValueRef CHAKRA_CALLBACK getShaderPrecisionFormat(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK getExtension(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK getParameter(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK createRenderbuffer(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK bindRenderbuffer(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK renderbufferStorage(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK createFramebuffer(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK bindFramebuffer(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK framebufferRenderbuffer(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK framebufferTexture2D(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK createTexture(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK bindTexture(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK texParameteri(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK texImage2D(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK texImage2D1(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK texImage2D2(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK texImage2D3(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK texImage2D4(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK activeTexture(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK generateMipmap(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK pixelStorei(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK clearDepth(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK clearStencil(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK enable(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK disable(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK depthFunc(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK depthMask(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK depthRange(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK frontFace(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK cullFace(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK blendColor(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK blendEquation(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK blendEquationSeparate(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK blendFunc(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK blendFuncSeparate(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK scissor(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK viewport(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK clearColor(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK clear(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK createBuffer(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK deleteBuffer(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK bindBuffer(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK bufferData1(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK bufferData2(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK colorMask(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK drawArrays(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK drawElements(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK enableVertexAttribArray(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK disableVertexAttribArray(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK vertexAttribPointer(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK createProgram(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK useProgram(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK validateProgram(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK linkProgram(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK getProgramParameter(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK getProgramInfoLog(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK deleteProgram(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK bindAttribLocation(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK getActiveUniform(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK getActiveAttrib(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK getAttribLocation(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK createShader(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK shaderSource(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK compileShader(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK getShaderParameter(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK getShaderInfoLog(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK attachShader(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK deleteShader(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK getUniformLocation(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK uniform1f(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK uniform1fv(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK uniform1i(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK uniform1iv(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK uniform2f(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK uniform2fv(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK uniform2i(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK uniform2iv(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK uniform3f(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK uniform3fv(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK uniform3i(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK uniform3iv(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK uniform4f(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK uniform4fv(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK uniform4i(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK uniform4iv(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK uniformMatrix2fv(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK uniformMatrix3fv(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK uniformMatrix4fv(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			// Canvas 3D context functions
			static JsValueRef CHAKRA_CALLBACK drawImage1(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK drawImage2(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK drawImage3(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK getImageData(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK stencilFunc(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK stencilMask(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);

			static JsValueRef CHAKRA_CALLBACK stencilOp(
				JsValueRef callee,
				bool isConstructCall,
				JsValueRef* arguments,
				unsigned short argumentCount,
				PVOID callbackData
			);
		};
	}
}

