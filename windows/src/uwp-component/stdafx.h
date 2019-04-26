// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define HOLOJS_USE_JSRT

#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>

#include "holojs/private/error-handling.h"
// reference additional headers your program requires here

#include <d2d1_3.h>
#include <d2d1effects_2.h>
#include <d3d11_3.h>
#include <dwrite_3.h>
#include <dxgi1_4.h>

#include <DirectXColors.h>
#include <DirectXMath.h>
#include <agile.h>
#include <concrt.h>
#include <memory>
#include <wincodec.h>
#include <wrl.h>
#include <wrl/client.h>

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
