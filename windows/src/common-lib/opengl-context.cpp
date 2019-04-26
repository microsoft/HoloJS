#include "stdafx.h"
#include "include/holojs/windows/mixed-reality/mixed-reality-context.h"
#include "include/holojs/windows/opengl-context.h"
#include "holojs/private/error-handling.h"

using namespace HoloJs::OpenGL;
using namespace std;

OpenGLContext::~OpenGLContext() { cleanup(); }

void OpenGLContext::cleanup()
{
    m_webGLSharedTexture.Reset();

    if (m_EglDisplay != EGL_NO_DISPLAY && m_EglSurface != EGL_NO_SURFACE) {
        eglDestroySurface(m_EglDisplay, m_EglSurface);
        m_EglSurface = EGL_NO_SURFACE;
    }

    if (m_EglDisplay != EGL_NO_DISPLAY && m_EglContext != EGL_NO_CONTEXT) {
        eglDestroyContext(m_EglDisplay, m_EglContext);
        m_EglContext = EGL_NO_CONTEXT;
    }

    if (m_EglDisplay != EGL_NO_DISPLAY) {
        eglTerminate(m_EglDisplay);
        m_EglDisplay = EGL_NO_DISPLAY;
    }

    if (m_EglDevice != nullptr) {
        PFNEGLRELEASEDEVICEANGLEPROC eglReleaseDeviceANGLE =
            reinterpret_cast<PFNEGLRELEASEDEVICEANGLEPROC>(eglGetProcAddress("eglReleaseDeviceANGLE"));

        eglReleaseDeviceANGLE(m_EglDevice);
    }
}

HRESULT OpenGLContext::initializeEGLDisplay()
{
    ID3D11Device* d3dDevice =
        m_mixedRealityContext ? m_mixedRealityContext->getMixedRealityDeviceResources()->getD3DDevice() : nullptr;

    PFNEGLCREATEDEVICEANGLEPROC eglCreateDeviceANGLE =
        reinterpret_cast<PFNEGLCREATEDEVICEANGLEPROC>(eglGetProcAddress("eglCreateDeviceANGLE"));
    RETURN_IF_TRUE(eglCreateDeviceANGLE == nullptr);

    if (d3dDevice != nullptr) {
        m_EglDevice = eglCreateDeviceANGLE(EGL_D3D11_DEVICE_ANGLE, reinterpret_cast<void*>(d3dDevice), nullptr);
    }

    PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT =
        reinterpret_cast<PFNEGLGETPLATFORMDISPLAYEXTPROC>(eglGetProcAddress("eglGetPlatformDisplayEXT"));
    RETURN_IF_TRUE(eglGetPlatformDisplayEXT == nullptr);

    if (m_EglDisplay == EGL_NO_DISPLAY) {
        if (d3dDevice != nullptr) {
            m_EglDisplay = eglGetPlatformDisplayEXT(EGL_PLATFORM_DEVICE_EXT, m_EglDevice, nullptr);
        } else {
            const EGLint defaultDisplayAttributes[] = {
                EGL_PLATFORM_ANGLE_TYPE_ANGLE,
                EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,
                EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE,
                EGL_TRUE,
                EGL_NONE,
            };

            m_EglDisplay =
                eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY, defaultDisplayAttributes);
        }

        RETURN_IF_TRUE(m_EglDisplay == EGL_NO_DISPLAY);

        RETURN_IF_TRUE(eglInitialize(m_EglDisplay, NULL, NULL) == EGL_FALSE);
    }

    return S_OK;
}

