#pragma once

#include "../include/holojs/private/chakra.h"
#include <memory>
#include <GLES2/gl2.h>

namespace HoloJs {

namespace ResourceManagement {
class ResourceManager;
}

namespace WebGL {

class WebGLProjections {
   public:
    WebGLProjections(std::shared_ptr<HoloJs::ResourceManagement::ResourceManager> resourceManager)
        : m_resourceManager(resourceManager){};
    HRESULT initialize();

    bool isContextFrontFaceSwitched = false;

   private:
    std::shared_ptr<HoloJs::ResourceManagement::ResourceManager> m_resourceManager;

    JsValueRef m_createContext = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticCreateContext(JsValueRef callee,
                                                          bool isConstructCall,
                                                          JsValueRef* arguments,
                                                          unsigned short argumentCount,
                                                          PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->createContext(arguments, argumentCount);
    }
    JsValueRef createContext(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_getShaderPrecisionFormat = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticGetShaderPrecisionFormat(JsValueRef callee,
                                                                     bool isConstructCall,
                                                                     JsValueRef* arguments,
                                                                     unsigned short argumentCount,
                                                                     PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->getShaderPrecisionFormat(arguments, argumentCount);
    }
    JsValueRef getShaderPrecisionFormat(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_getExtension = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticGetExtension(JsValueRef callee,
                                                         bool isConstructCall,
                                                         JsValueRef* arguments,
                                                         unsigned short argumentCount,
                                                         PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->getExtension(arguments, argumentCount);
    }
    JsValueRef getExtension(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_getParameter = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticGetParameter(JsValueRef callee,
                                                         bool isConstructCall,
                                                         JsValueRef* arguments,
                                                         unsigned short argumentCount,
                                                         PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->getParameter(arguments, argumentCount);
    }
    JsValueRef getParameter(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_createRenderbuffer = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticCreateRenderbuffer(JsValueRef callee,
                                                               bool isConstructCall,
                                                               JsValueRef* arguments,
                                                               unsigned short argumentCount,
                                                               PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->createRenderbuffer(arguments, argumentCount);
    }
    JsValueRef createRenderbuffer(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_bindRenderbuffer = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticBindRenderbuffer(JsValueRef callee,
                                                             bool isConstructCall,
                                                             JsValueRef* arguments,
                                                             unsigned short argumentCount,
                                                             PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->bindRenderbuffer(arguments, argumentCount);
    }
    JsValueRef bindRenderbuffer(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_renderbufferStorage = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticRenderbufferStorage(JsValueRef callee,
                                                                bool isConstructCall,
                                                                JsValueRef* arguments,
                                                                unsigned short argumentCount,
                                                                PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->renderbufferStorage(arguments, argumentCount);
    }
    JsValueRef renderbufferStorage(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_createFramebuffer = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticCreateFramebuffer(JsValueRef callee,
                                                              bool isConstructCall,
                                                              JsValueRef* arguments,
                                                              unsigned short argumentCount,
                                                              PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->createFramebuffer(arguments, argumentCount);
    }
    JsValueRef createFramebuffer(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_bindFramebuffer = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticBindFramebuffer(JsValueRef callee,
                                                            bool isConstructCall,
                                                            JsValueRef* arguments,
                                                            unsigned short argumentCount,
                                                            PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->bindFramebuffer(arguments, argumentCount);
    }
    JsValueRef bindFramebuffer(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_framebufferRenderbuffer = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticFramebufferRenderbuffer(JsValueRef callee,
                                                                    bool isConstructCall,
                                                                    JsValueRef* arguments,
                                                                    unsigned short argumentCount,
                                                                    PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->framebufferRenderbuffer(arguments, argumentCount);
    }
    JsValueRef framebufferRenderbuffer(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_framebufferTexture2D = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticFramebufferTexture2D(JsValueRef callee,
                                                                 bool isConstructCall,
                                                                 JsValueRef* arguments,
                                                                 unsigned short argumentCount,
                                                                 PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->framebufferTexture2D(arguments, argumentCount);
    }
    JsValueRef framebufferTexture2D(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_createTexture = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticCreateTexture(JsValueRef callee,
                                                          bool isConstructCall,
                                                          JsValueRef* arguments,
                                                          unsigned short argumentCount,
                                                          PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->createTexture(arguments, argumentCount);
    }
    JsValueRef createTexture(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_deleteTexture = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticDeleteTexture(JsValueRef callee,
                                                          bool isConstructCall,
                                                          JsValueRef* arguments,
                                                          unsigned short argumentCount,
                                                          PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->deleteTexture(arguments, argumentCount);
    }
    JsValueRef deleteTexture(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_bindTexture = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticBindTexture(JsValueRef callee,
                                                        bool isConstructCall,
                                                        JsValueRef* arguments,
                                                        unsigned short argumentCount,
                                                        PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->bindTexture(arguments, argumentCount);
    }
    JsValueRef bindTexture(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_texParameteri = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticTexParameteri(JsValueRef callee,
                                                          bool isConstructCall,
                                                          JsValueRef* arguments,
                                                          unsigned short argumentCount,
                                                          PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->texParameteri(arguments, argumentCount);
    }
    JsValueRef texParameteri(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_texImage2D1 = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticTexImage2D1(JsValueRef callee,
                                                        bool isConstructCall,
                                                        JsValueRef* arguments,
                                                        unsigned short argumentCount,
                                                        PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->texImage2D1(arguments, argumentCount);
    }
    JsValueRef texImage2D1(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_texImage2D2 = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticTexImage2D2(JsValueRef callee,
                                                        bool isConstructCall,
                                                        JsValueRef* arguments,
                                                        unsigned short argumentCount,
                                                        PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->texImage2D2(arguments, argumentCount);
    }
    JsValueRef texImage2D2(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_texImage2D3 = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticTexImage2D3(JsValueRef callee,
                                                        bool isConstructCall,
                                                        JsValueRef* arguments,
                                                        unsigned short argumentCount,
                                                        PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->texImage2D3(arguments, argumentCount);
    }
    JsValueRef texImage2D3(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_texImage2D4 = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticTexImage2D4(JsValueRef callee,
                                                        bool isConstructCall,
                                                        JsValueRef* arguments,
                                                        unsigned short argumentCount,
                                                        PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->texImage2D4(arguments, argumentCount);
    }
    JsValueRef texImage2D4(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_activeTexture = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticActiveTexture(JsValueRef callee,
                                                          bool isConstructCall,
                                                          JsValueRef* arguments,
                                                          unsigned short argumentCount,
                                                          PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->activeTexture(arguments, argumentCount);
    }
    JsValueRef activeTexture(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_generateMipmap = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticGenerateMipmap(JsValueRef callee,
                                                           bool isConstructCall,
                                                           JsValueRef* arguments,
                                                           unsigned short argumentCount,
                                                           PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->generateMipmap(arguments, argumentCount);
    }
    JsValueRef generateMipmap(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_pixelStorei = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticPixelStorei(JsValueRef callee,
                                                        bool isConstructCall,
                                                        JsValueRef* arguments,
                                                        unsigned short argumentCount,
                                                        PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->pixelStorei(arguments, argumentCount);
    }
    JsValueRef pixelStorei(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_clearDepth = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticClearDepth(JsValueRef callee,
                                                       bool isConstructCall,
                                                       JsValueRef* arguments,
                                                       unsigned short argumentCount,
                                                       PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->clearDepth(arguments, argumentCount);
    }
    JsValueRef clearDepth(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_clearStencil = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticClearStencil(JsValueRef callee,
                                                         bool isConstructCall,
                                                         JsValueRef* arguments,
                                                         unsigned short argumentCount,
                                                         PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->clearStencil(arguments, argumentCount);
    }
    JsValueRef clearStencil(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_enable = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticEnable(JsValueRef callee,
                                                   bool isConstructCall,
                                                   JsValueRef* arguments,
                                                   unsigned short argumentCount,
                                                   PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->enable(arguments, argumentCount);
    }
    JsValueRef enable(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_disable = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticDisable(JsValueRef callee,
                                                    bool isConstructCall,
                                                    JsValueRef* arguments,
                                                    unsigned short argumentCount,
                                                    PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->disable(arguments, argumentCount);
    }
    JsValueRef disable(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_depthFunc = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticDepthFunc(JsValueRef callee,
                                                      bool isConstructCall,
                                                      JsValueRef* arguments,
                                                      unsigned short argumentCount,
                                                      PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->depthFunc(arguments, argumentCount);
    }
    JsValueRef depthFunc(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_depthMask = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticDepthMask(JsValueRef callee,
                                                      bool isConstructCall,
                                                      JsValueRef* arguments,
                                                      unsigned short argumentCount,
                                                      PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->depthMask(arguments, argumentCount);
    }
    JsValueRef depthMask(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_depthRange = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticDepthRange(JsValueRef callee,
                                                       bool isConstructCall,
                                                       JsValueRef* arguments,
                                                       unsigned short argumentCount,
                                                       PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->depthRange(arguments, argumentCount);
    }
    JsValueRef depthRange(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_frontFace = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticFrontFace(JsValueRef callee,
                                                      bool isConstructCall,
                                                      JsValueRef* arguments,
                                                      unsigned short argumentCount,
                                                      PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->frontFace(arguments, argumentCount);
    }
    JsValueRef frontFace(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_cullFace = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticCullFace(JsValueRef callee,
                                                     bool isConstructCall,
                                                     JsValueRef* arguments,
                                                     unsigned short argumentCount,
                                                     PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->cullFace(arguments, argumentCount);
    }
    JsValueRef cullFace(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_blendColor = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticBlendColor(JsValueRef callee,
                                                       bool isConstructCall,
                                                       JsValueRef* arguments,
                                                       unsigned short argumentCount,
                                                       PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->blendColor(arguments, argumentCount);
    }
    JsValueRef blendColor(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_blendEquation = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticBlendEquation(JsValueRef callee,
                                                          bool isConstructCall,
                                                          JsValueRef* arguments,
                                                          unsigned short argumentCount,
                                                          PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->blendEquation(arguments, argumentCount);
    }
    JsValueRef blendEquation(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_blendEquationSeparate = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticBlendEquationSeparate(JsValueRef callee,
                                                                  bool isConstructCall,
                                                                  JsValueRef* arguments,
                                                                  unsigned short argumentCount,
                                                                  PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->blendEquationSeparate(arguments, argumentCount);
    }
    JsValueRef blendEquationSeparate(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_blendFunc = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticBlendFunc(JsValueRef callee,
                                                      bool isConstructCall,
                                                      JsValueRef* arguments,
                                                      unsigned short argumentCount,
                                                      PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->blendFunc(arguments, argumentCount);
    }
    JsValueRef blendFunc(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_blendFuncSeparate = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticBlendFuncSeparate(JsValueRef callee,
                                                              bool isConstructCall,
                                                              JsValueRef* arguments,
                                                              unsigned short argumentCount,
                                                              PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->blendFuncSeparate(arguments, argumentCount);
    }
    JsValueRef blendFuncSeparate(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_scissor = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticScissor(JsValueRef callee,
                                                    bool isConstructCall,
                                                    JsValueRef* arguments,
                                                    unsigned short argumentCount,
                                                    PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->scissor(arguments, argumentCount);
    }
    JsValueRef scissor(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_viewport = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticViewport(JsValueRef callee,
                                                     bool isConstructCall,
                                                     JsValueRef* arguments,
                                                     unsigned short argumentCount,
                                                     PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->viewport(arguments, argumentCount);
    }
    JsValueRef viewport(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_clearColor = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticClearColor(JsValueRef callee,
                                                       bool isConstructCall,
                                                       JsValueRef* arguments,
                                                       unsigned short argumentCount,
                                                       PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->clearColor(arguments, argumentCount);
    }
    JsValueRef clearColor(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_clear = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticClear(JsValueRef callee,
                                                  bool isConstructCall,
                                                  JsValueRef* arguments,
                                                  unsigned short argumentCount,
                                                  PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->clear(arguments, argumentCount);
    }
    JsValueRef clear(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_createBuffer = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticCreateBuffer(JsValueRef callee,
                                                         bool isConstructCall,
                                                         JsValueRef* arguments,
                                                         unsigned short argumentCount,
                                                         PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->createBuffer(arguments, argumentCount);
    }
    JsValueRef createBuffer(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_deleteBuffer = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticDeleteBuffer(JsValueRef callee,
                                                         bool isConstructCall,
                                                         JsValueRef* arguments,
                                                         unsigned short argumentCount,
                                                         PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->deleteBuffer(arguments, argumentCount);
    }
    JsValueRef deleteBuffer(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_bindBuffer = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticBindBuffer(JsValueRef callee,
                                                       bool isConstructCall,
                                                       JsValueRef* arguments,
                                                       unsigned short argumentCount,
                                                       PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->bindBuffer(arguments, argumentCount);
    }
    JsValueRef bindBuffer(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_bufferData1 = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticBufferData1(JsValueRef callee,
                                                        bool isConstructCall,
                                                        JsValueRef* arguments,
                                                        unsigned short argumentCount,
                                                        PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->bufferData1(arguments, argumentCount);
    }
    JsValueRef bufferData1(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_bufferData2 = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticBufferData2(JsValueRef callee,
                                                        bool isConstructCall,
                                                        JsValueRef* arguments,
                                                        unsigned short argumentCount,
                                                        PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->bufferData2(arguments, argumentCount);
    }
    JsValueRef bufferData2(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_bufferSubData = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticBufferSubData(JsValueRef callee,
                                                          bool isConstructCall,
                                                          JsValueRef* arguments,
                                                          unsigned short argumentCount,
                                                          PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->bufferSubData(arguments, argumentCount);
    }
    JsValueRef bufferSubData(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_colorMask = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticColorMask(JsValueRef callee,
                                                      bool isConstructCall,
                                                      JsValueRef* arguments,
                                                      unsigned short argumentCount,
                                                      PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->colorMask(arguments, argumentCount);
    }
    JsValueRef colorMask(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_drawArrays = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticDrawArrays(JsValueRef callee,
                                                       bool isConstructCall,
                                                       JsValueRef* arguments,
                                                       unsigned short argumentCount,
                                                       PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->drawArrays(arguments, argumentCount);
    }
    JsValueRef drawArrays(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_drawElements = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticDrawElements(JsValueRef callee,
                                                         bool isConstructCall,
                                                         JsValueRef* arguments,
                                                         unsigned short argumentCount,
                                                         PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->drawElements(arguments, argumentCount);
    }
    JsValueRef drawElements(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_enableVertexAttribArray = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticEnableVertexAttribArray(JsValueRef callee,
                                                                    bool isConstructCall,
                                                                    JsValueRef* arguments,
                                                                    unsigned short argumentCount,
                                                                    PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->enableVertexAttribArray(arguments, argumentCount);
    }
    JsValueRef enableVertexAttribArray(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_disableVertexAttribArray = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticDisableVertexAttribArray(JsValueRef callee,
                                                                     bool isConstructCall,
                                                                     JsValueRef* arguments,
                                                                     unsigned short argumentCount,
                                                                     PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->disableVertexAttribArray(arguments, argumentCount);
    }
    JsValueRef disableVertexAttribArray(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_vertexAttribPointer = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticVertexAttribPointer(JsValueRef callee,
                                                                bool isConstructCall,
                                                                JsValueRef* arguments,
                                                                unsigned short argumentCount,
                                                                PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->vertexAttribPointer(arguments, argumentCount);
    }
    JsValueRef vertexAttribPointer(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_createProgram = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticCreateProgram(JsValueRef callee,
                                                          bool isConstructCall,
                                                          JsValueRef* arguments,
                                                          unsigned short argumentCount,
                                                          PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->createProgram(arguments, argumentCount);
    }
    JsValueRef createProgram(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_useProgram = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticUseProgram(JsValueRef callee,
                                                       bool isConstructCall,
                                                       JsValueRef* arguments,
                                                       unsigned short argumentCount,
                                                       PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->useProgram(arguments, argumentCount);
    }
    JsValueRef useProgram(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_validateProgram = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticValidateProgram(JsValueRef callee,
                                                            bool isConstructCall,
                                                            JsValueRef* arguments,
                                                            unsigned short argumentCount,
                                                            PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->validateProgram(arguments, argumentCount);
    }
    JsValueRef validateProgram(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_linkProgram = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticLinkProgram(JsValueRef callee,
                                                        bool isConstructCall,
                                                        JsValueRef* arguments,
                                                        unsigned short argumentCount,
                                                        PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->linkProgram(arguments, argumentCount);
    }
    JsValueRef linkProgram(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_getProgramParameter = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticGetProgramParameter(JsValueRef callee,
                                                                bool isConstructCall,
                                                                JsValueRef* arguments,
                                                                unsigned short argumentCount,
                                                                PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->getProgramParameter(arguments, argumentCount);
    }
    JsValueRef getProgramParameter(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_getProgramInfoLog = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticGetProgramInfoLog(JsValueRef callee,
                                                              bool isConstructCall,
                                                              JsValueRef* arguments,
                                                              unsigned short argumentCount,
                                                              PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->getProgramInfoLog(arguments, argumentCount);
    }
    JsValueRef getProgramInfoLog(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_deleteProgram = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticDeleteProgram(JsValueRef callee,
                                                          bool isConstructCall,
                                                          JsValueRef* arguments,
                                                          unsigned short argumentCount,
                                                          PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->deleteProgram(arguments, argumentCount);
    }
    JsValueRef deleteProgram(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_bindAttribLocation = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticBindAttribLocation(JsValueRef callee,
                                                               bool isConstructCall,
                                                               JsValueRef* arguments,
                                                               unsigned short argumentCount,
                                                               PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->bindAttribLocation(arguments, argumentCount);
    }
    JsValueRef bindAttribLocation(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_getActiveUniform = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticGetActiveUniform(JsValueRef callee,
                                                             bool isConstructCall,
                                                             JsValueRef* arguments,
                                                             unsigned short argumentCount,
                                                             PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->getActiveUniform(arguments, argumentCount);
    }
    JsValueRef getActiveUniform(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_getActiveAttrib = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticGetActiveAttrib(JsValueRef callee,
                                                            bool isConstructCall,
                                                            JsValueRef* arguments,
                                                            unsigned short argumentCount,
                                                            PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->getActiveAttrib(arguments, argumentCount);
    }
    JsValueRef getActiveAttrib(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_getAttribLocation = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticGetAttribLocation(JsValueRef callee,
                                                              bool isConstructCall,
                                                              JsValueRef* arguments,
                                                              unsigned short argumentCount,
                                                              PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->getAttribLocation(arguments, argumentCount);
    }
    JsValueRef getAttribLocation(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_createShader = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticCreateShader(JsValueRef callee,
                                                         bool isConstructCall,
                                                         JsValueRef* arguments,
                                                         unsigned short argumentCount,
                                                         PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->createShader(arguments, argumentCount);
    }
    JsValueRef createShader(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_shaderSource = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticShaderSource(JsValueRef callee,
                                                         bool isConstructCall,
                                                         JsValueRef* arguments,
                                                         unsigned short argumentCount,
                                                         PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->shaderSource(arguments, argumentCount);
    }
    JsValueRef shaderSource(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_compileShader = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticCompileShader(JsValueRef callee,
                                                          bool isConstructCall,
                                                          JsValueRef* arguments,
                                                          unsigned short argumentCount,
                                                          PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->compileShader(arguments, argumentCount);
    }
    JsValueRef compileShader(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_getShaderParameter = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticGetShaderParameter(JsValueRef callee,
                                                               bool isConstructCall,
                                                               JsValueRef* arguments,
                                                               unsigned short argumentCount,
                                                               PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->getShaderParameter(arguments, argumentCount);
    }
    JsValueRef getShaderParameter(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_getShaderInfoLog = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticGetShaderInfoLog(JsValueRef callee,
                                                             bool isConstructCall,
                                                             JsValueRef* arguments,
                                                             unsigned short argumentCount,
                                                             PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->getShaderInfoLog(arguments, argumentCount);
    }
    JsValueRef getShaderInfoLog(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_attachShader = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticAttachShader(JsValueRef callee,
                                                         bool isConstructCall,
                                                         JsValueRef* arguments,
                                                         unsigned short argumentCount,
                                                         PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->attachShader(arguments, argumentCount);
    }
    JsValueRef attachShader(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_deleteShader = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticDeleteShader(JsValueRef callee,
                                                         bool isConstructCall,
                                                         JsValueRef* arguments,
                                                         unsigned short argumentCount,
                                                         PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->deleteShader(arguments, argumentCount);
    }
    JsValueRef deleteShader(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_getUniformLocation = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticGetUniformLocation(JsValueRef callee,
                                                               bool isConstructCall,
                                                               JsValueRef* arguments,
                                                               unsigned short argumentCount,
                                                               PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->getUniformLocation(arguments, argumentCount);
    }
    JsValueRef getUniformLocation(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_uniform1f = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticUniform1f(JsValueRef callee,
                                                      bool isConstructCall,
                                                      JsValueRef* arguments,
                                                      unsigned short argumentCount,
                                                      PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->uniform1f(arguments, argumentCount);
    }
    JsValueRef uniform1f(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_uniform1fv = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticUniform1fv(JsValueRef callee,
                                                       bool isConstructCall,
                                                       JsValueRef* arguments,
                                                       unsigned short argumentCount,
                                                       PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->uniform1fv(arguments, argumentCount);
    }
    JsValueRef uniform1fv(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_uniform1i = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticUniform1i(JsValueRef callee,
                                                      bool isConstructCall,
                                                      JsValueRef* arguments,
                                                      unsigned short argumentCount,
                                                      PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->uniform1i(arguments, argumentCount);
    }
    JsValueRef uniform1i(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_uniform1iv = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticUniform1iv(JsValueRef callee,
                                                       bool isConstructCall,
                                                       JsValueRef* arguments,
                                                       unsigned short argumentCount,
                                                       PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->uniform1iv(arguments, argumentCount);
    }
    JsValueRef uniform1iv(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_uniform2f = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticUniform2f(JsValueRef callee,
                                                      bool isConstructCall,
                                                      JsValueRef* arguments,
                                                      unsigned short argumentCount,
                                                      PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->uniform2f(arguments, argumentCount);
    }
    JsValueRef uniform2f(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_uniform2fv = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticUniform2fv(JsValueRef callee,
                                                       bool isConstructCall,
                                                       JsValueRef* arguments,
                                                       unsigned short argumentCount,
                                                       PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->uniform2fv(arguments, argumentCount);
    }
    JsValueRef uniform2fv(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_uniform2i = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticUniform2i(JsValueRef callee,
                                                      bool isConstructCall,
                                                      JsValueRef* arguments,
                                                      unsigned short argumentCount,
                                                      PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->uniform2i(arguments, argumentCount);
    }
    JsValueRef uniform2i(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_uniform2iv = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticUniform2iv(JsValueRef callee,
                                                       bool isConstructCall,
                                                       JsValueRef* arguments,
                                                       unsigned short argumentCount,
                                                       PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->uniform2iv(arguments, argumentCount);
    }
    JsValueRef uniform2iv(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_uniform3f = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticUniform3f(JsValueRef callee,
                                                      bool isConstructCall,
                                                      JsValueRef* arguments,
                                                      unsigned short argumentCount,
                                                      PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->uniform3f(arguments, argumentCount);
    }
    JsValueRef uniform3f(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_uniform3fv = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticUniform3fv(JsValueRef callee,
                                                       bool isConstructCall,
                                                       JsValueRef* arguments,
                                                       unsigned short argumentCount,
                                                       PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->uniform3fv(arguments, argumentCount);
    }
    JsValueRef uniform3fv(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_uniform3i = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticUniform3i(JsValueRef callee,
                                                      bool isConstructCall,
                                                      JsValueRef* arguments,
                                                      unsigned short argumentCount,
                                                      PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->uniform3i(arguments, argumentCount);
    }
    JsValueRef uniform3i(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_uniform3iv = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticUniform3iv(JsValueRef callee,
                                                       bool isConstructCall,
                                                       JsValueRef* arguments,
                                                       unsigned short argumentCount,
                                                       PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->uniform3iv(arguments, argumentCount);
    }
    JsValueRef uniform3iv(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_uniform4f = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticUniform4f(JsValueRef callee,
                                                      bool isConstructCall,
                                                      JsValueRef* arguments,
                                                      unsigned short argumentCount,
                                                      PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->uniform4f(arguments, argumentCount);
    }
    JsValueRef uniform4f(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_uniform4fv = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticUniform4fv(JsValueRef callee,
                                                       bool isConstructCall,
                                                       JsValueRef* arguments,
                                                       unsigned short argumentCount,
                                                       PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->uniform4fv(arguments, argumentCount);
    }
    JsValueRef uniform4fv(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_uniform4i = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticUniform4i(JsValueRef callee,
                                                      bool isConstructCall,
                                                      JsValueRef* arguments,
                                                      unsigned short argumentCount,
                                                      PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->uniform4i(arguments, argumentCount);
    }
    JsValueRef uniform4i(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_uniform4iv = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticUniform4iv(JsValueRef callee,
                                                       bool isConstructCall,
                                                       JsValueRef* arguments,
                                                       unsigned short argumentCount,
                                                       PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->uniform4iv(arguments, argumentCount);
    }
    JsValueRef uniform4iv(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_uniformMatrix2fv = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticUniformMatrix2fv(JsValueRef callee,
                                                             bool isConstructCall,
                                                             JsValueRef* arguments,
                                                             unsigned short argumentCount,
                                                             PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->uniformMatrix2fv(arguments, argumentCount);
    }
    JsValueRef uniformMatrix2fv(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_uniformMatrix3fv = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticUniformMatrix3fv(JsValueRef callee,
                                                             bool isConstructCall,
                                                             JsValueRef* arguments,
                                                             unsigned short argumentCount,
                                                             PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->uniformMatrix3fv(arguments, argumentCount);
    }
    JsValueRef uniformMatrix3fv(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_uniformMatrix4fv = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticUniformMatrix4fv(JsValueRef callee,
                                                             bool isConstructCall,
                                                             JsValueRef* arguments,
                                                             unsigned short argumentCount,
                                                             PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->uniformMatrix4fv(arguments, argumentCount);
    }
    JsValueRef uniformMatrix4fv(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_stencilFunc = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticStencilFunc(JsValueRef callee,
                                                        bool isConstructCall,
                                                        JsValueRef* arguments,
                                                        unsigned short argumentCount,
                                                        PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->stencilFunc(arguments, argumentCount);
    }
    JsValueRef stencilFunc(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_stencilMask = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticStencilMask(JsValueRef callee,
                                                        bool isConstructCall,
                                                        JsValueRef* arguments,
                                                        unsigned short argumentCount,
                                                        PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->stencilMask(arguments, argumentCount);
    }
    JsValueRef stencilMask(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_stencilOp = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticStencilOp(JsValueRef callee,
                                                      bool isConstructCall,
                                                      JsValueRef* arguments,
                                                      unsigned short argumentCount,
                                                      PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->stencilOp(arguments, argumentCount);
    }
    JsValueRef stencilOp(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_lineWidth = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticLineWidth(JsValueRef callee,
                                                      bool isConstructCall,
                                                      JsValueRef* arguments,
                                                      unsigned short argumentCount,
                                                      PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->lineWidth(arguments, argumentCount);
    }
    JsValueRef lineWidth(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_polygonOffset = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticPolygonOffset(JsValueRef callee,
                                                          bool isConstructCall,
                                                          JsValueRef* arguments,
                                                          unsigned short argumentCount,
                                                          PVOID callbackData)
    {
        return reinterpret_cast<WebGLProjections*>(callbackData)->polygonOffset(arguments, argumentCount);
    }
    JsValueRef CALLBACK polygonOffset(JsValueRef* arguments, unsigned short argumentCount);

    static const GLenum UNPACK_FLIP_Y_WEBGL = 0x9240;
    static const GLenum UNPACK_PREMULTIPLY_ALPHA_WEBGL = 0x9241;
};
}  // namespace WebGL
}  // namespace HoloJs
