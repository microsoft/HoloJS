#pragma once

#include "holojs/private/holojs-script-host-internal.h"
#include <functional>
#include <vector>
#include <string>

namespace HoloJs {
namespace Platforms {
namespace Win32 {
class QrScanner {
   public:
    QrScanner(HoloJs::IHoloJsScriptHostInternal* host) : m_host(host) {}

    HRESULT tryDecode();

    void setOnResultsCallback(std::function<void(std::wstring text)> callback)
    {
        m_callback = callback;
    }

   private:

    std::function<void(std::wstring text)> m_callback;
    bool m_isBusy = false;

    HoloJs::IHoloJsScriptHostInternal* m_host;
};

}  // namespace Win32
}  // namespace Platform
}  // namespace HoloJs
