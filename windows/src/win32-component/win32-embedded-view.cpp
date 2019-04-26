#include "stdafx.h"
#include "win32-embedded-view.h"
#include "win32-spatial-input.h"
#include "win32-timers-implementation.h"
#include <commctrl.h>
#include <d3d11_1.h>
#include <directxcolors.h>
#include <memory>

using namespace HoloJs::Win32;
using namespace HoloJs::AppModel;
using namespace std;
using namespace concurrency;

#define STOP_EXECUTION WM_USER
#define EXECUTE_SCRIPT (WM_USER + 1)
#define RESIZE_NOTIFICATION (WM_USER + 3)

// Step 4: the Window Procedure
LRESULT CALLBACK
SubclassWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    Win32HoloJsEmbeddedView* window = reinterpret_cast<Win32HoloJsEmbeddedView*>(dwRefData);

    switch (msg) {
        case EXECUTE_SCRIPT: {
            window->executeScriptImmediate(reinterpret_cast<wchar_t*>(lParam));
            delete reinterpret_cast<wchar_t*>(lParam);
            break;
        }

        case WM_CLOSE:
            window->stop();
            break;

        case WM_KEYDOWN:
            window->onKeyDown(wParam, lParam);
            return DefWindowProc(hwnd, msg, wParam, lParam);

        case WM_KEYUP:
            window->onKeyUp(wParam, lParam);
            return DefWindowProc(hwnd, msg, wParam, lParam);

        case WM_MOUSEWHEEL:
            window->onMouseWheelEvent(HoloJs::MouseButtonEventType::Wheel, wParam, lParam);
            break;

        case WM_MOUSEMOVE: {
            SetFocus(hwnd);
            window->onMouseEvent(HoloJs::MouseButtonEventType::Move, wParam, lParam);
        } break;

        case WM_RBUTTONDBLCLK:
            window->onMouseButtonEvent(
                HoloJs::MouseButtonEventType::DblClick, wParam, lParam, HoloJs::MouseButton::Second);
            break;

        case WM_MBUTTONDBLCLK:
            window->onMouseButtonEvent(
                HoloJs::MouseButtonEventType::DblClick, wParam, lParam, HoloJs::MouseButton::Auxiliary);
            break;

        case WM_LBUTTONDBLCLK:
            window->onMouseButtonEvent(
                HoloJs::MouseButtonEventType::DblClick, wParam, lParam, HoloJs::MouseButton::Main);
            break;

        case WM_RBUTTONUP:
            window->onMouseButtonEvent(HoloJs::MouseButtonEventType::Up, wParam, lParam, HoloJs::MouseButton::Second);
            break;

        case WM_LBUTTONUP:
            window->onMouseButtonEvent(
                HoloJs::MouseButtonEventType::Up, wParam, lParam, HoloJs::MouseButton::Auxiliary);
            break;

        case WM_MBUTTONUP:
            window->onMouseButtonEvent(HoloJs::MouseButtonEventType::Up, wParam, lParam, HoloJs::MouseButton::Main);
            break;

        case WM_RBUTTONDOWN:
            window->onMouseButtonEvent(HoloJs::MouseButtonEventType::Down, wParam, lParam, HoloJs::MouseButton::Second);
            break;

        case WM_LBUTTONDOWN:
            window->onMouseButtonEvent(HoloJs::MouseButtonEventType::Down, wParam, lParam, HoloJs::MouseButton::Main);
            break;

        case WM_MBUTTONDOWN:
            window->onMouseButtonEvent(
                HoloJs::MouseButtonEventType::Down, wParam, lParam, HoloJs::MouseButton::Auxiliary);
            break;

        case WM_DESTROY:
            break;

        case WM_SIZE: {
            window->onResize(LOWORD(lParam), HIWORD(lParam));
        } break;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc;
            hdc = BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
        } break;

        default:
            return DefSubclassProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

HRESULT Win32HoloJsEmbeddedView::initialize(HoloJs::IHoloJsScriptHostInternal* host)
{
    m_host = host;

    CoInitializeEx(nullptr, 0);

    UINT_PTR subclassId = 0;

    RETURN_IF_FALSE(SetWindowSubclass(m_window, SubclassWndProc, subclassId, reinterpret_cast<DWORD_PTR>(this)));

    m_isInitialized = true;

    return S_OK;
}

