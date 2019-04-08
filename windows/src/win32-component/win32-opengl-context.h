#pragma once

#include "holojs/windows/opengl-context.h"

namespace HoloJs {
namespace Win32 {

class Win32OpenGLContext : public HoloJs::OpenGL::OpenGLContext{
   public:
    Win32OpenGLContext() {}
    virtual ~Win32OpenGLContext() {}

    virtual HRESULT createEGLWindowSurface();
};

}  // namespace OpenGL
}  // namespace HoloJs
