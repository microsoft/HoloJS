#pragma once

#include "include/holojs/private/chakra.h"
#include "include/holojs/private/event-target.h"
#include <map>
#include <string>
#include <vector>

namespace HoloJs {
namespace Interfaces {

class DOMEventRegistration {
   public:
    DOMEventRegistration() {}
    ~DOMEventRegistration() {}

    long initialize();

   private:
    JS_PROJECTION_WITH_CONTEXT_DEFINE(DOMEventRegistration, addEventListener)
    JS_PROJECTION_WITH_CONTEXT_DEFINE(DOMEventRegistration, removeEventListener)
    JS_PROJECTION_DEFINE(DOMEventRegistration, dispatchEvent)
};

}  // namespace Interfaces
}  // namespace HoloJs