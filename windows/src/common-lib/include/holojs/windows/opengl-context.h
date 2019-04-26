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
        EGLint width;
        eglQuerySurface(m_EglDisplay, m_EglSurface, EGL_WIDTH, &width);
        return width;
    }

    int getHeight()
    {
        EGLint height;
        eglQuerySurface(m_EglDisplay, m_EglSurface, EGL_HEIGHT, &height);
        return height;
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
};

}  // namespace OpenGL
}  // namespace HoloJs
