#include "stdafx.h"
#include "holojs/windows/mixed-reality/mixed-reality-context.h"
#include "win32-opengl-context.h"
#include "holojs/private/error-handling.h"

using namespace HoloJs::OpenGL;
using namespace std;
using namespace HoloJs::Win32;


HRESULT Win32OpenGLContext::createEGLWindowSurface()
{
    m_EglSurface = eglCreateWindowSurface(m_EglDisplay, m_EglConfig, m_window, nullptr);
    RETURN_IF_TRUE(m_EglSurface == EGL_NO_SURFACE);

    return S_OK;
}