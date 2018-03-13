#pragma once

#include <windows.h>

// The Chakra headers in the SDK do not expose some functions for store apps, even
// though they are documented as store compatible on MSDN. This header
// fudges #defines to get access to those functions
#define USE_EDGEMODE_JSRT
#define CHAKRA_CALLBACK CALLBACK
#define CHAKRA_API STDAPI_(JsErrorCode)
typedef DWORD_PTR ChakraCookie;
typedef BYTE* ChakraBytePtr;

#include <jsrt.h>
#include <chakrart.h>

class JsRefReleaseAtScopeExit {
public:
    JsRefReleaseAtScopeExit(JsValueRef ref) : m_ref(ref) {}

    ~JsRefReleaseAtScopeExit() { JsRelease(m_ref, nullptr); }

private:
    JsValueRef m_ref;
};

class JsCallAtScopeExit {
public:
    JsCallAtScopeExit(JsValueRef func) : m_function(func) {}

    void Revoke() { m_function = JS_INVALID_REFERENCE; }

    ~JsCallAtScopeExit() { if (m_function != JS_INVALID_REFERENCE) JsCallFunction(m_function, &m_function, 1, nullptr); }

private:
    JsValueRef m_function;
};
