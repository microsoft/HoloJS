#include "stdafx.h"
#include "../host-interfaces.h"
#include "win32-mixed-reality-context.h"
#include "holojs/private/script-host-utilities.h"
#include "win32-spatial-input.h"
#include <..\um\HolographicSpaceInterop.h>
#include <..\winrt\WinRTBase.h>
#include <ppltasks.h>
#include <windows.graphics.holographic.h>

using namespace HoloJs::Win32;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace Windows::Graphics::Holographic;
using namespace std;
using namespace std::placeholders;
using namespace concurrency;
using namespace Windows::Foundation;
using namespace concurrency;
using namespace HoloJs::Interfaces;

HRESULT Win32MixedRealityContext::createHolographicSpace()

{
    RETURN_IF_TRUE(m_window == nullptr);

    ComPtr<ABI::Windows::Graphics::Holographic::IHolographicSpaceStatics> spHolographicSpaceFactory;
    RETURN_IF_FAILED(ABI::Windows::Foundation::GetActivationFactory(
        HStringReference(RuntimeClass_Windows_Graphics_Holographic_HolographicSpace).Get(),
        &spHolographicSpaceFactory));

    ComPtr<IHolographicSpaceInterop> spHolographicSpaceInterop;
    RETURN_IF_FAILED(spHolographicSpaceFactory.As(&spHolographicSpaceInterop));

    ComPtr<ABI::Windows::Graphics::Holographic::IHolographicSpace> holographicSpaceHWND;
    RETURN_IF_FAILED(spHolographicSpaceInterop->CreateForWindow(m_window, IID_PPV_ARGS(&holographicSpaceHWND)));

    m_holographicSpace = safe_cast<Windows::Graphics::Holographic::IHolographicSpace ^>(
        reinterpret_cast<Platform::Object ^>(holographicSpaceHWND.Get()));

    return S_OK;
}
