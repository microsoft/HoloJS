#pragma once

#include "holojs/holojs-script-host.h"
#include "holojs/private/holojs-view.h"
#include "holojs/private/script-host-errors.h"
#include "holojs/private/script-window.h"
#include "holojs/private/timers.h"
#include "holojs/private/holojs-script-host-internal.h"
#include "win32-base-view.h"
#include "win32-mixed-reality-context.h"
#include "win32-opengl-context.h"
#include "win32-timers-implementation.h"
#include <memory>
#include <wrl.h>
#include <mutex>

namespace HoloJs {
namespace Win32 {

class Win32HoloJsEmbeddedView : public IHoloJsView, public Win32HoloJsBaseView {
   public:
    Win32HoloJsEmbeddedView(HoloJs::ViewConfiguration viewConfiguration) {}
    virtual ~Win32HoloJsEmbeddedView() { stop(); }

    virtual HRESULT initialize(HoloJs::IHoloJsScriptHostInternal* host);
    virtual void setViewWindow(void* window) { m_window = reinterpret_cast<HWND>(window); }
    virtual void run();
    virtual void stop();

    virtual void onError(HoloJs::ScriptHostErrorType errorType);

    virtual HRESULT executeApp(std::shared_ptr<HoloJs::AppModel::HoloJsApp> app);

    virtual long executeOnViewThread(HoloJs::IForegroundWorkItem* workItem);
    virtual long executeInBackground(HoloJs::IBackgroundWorkItem* workItem);

	virtual long getStationaryCoordinateSystem(void** coordinateSystem)
    {
        return E_FAIL;
    }

    virtual HRESULT executeScript(const wchar_t* script);
    HRESULT executeScriptImmediate(const wchar_t* script);

    void onResize(int width, int height) {
		m_width = width;
		m_height = height;
		if (m_windowElement != nullptr) {
			m_windowElement->resize(width, height); 
		}
		
	}

    virtual bool isWindingOrderReversed() { return false; }

    void tick();
    virtual void runApp(std::shared_ptr<HoloJs::AppModel::HoloJsApp> app);

    virtual void setIcon(void* platformIcon)
    { /* not available in embedded view */
    }

    virtual void setTitle(const std::wstring& title)
    { /* not available in embedded view */
    }

   private:

    HWND m_window;
	int m_width, m_height;

    std::unique_ptr<HoloJs::Timers> m_timers;

    HoloJs::IHoloJsScriptHostInternal* m_host;
    bool m_isInitialized = false;

    void render();

    D3D_DRIVER_TYPE m_driverType = D3D_DRIVER_TYPE_NULL;
    D3D_FEATURE_LEVEL m_featureLevel = D3D_FEATURE_LEVEL_11_0;

    Microsoft::WRL::ComPtr<ID3D11Device> m_d3dDevice = nullptr;
    Microsoft::WRL::ComPtr<ID3D11Device1> m_d3dDevice1 = nullptr;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_immediateContext = nullptr;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext1> m_immediateContext1 = nullptr;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain = nullptr;
    Microsoft::WRL::ComPtr<IDXGISwapChain1> m_swapChain1 = nullptr;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView = nullptr;

    std::unique_ptr<OpenGL::OpenGLContext> m_openglContext;

    HRESULT onOpenGLDeviceLost();

	HoloJs::ViewConfiguration m_viewConfiguration;
};
}  // namespace Win32
}  // namespace HoloJs
