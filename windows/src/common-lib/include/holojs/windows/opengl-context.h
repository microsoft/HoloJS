#pragma once

// Enable function definitions in the GL headers below
#define GL_GLEXT_PROTOTYPES
#define EGL_EGL_PROTOTYPES 1

// OpenGL ES includes
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

// EGL includes
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>

#include <d3d11_4.h>

namespace HoloJs {
namespace MixedReality {
class MixedRealityContext;
}
namespace OpenGL {

class OpenGLContext {
   public:
    OpenGLContext() {}
    ~OpenGLContext();

    void setWindow(HWND window) { m_window = window; }

    void setCoreWindow(Platform::Agile<Windows::UI::Core::CoreWindow> coreWindow) { m_coreWindow = coreWindow; }

    void setOffscreenRendering(ID3D11Texture2D* renderSurface)
    {
        m_isOffscreenRendering = true;
        D3D11_TEXTURE2D_DESC renderSurfaceDesc;
        renderSurface->GetDesc(&renderSurfaceDesc);

		m_offscreenRenderWidth = renderSurfaceDesc.Width;
        m_offscreenRenderHeight = renderSurfaceDesc.Height;

        m_webGLSharedTexture = renderSurface;
    }

    void setMixedRealityContext(std::shared_ptr<HoloJs::MixedReality::MixedRealityContext> mixedRealityContext)
    {
        m_mixedRealityContext = mixedRealityContext;
    }

    HRESULT initialize();
    HRESULT swap();

    ID3D11Texture2D* getWebGLTexture() { return m_webGLSharedTexture.Get(); }

    virtual HRESULT createEGLWindowSurface() = 0;

    int getWidth()
    {
        if (m_isOffscreenRendering) {
            return m_offscreenRenderWidth;
        } else {
            EGLint width;
            eglQuerySurface(m_EglDisplay, m_EglSurface, EGL_WIDTH, &width);
            return width;
        }
    }

    int getHeight()
    {
        if (m_isOffscreenRendering) {
            return m_offscreenRenderHeight;
        } else {
            EGLint height;
            eglQuerySurface(m_EglDisplay, m_EglSurface, EGL_HEIGHT, &height);
            return height;
        }
    }

   protected:
    Platform::Agile<Windows::UI::Core::CoreWindow> m_coreWindow;
    EGLDisplay m_EglDisplay;
    EGLConfig m_EglConfig;
    EGLSurface m_EglSurface;
    HWND m_window;

   private:
    void cleanup();

    EGLDeviceEXT m_EglDevice;

    EGLContext m_EglContext;

    std::shared_ptr<HoloJs::MixedReality::MixedRealityContext> m_mixedRealityContext;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_webGLSharedTexture;

    HRESULT initializeEGLDisplay();

    HRESULT initializeEGLContext();

    HRESULT initializeSharedTexture(ID3D11Device* d3dDevice, unsigned int width, unsigned int height);

    HRESULT initializeEGLSurface();

    HRESULT createPbufferFromSharedTexture(unsigned int width, unsigned int height);

    bool m_isOffscreenRendering = false;
    unsigned int m_offscreenRenderWidth;
    unsigned int m_offscreenRenderHeight;
};

}  // namespace OpenGL
}  // namespace HoloJs
