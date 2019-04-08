#pragma once

__declspec(dllexport) void Log(const char* file, int line);
__declspec(dllexport) void PrintException();

#define HOLOJS_FAILED(x) ((x) < 0)
#define HOLOJS_SUCCEEDED(x) ((x) >= 0)

namespace HoloJs {
extern long Error;
extern long Success;
}  // namespace HoloJs

#define EXIT_IF_JS_ERROR(x)          \
    do {                             \
        if ((x) != JsNoError) {      \
            Log(__FILE__, __LINE__); \
            return;                  \
        }                            \
    } while (0, 0)
#define EXIT_IF_FALSE(x)             \
    do {                             \
        if ((x) == false) {          \
            Log(__FILE__, __LINE__); \
            return;                  \
        }                            \
    } while (0, 0)
#define EXIT_IF_TRUE(x)              \
    do {                             \
        if ((x) == true) {           \
            Log(__FILE__, __LINE__); \
            return;                  \
        }                            \
    } while (0, 0)
#define EXIT_IF_FAILED(x)            \
    do {                             \
        if (HOLOJS_FAILED(x)) {             \
            Log(__FILE__, __LINE__); \
            return;                  \
        }                            \
    } while (0, 0)
#define RETURN_IF_JS_ERROR(x)        \
    do {                             \
        if ((x) != JsNoError) {      \
            Log(__FILE__, __LINE__); \
            return HoloJs::Error;    \
        }                            \
    } while (0, 0)
#define RETURN_IF_FAILED(x)          \
    do {                             \
        if (HOLOJS_FAILED(x)) {      \
            Log(__FILE__, __LINE__); \
            return x;                \
        }                            \
    } while (0, 0)
#define RETURN_NULL_IF_JS_ERROR(x)   \
    do {                             \
        if ((x) != JsNoError) {      \
            Log(__FILE__, __LINE__); \
            return nullptr;          \
        }                            \
    } while (0, 0)
#define RETURN_IF_FALSE(x)           \
    do {                             \
        if ((x) == false) {          \
            Log(__FILE__, __LINE__); \
            return HoloJs::Error;    \
        }                            \
    } while (0, 0)
#define RETURN_IF_TRUE(x)            \
    do {                             \
        if ((x) == true) {           \
            Log(__FILE__, __LINE__); \
            return HoloJs::Error;    \
        }                            \
    } while (0, 0)
#define RETURN_IF_NULL(x)            \
    do {                             \
        if ((x) == nullptr) {        \
            Log(__FILE__, __LINE__); \
            return HoloJs::Error;    \
        }                            \
    } while (0, 0)
#define RETURN_NULL_IF_FALSE(x)      \
    do {                             \
        if ((x) == false) {          \
            Log(__FILE__, __LINE__); \
            return nullptr;          \
        }                            \
    } while (0, 0)
#define RETURN_NULL_IF_TRUE(x)       \
    do {                             \
        if ((x) == true) {          \
            Log(__FILE__, __LINE__); \
            return nullptr;          \
        }                            \
    } while (0, 0)
#define RETURN_NULL_IF_FAILED(x)     \
    do {                             \
        if (HOLOJS_FAILED(x)) {             \
            Log(__FILE__, __LINE__); \
            return nullptr;          \
        }                            \
    } while (0, 0)
#define RETURN_INVALID_REF_IF_FALSE(x)   \
    do {                                 \
        if ((x) == false) {              \
            Log(__FILE__, __LINE__);     \
            return JS_INVALID_REFERENCE; \
        }                                \
    } while (0, 0)
#define RETURN_INVALID_REF_IF_TRUE(x)    \
    do {                                 \
        if ((x) == true) {               \
            Log(__FILE__, __LINE__);     \
            return JS_INVALID_REFERENCE; \
        }                                \
    } while (0, 0)
#define RETURN_INVALID_REF_IF_FAILED(x)  \
    do {                                 \
        if (HOLOJS_FAILED(x)) {                 \
            Log(__FILE__, __LINE__);     \
            return JS_INVALID_REFERENCE; \
        }                                \
    } while (0, 0)
#define RETURN_INVALID_REF_IF_NULL(x)    \
    do {                                 \
        if ((x) == nullptr) {            \
            Log(__FILE__, __LINE__);     \
            return JS_INVALID_REFERENCE; \
        }                                \
    } while (0, 0)
#define RETURN_INVALID_REF_IF_JS_ERROR(x) \
    do {                                  \
        if ((x) != JsNoError) {           \
            Log(__FILE__, __LINE__);      \
            return JS_INVALID_REFERENCE;  \
        }                                 \
    } while (0, 0)

#define HANDLE_EXCEPTION_IF_JS_ERROR(x) \
    if ((x) != JsNoError) {             \
        PrintException();               \
    }
#define RETURN_ON_SCRIPT_ERROR(x) \
    if ((x) != JsNoError) {       \
        PrintException();         \
        return HoloJs::Error;     \
    }
