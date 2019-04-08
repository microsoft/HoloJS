#include "stdafx.h"
#include "holojs/windows/mixed-reality/mixed-reality-context.h"
#include "holojs/private/error-handling.h"
#include "uwp-opengl-context.h"

using namespace HoloJs::OpenGL;
using namespace std;
using namespace HoloJs::Win32;
using namespace Windows::Foundation::Collections;

HRESULT UWPOpenGLContext::createEGLWindowSurface()
{
    PropertySet ^ surfaceCreationProperties = ref new PropertySet();
    surfaceCreationProperties->Insert(ref new Platform::String(L"EGLNativeWindowTypeProperty"), m_coreWindow.Get());

    m_EglSurface = eglCreateWindowSurface(
        m_EglDisplay, m_EglConfig, reinterpret_cast<IInspectable*>(surfaceCreationProperties), nullptr);
    RETURN_IF_TRUE(m_EglSurface == EGL_NO_SURFACE);

    return S_OK;
}