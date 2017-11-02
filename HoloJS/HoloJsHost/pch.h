#pragma once

#include <collection.h>
#include <ppltasks.h>
#include <experimental\resumable>
#include <pplawait.h>

#include <filesystem>
#include <memory>

#include <wrl.h>
#include <robuffer.h>

#include <WindowsNumerics.h>

// Enable function definitions in the GL headers below
#define GL_GLEXT_PROTOTYPES

// OpenGL ES includes
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

// EGL includes
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>

//WIC
#include <wincodec.h>

// Media Foundation
#include <mfapi.h>
#include <mfidl.h>
#include <Mfreadwrite.h>

//D3D
#include <d3d11.h>

#include "ErrorHandling.h"