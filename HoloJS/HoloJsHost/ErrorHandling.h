#pragma once

void Log(PCSTR file, int line);

#define EXIT_IF_JS_ERROR(x) do { if ((x) != JsNoError) { Log(__FILE__, __LINE__); return; } } while (0, 0)
#define EXIT_IF_FALSE(x) do { if ((x) == false) { Log(__FILE__, __LINE__); return; } } while (0, 0)
#define EXIT_IF_TRUE(x) do { if ((x) == true) { Log(__FILE__, __LINE__); return; } } while (0, 0)
#define EXIT_IF_FAILED(x) do { if (FAILED(x)) { Log(__FILE__, __LINE__); return; } } while (0, 0)
#define RETURN_IF_JS_ERROR(x) do { if ((x) != JsNoError) { Log(__FILE__, __LINE__); return false; } } while (0, 0)
#define RETURN_IF_FAILED(x) do { if (FAILED(x)) { Log(__FILE__, __LINE__); return x; } } while (0, 0)
#define RETURN_NULL_IF_JS_ERROR(x) do { if ((x) != JsNoError) { Log(__FILE__, __LINE__); return nullptr; } } while (0, 0)
#define RETURN_IF_FALSE(x) do { if ((x) == false) { Log(__FILE__, __LINE__); return false; } } while (0, 0)
#define RETURN_IF_TRUE(x) do { if ((x) == true) { Log(__FILE__, __LINE__); return false; } } while (0, 0)
#define RETURN_IF_NULL(x) do { if ((x) == nullptr) { Log(__FILE__, __LINE__); return false; } } while (0, 0)
#define RETURN_NULL_IF_FALSE(x) do { if ((x) == false) { Log(__FILE__, __LINE__); return nullptr; } } while (0, 0)
#define RETURN_NULL_IF_FAILED(x) do { if (FAILED(x)) { Log(__FILE__, __LINE__); return nullptr; } } while (0, 0)
#define RETURN_INVALID_REF_IF_FALSE(x) do { if ((x) == false) { Log(__FILE__, __LINE__); return JS_INVALID_REFERENCE; } } while (0, 0)
#define RETURN_INVALID_REF_IF_TRUE(x) do { if ((x) == true) { Log(__FILE__, __LINE__); return JS_INVALID_REFERENCE; } } while (0, 0)
#define RETURN_INVALID_REF_IF_FAILED(x) do { if (FAILED(x)) { Log(__FILE__, __LINE__); return JS_INVALID_REFERENCE; } } while (0, 0)
#define RETURN_INVALID_REF_IF_NULL(x) do { if ((x) == nullptr) { Log(__FILE__, __LINE__); return JS_INVALID_REFERENCE; } } while (0, 0)
#define RETURN_INVALID_REF_IF_JS_ERROR(x) do { if ((x) != JsNoError) { Log(__FILE__, __LINE__); return JS_INVALID_REFERENCE; } } while (0, 0)

#define HANDLE_EXCEPTION_IF_JS_ERROR(x) if ((x) != JsNoError) { HologramJS::ScriptErrorHandling::PrintException();}
#define RETURN_ON_SCRIPT_ERROR(x) if ((x) != JsNoError) { HologramJS::ScriptErrorHandling::PrintException(); return false; }
