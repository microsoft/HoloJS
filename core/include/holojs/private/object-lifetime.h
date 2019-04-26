#pragma once

#include "chakra.h"
#include <string>
#include <vector>

namespace HoloJs {

namespace ResourceManagement {

enum class ObjectType {
    Blob,
    Window,
    ICanvasRenderContext2D,
    IImage,
    WebGLContext,
    WebGLShaderPrecisionFormat,
    WebGLRenderbuffer,
    WebGLFramebuffer,
    WebGLTexture,
    WebGLBuffer,
    WebGLActiveInfo,
    WebGLProgram,
    WebGLShader,
    WebGLUniformLocation,
    IXmlHttpRequest,
    IAudioBuffer,
    IGainNode,
    IAudioContext,
    IAudioDestinationNode,
    IAudioBufferSource,
    IPannerNode,
    IAudioListener,
    IAudioNode,
    IAudioParam,
	IWebSocket,
	ISurfaceMapper,
    ISpeechRecognizer,
	ISpatialAnchor,
    ISpatialAnchorStore,
    None
};

class ResourceManager;

class IRelease {
   public:
    virtual void Release() = 0;

    virtual ~IRelease() {}
};

}  // namespace ResourceManagement

}  // namespace HoloJs