HRESULT OpenGLContext::initializeEGLContext()
{
    // Initialize OpenGL
    const EGLint configAttributes[] = {EGL_RED_SIZE,
                                       8,
                                       EGL_GREEN_SIZE,
                                       8,
                                       EGL_BLUE_SIZE,
                                       8,
                                       EGL_ALPHA_SIZE,
                                       8,
                                       EGL_DEPTH_SIZE,
                                       8,
                                       EGL_STENCIL_SIZE,
                                       8,
                                       EGL_NONE};

    const EGLint contextAttributes[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};

    EGLint numConfigs = 0;
    RETURN_IF_TRUE(eglChooseConfig(m_EglDisplay, configAttributes, &m_EglConfig, 1, &numConfigs) == EGL_FALSE);
    RETURN_IF_TRUE(numConfigs == 0);

    if (m_EglContext == EGL_NO_CONTEXT) {
        m_EglContext = eglCreateContext(m_EglDisplay, m_EglConfig, EGL_NO_CONTEXT, contextAttributes);
        RETURN_IF_TRUE(m_EglContext == EGL_NO_CONTEXT);
    }

    return S_OK;
}

HRESULT OpenGLContext::initializeEGLSurface()
{
    if (!m_mixedRealityContext) {
        if (m_EglSurface == EGL_NO_SURFACE) {
            RETURN_IF_FAILED(createEGLWindowSurface());

            //m_EglSurface = eglCreateWindowSurface(m_EglDisplay, m_EglConfig, m_window, nullptr);
        }
    } else {
        RETURN_IF_FAILED(
            initializeSharedTexture(m_mixedRealityContext->getMixedRealityDeviceResources()->getD3DDevice(),
                                    static_cast<UINT>(m_mixedRealityContext->getWidth() * 2),
                                    static_cast<UINT>(m_mixedRealityContext->getHeight())));

        RETURN_IF_FAILED(createPbufferFromSharedTexture(static_cast<UINT>(m_mixedRealityContext->getWidth() * 2),
                                                        static_cast<UINT>(m_mixedRealityContext->getHeight())));
    }

    return S_OK;
}

HRESULT OpenGLContext::initializeSharedTexture(ID3D11Device* d3dDevice, unsigned int width, unsigned int height)
{
    // Create DX shared texture
    D3D11_TEXTURE2D_DESC texDesc = {0};
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;

    RETURN_IF_FAILED(d3dDevice->CreateTexture2D(&texDesc, nullptr, m_webGLSharedTexture.ReleaseAndGetAddressOf()));
    return S_OK;
}

HRESULT OpenGLContext::createPbufferFromSharedTexture(unsigned int width, unsigned int height)
{
    IDXGIResource* dxgiResource;
    HANDLE sharedHandle;
    RETURN_IF_FAILED(m_webGLSharedTexture->QueryInterface(&dxgiResource));

    RETURN_IF_FAILED(dxgiResource->GetSharedHandle(&sharedHandle));

    EGLint pBufferAttributes[] = {EGL_WIDTH,
                                  static_cast<EGLint>(width),
                                  EGL_HEIGHT,
                                  static_cast<EGLint>(height),
                                  EGL_TEXTURE_TARGET,
                                  EGL_TEXTURE_2D,
                                  EGL_TEXTURE_FORMAT,
                                  EGL_TEXTURE_RGBA,
                                  EGL_NONE};

    // Create Pbuffer on the shared texture
    m_EglSurface = eglCreatePbufferFromClientBuffer(
        m_EglDisplay, EGL_D3D_TEXTURE_2D_SHARE_HANDLE_ANGLE, sharedHandle, m_EglConfig, pBufferAttributes);
    RETURN_IF_TRUE(m_EglSurface == EGL_NO_SURFACE);

    return S_OK;
}

HRESULT OpenGLContext::initialize()
{
    RETURN_IF_FAILED(initializeEGLDisplay());

    RETURN_IF_FAILED(initializeEGLContext());

    RETURN_IF_FAILED(initializeEGLSurface());

    RETURN_IF_TRUE(eglMakeCurrent(m_EglDisplay, m_EglSurface, m_EglSurface, m_EglContext) == EGL_FALSE);

    return S_OK;
}

HRESULT OpenGLContext::swap()
{
    if (!m_mixedRealityContext) {
        if (eglSwapBuffers(m_EglDisplay, m_EglSurface) != GL_TRUE) {
            cleanup();
            return E_FAIL;
        }
    }

    return S_OK;
}