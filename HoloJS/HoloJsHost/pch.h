#pragma once

#include <collection.h>
#include <ppltasks.h>
#include <experimental\resumable>
#include <pplawait.h>

// Chakra stuff
#define USE_EDGEMODE_JSRT
#define CHAKRA_CALLBACK CALLBACK
#define CHAKRA_API STDAPI_(JsErrorCode)
typedef DWORD_PTR ChakraCookie;
typedef BYTE* ChakraBytePtr;
#include <jsrt.h>
#include <chakrart.h>

#include <filesystem>
#include <memory>

#include <wrl.h>
#include <robuffer.h>

// Enable function definitions in the GL headers below
#define GL_GLEXT_PROTOTYPES

// OpenGL ES includes
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

// EGL includes
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>

// ANGLE include for Windows Store
#include <angle_windowsstore.h>

//WIC
#include <wincodec.h>

// Media Foundation
#include <mfapi.h>
#include <mfidl.h>
#include <Mfreadwrite.h>

//D3D
#include <d3d11.h>

void Log(PCSTR file, int line);

#define EXIT_IF_JS_ERROR(x) do { if ((x) != JsNoError) { Log(__FILE__, __LINE__); return; } } while (0, 0)
#define EXIT_IF_FALSE(x) do { if ((x) == false) { Log(__FILE__, __LINE__); return; } } while (0, 0)
#define EXIT_IF_FAILED(x) do { if (FAILED(x)) { Log(__FILE__, __LINE__); return; } } while (0, 0)
#define RETURN_IF_JS_ERROR(x) do { if ((x) != JsNoError) { Log(__FILE__, __LINE__); return false; } } while (0, 0)
#define RETURN_IF_FAILED(x) do { if (FAILED(x)) { Log(__FILE__, __LINE__); return false; } } while (0, 0)
#define RETURN_NULL_IF_JS_ERROR(x) do { if ((x) != JsNoError) { Log(__FILE__, __LINE__); return nullptr; } } while (0, 0)
#define RETURN_IF_FALSE(x) do { if ((x) == false) { Log(__FILE__, __LINE__); return false; } } while (0, 0)
#define RETURN_IF_NULL(x) do { if ((x) == nullptr) { Log(__FILE__, __LINE__); return false; } } while (0, 0)
#define RETURN_NULL_IF_FALSE(x) do { if ((x) == false) { Log(__FILE__, __LINE__); return nullptr; } } while (0, 0)
#define RETURN_NULL_IF_FAILED(x) do { if (FAILED(x)) { Log(__FILE__, __LINE__); return nullptr; } } while (0, 0)
#define RETURN_INVALID_REF_IF_FALSE(x) do { if ((x) == false) { Log(__FILE__, __LINE__); return JS_INVALID_REFERENCE; } } while (0, 0)
#define RETURN_INVALID_REF_IF_TRUE(x) do { if ((x) == true) { Log(__FILE__, __LINE__); return JS_INVALID_REFERENCE; } } while (0, 0)
#define RETURN_INVALID_REF_IF_FAILED(x) do { if (FAILED(x)) { Log(__FILE__, __LINE__); return JS_INVALID_REFERENCE; } } while (0, 0)
#define RETURN_INVALID_REF_IF_NULL(x) do { if ((x) == nullptr) { Log(__FILE__, __LINE__); return JS_INVALID_REFERENCE; } } while (0, 0)
#define RETURN_INVALID_REF_IF_JS_ERROR(x) do { if ((x) != JsNoError) { Log(__FILE__, __LINE__); return JS_INVALID_REFERENCE; } } while (0, 0)

#include "IRelease.h"
#include "ObjectEvents.h"
#include "ExternalObject.h"
#include "ScriptHostUtilities.h"
#include "ScriptResourceTracker.h"

#include <windows.storage.streams.h>
#include <wrl/implements.h>
#include "IBufferOnMemory.h"

#include "MouseInput.h"
#include "KeyboardInput.h"