void CALLBACK VSyncTimerproc(HWND Arg1, UINT Arg2, UINT_PTR Arg3, DWORD Arg4)
{
    auto window = reinterpret_cast<Win32HoloJsEmbeddedView*>(Arg3);
    EXIT_IF_TRUE(window == nullptr);

    window->tick();
}

void Win32HoloJsEmbeddedView::tick()
{
    executeOneForegroundWorkItem();
    render();
}

void Win32HoloJsEmbeddedView::run()
{
    m_openglContext = std::make_unique<Win32::Win32OpenGLContext>();
    m_openglContext->setWindow(m_window);

    EXIT_IF_FAILED(m_openglContext->initialize());

    RECT size;
    GetWindowRect(m_window, &size);
    onResize(size.right - size.left, size.bottom - size.top);

    auto result = SetTimer(m_window, reinterpret_cast<UINT_PTR>(this), 16, &VSyncTimerproc);
    EXIT_IF_TRUE(result == 0);
}

void Win32HoloJsEmbeddedView::onError(HoloJs::ScriptHostErrorType errorType)
{
    switch (errorType) {
        case ScriptHostErrorType::LoadingError:
            int msgboxID = MessageBox(m_window,
                                      (LPCWSTR)L"Could not load the script from the specified location",
                                      (LPCWSTR)L"Script loading error",
                                      MB_ICONERROR | MB_OK);
            break;
    }
}

void Win32HoloJsEmbeddedView::render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    EXIT_IF_FAILED(m_windowElement->vSync());
    glFlush();
    EXIT_IF_FAILED(m_openglContext->swap());
}

HRESULT Win32HoloJsEmbeddedView::onOpenGLDeviceLost()
{
    m_windowElement->deviceLost();
    RETURN_IF_FAILED(m_openglContext->initialize());
    m_windowElement->deviceRestored();

    return S_OK;
}

void Win32HoloJsEmbeddedView::stop()
{
    KillTimer(m_window, reinterpret_cast<UINT_PTR>(this));
    m_timers.reset();
    RemoveWindowSubclass(m_window, SubclassWndProc, 0);
}

HRESULT Win32HoloJsEmbeddedView::executeApp(std::shared_ptr<HoloJs::AppModel::HoloJsApp> app)
{
    return queueForegroundWorkItem(new QueuedAppStartWorkItem(app));
}

void Win32HoloJsEmbeddedView::runApp(std::shared_ptr<HoloJs::AppModel::HoloJsApp> app)
{
    EXIT_IF_FAILED(m_host->createExecutionContext());

    m_timers = make_unique<HoloJs::Timers>();
    m_timers->setTimersImplementation(new Win32Timers(m_window));
    EXIT_IF_FAILED(m_timers->initialize());

    m_windowElement = m_host->getWindowElement();
    m_windowElement->setHeadsetAvailable(false);
    m_windowElement->resize(m_width, m_height);

    SetWindowText(m_window, app->getName().empty() ? L"HoloJs App" : app->getName().c_str());

    EXIT_IF_FAILED(m_host->executeLoadedApp(app));
}

HRESULT Win32HoloJsEmbeddedView::executeScriptImmediate(const wchar_t* script)
{
    return m_host->executeImmediate(script, nullptr);
}

HRESULT Win32HoloJsEmbeddedView::executeScript(const wchar_t* script)
{
    wchar_t* scriptCopy = new wchar_t[wcslen(script) + 1];
    memset(scriptCopy, 0, (wcslen(script) + 1) * sizeof(wchar_t));
    memcpy(scriptCopy, script, wcslen(script) * sizeof(wchar_t));
    SendMessage(m_window, EXECUTE_SCRIPT, 0, reinterpret_cast<WPARAM>(scriptCopy));

    return S_OK;
}

long Win32HoloJsEmbeddedView::executeOnViewThread(HoloJs::IForegroundWorkItem* workItem)
{
    return queueForegroundWorkItem(workItem);
}

long Win32HoloJsEmbeddedView::executeInBackground(HoloJs::IBackgroundWorkItem* workItem)
{
    create_task([workItem]() -> long {
        auto result = workItem->execute();
        delete workItem;

        return result;
    });

    return S_OK;
}