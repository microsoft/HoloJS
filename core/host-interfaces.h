#pragma once

#include "include/holojs/private/chakra.h"
#include "include/holojs/console.h"
#include "timers.h"
#include <string>
#include <GLES2/gl2.h>

namespace HoloJs {
namespace Interfaces {

class JsRefReleaseAtScopeExit {
   public:
    JsRefReleaseAtScopeExit(JsValueRef ref) : m_ref(ref) {}

    ~JsRefReleaseAtScopeExit() { JsRelease(m_ref, nullptr); }

   private:
    JsValueRef m_ref;
};

class Console {
   public:
    Console() {}
    ~Console() {}

    long initialize();

    void setConsoleConfig(HoloJs::IConsoleConfiguration* consoleConfig) { m_config = consoleConfig; }

   private:
    JsValueRef m_logFunction = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK logStatic(JsValueRef callee,
                                                bool isConstructCall,
                                                JsValueRef* arguments,
                                                unsigned short argumentCount,
                                                PVOID callbackData);

    JsValueRef m_debugFunction = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK debugStatic(JsValueRef callee,
                                                  bool isConstructCall,
                                                  JsValueRef* arguments,
                                                  unsigned short argumentCount,
                                                  PVOID callbackData);

    JsValueRef log(JsValueRef callee, JsValueRef* arguments, unsigned short argumentCount);
    JsValueRef debug(JsValueRef callee, JsValueRef* arguments, unsigned short argumentCount);
    HoloJs::IConsoleConfiguration* m_config = nullptr;
};

class PromiseContinuation {
   public:
    PromiseContinuation() {}
    ~PromiseContinuation() {}

    long initialize();

   private:
    static void CALLBACK staticPromiseContinuationCallback(JsValueRef task, void* callbackState);
    void promiseContinuationCallback(JsValueRef task);
};

}  // namespace Interfaces
}  // namespace HoloJs
