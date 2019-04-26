#include "stdafx.h"
#include "uwp-mixed-reality-context.h"
#include "uwp-spatial-input.h"

using namespace HoloJs::UWP;
using namespace Windows::Graphics::Holographic;
using namespace std;

HRESULT UWPMixedRealityContext::createHolographicSpace()

{
    RETURN_IF_FALSE(m_coreWindow);

    m_holographicSpace = HolographicSpace::CreateForCoreWindow(m_coreWindow.Get());

    return S_OK;
}