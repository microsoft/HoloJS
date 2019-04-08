#pragma once

#include "holojs/windows/opengl-context.h"

namespace HoloJs {
namespace Win32 {

class UWPOpenGLContext : public HoloJs::OpenGL::OpenGLContext {
   public:
    UWPOpenGLContext() {}
    virtual ~UWPOpenGLContext() {}

    virtual HRESULT createEGLWindowSurface();
};

}  // namespace Win32
}  // namespace HoloJs
