#pragma once

#ifdef HOLOJS_USE_JSRT

// The Chakra headers in the SDK do not expose some functions for store apps, even
// though they are documented as store compatible on MSDN. This header
// fudges #defines to get access to those functions
#define USE_EDGEMODE_JSRT

#define CHAKRA_CALLBACK CALLBACK
typedef unsigned char* ChakraBytePtr;
#include <jsrt.h>
#else
#include <ChakraCore.h>
#endif


#define JS_PROJECTION_DEFINE(iface, name)                                                   \
    JsValueRef m_##name = JS_INVALID_REFERENCE;                                             \
    static JsValueRef CHAKRA_CALLBACK static_##name(JsValueRef callee,                      \
                                                    bool isConstructCall,                   \
                                                    JsValueRef* arguments,                  \
                                                    unsigned short argumentCount,           \
                                                    PVOID callbackData)                     \
    {                                                                                       \
        return reinterpret_cast<##iface*>(callbackData)->_##name(arguments, argumentCount); \
    }                                                                                       \
    JsValueRef _##name(JsValueRef* arguments, unsigned short argumentCount);

#define JS_PROJECTION_REGISTER(namespaceName, scriptName, nativeName) \
    RETURN_IF_FAILED(                                                 \
        ScriptHostUtilities::ProjectFunction(scriptName, namespaceName, static_##nativeName, this, &m_##nativeName));