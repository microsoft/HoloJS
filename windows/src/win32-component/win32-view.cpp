#include "stdafx.h"
#include "win32-spatial-input.h"
#include "win32-timers-implementation.h"
#include "win32-view.h"
#include <d3d11_1.h>
#include <directxcolors.h>
#include <memory>

using namespace HoloJs::Win32;
using namespace HoloJs::AppModel;
using namespace std;
using namespace concurrency;

static unsigned int g_viewThreadId{static_cast<unsigned int>(-1)};

static ATOM g_windowRegistrationAtom = 0;

#define STOP_EXECUTION (WM_USER + 6)
#define EXECUTE_SCRIPT (WM_USER + 1)
#define RESIZE_NOTIFICATION (WM_USER + 3)

#define INITIAL_WIDTH 640
#define INITIAL_HEIGHT 320

Win32HoloJsView::Win32HoloJsView(HoloJs::ViewConfiguration viewConfiguration) : m_viewConfiguration(viewConfiguration)
{
}

Win32HoloJsView::~Win32HoloJsView()
{
    stop();
    UnregisterClassW(m_className, nullptr);
    g_windowRegistrationAtom = 0;
}

// Step 4: the Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
        case WM_NCCREATE: {
            LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }

        case WM_CLOSE:
            PostThreadMessage(g_viewThreadId, STOP_EXECUTION, 0, 0);
            break;

        case WM_KEYDOWN: {
            auto window = reinterpret_cast<Win32HoloJsView*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
            window->onKeyDown(wParam, lParam);
        }

            return DefWindowProc(hwnd, msg, wParam, lParam);

        case WM_KEYUP: {
            auto window = reinterpret_cast<Win32HoloJsView*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
            window->onKeyUp(wParam, lParam);
        }

            return DefWindowProc(hwnd, msg, wParam, lParam);

        case WM_MOUSEWHEEL: {
            auto window = reinterpret_cast<Win32HoloJsView*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
            window->onMouseWheelEvent(HoloJs::MouseButtonEventType::Wheel, wParam, lParam);
        } break;

        case WM_MOUSEMOVE: {
            SetFocus(hwnd);
            auto window = reinterpret_cast<Win32HoloJsView*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
            window->onMouseEvent(HoloJs::MouseButtonEventType::Move, wParam, lParam);
        } break;

        case WM_RBUTTONDBLCLK: {
            auto window = reinterpret_cast<Win32HoloJsView*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
            window->onMouseButtonEvent(
                HoloJs::MouseButtonEventType::DblClick, wParam, lParam, HoloJs::MouseButton::Second);
        } break;

        case WM_MBUTTONDBLCLK: {
            auto window = reinterpret_cast<Win32HoloJsView*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
            window->onMouseButtonEvent(
                HoloJs::MouseButtonEventType::DblClick, wParam, lParam, HoloJs::MouseButton::Auxiliary);
        } break;

        case WM_LBUTTONDBLCLK: {
            auto window = reinterpret_cast<Win32HoloJsView*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
            window->onMouseButtonEvent(
                HoloJs::MouseButtonEventType::DblClick, wParam, lParam, HoloJs::MouseButton::Main);
        } break;

        case WM_RBUTTONUP: {
            auto window = reinterpret_cast<Win32HoloJsView*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
            window->onMouseButtonEvent(HoloJs::MouseButtonEventType::Up, wParam, lParam, HoloJs::MouseButton::Second);
        } break;

        case WM_LBUTTONUP: {
            auto window = reinterpret_cast<Win32HoloJsView*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
            window->onMouseButtonEvent(
                HoloJs::MouseButtonEventType::Up, wParam, lParam, HoloJs::MouseButton::Auxiliary);
        } break;

        case WM_MBUTTONUP: {
            auto window = reinterpret_cast<Win32HoloJsView*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
            window->onMouseButtonEvent(HoloJs::MouseButtonEventType::Up, wParam, lParam, HoloJs::MouseButton::Main);
        } break;

        case WM_RBUTTONDOWN: {
            auto window = reinterpret_cast<Win32HoloJsView*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
            window->onMouseButtonEvent(HoloJs::MouseButtonEventType::Down, wParam, lParam, HoloJs::MouseButton::Second);
        } break;

        case WM_LBUTTONDOWN: {
            auto window = reinterpret_cast<Win32HoloJsView*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
            window->onMouseButtonEvent(HoloJs::MouseButtonEventType::Down, wParam, lParam, HoloJs::MouseButton::Main);
        } break;

        case WM_MBUTTONDOWN: {
            auto window = reinterpret_cast<Win32HoloJsView*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
            window->onMouseButtonEvent(
                HoloJs::MouseButtonEventType::Down, wParam, lParam, HoloJs::MouseButton::Auxiliary);
        } break;

        case WM_DESTROY:
            break;

        case WM_SIZE: {
            Win32HoloJsView* window = reinterpret_cast<Win32HoloJsView*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
            window->onResize(LOWORD(lParam), HIWORD(lParam));
        } break;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc;
            hdc = BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
        } break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

HRESULT Win32HoloJsView::createWindow()
{
    WNDCLASSEX wc;

    // Step 1: Registering the Window Class
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = nullptr;
    wc.hIcon = (m_icon == nullptr ? LoadIcon(nullptr, IDI_APPLICATION) : m_icon);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = m_className;
    wc.hIconSm = nullptr;

    if (g_windowRegistrationAtom == 0) {
        g_windowRegistrationAtom = RegisterClassEx(&wc);
    }

    if (g_windowRegistrationAtom == 0) {
        MessageBox(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return E_FAIL;
    }

    PCWSTR windowTitle = m_title.empty() ? L"HoloJs" : m_title.c_str();

    // Step 2: Creating the Window
    m_window = CreateWindowEx(WS_EX_CLIENTEDGE,
                              m_className,
                              windowTitle,
                              WS_OVERLAPPEDWINDOW,
                              CW_USEDEFAULT,
                              CW_USEDEFAULT,
                              INITIAL_WIDTH,
                              INITIAL_HEIGHT,
                              NULL,
                              NULL,
                              nullptr,
                              this);

    if (m_window == NULL) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return E_FAIL;
    }

    if (MixedReality::MixedRealityContext::headsetAvailable()) {
        m_mixedRealityContext = make_shared<HoloJs::Win32::Win32MixedRealityContext>();
        m_mixedRealityContext->setWindow(m_window);
        m_mixedRealityContext->initializeHolographicSpace();
    }

    ShowWindow(m_window, SW_SHOW);
    UpdateWindow(m_window);

    return S_OK;
}

HRESULT Win32HoloJsView::initialize(HoloJs::IHoloJsScriptHostInternal* host)
{
    m_host = host;

    CoInitializeEx(nullptr, 0);

    RETURN_IF_FAILED(createWindow());

    g_viewThreadId = GetCurrentThreadId();

    m_isInitialized = true;

    return S_OK;
}

long Win32HoloJsView::releaseScriptResources()
{
    m_windowElement = nullptr;
    m_timers.reset();
    m_spatialInput.reset();

    if (m_mixedRealityContext) {
        m_mixedRealityContext->releaseScriptResources();
    }

    m_activeApp.reset();

    m_openglContext.reset();

    return S_OK;
}

long Win32HoloJsView::initializeScriptResources()
{
    m_timers = make_unique<HoloJs::Timers>();
    m_timers->setTimersImplementation(new Win32Timers(m_window));
    RETURN_IF_FAILED(m_timers->initialize());

    m_windowElement = m_host->getWindowElement();

    m_openglContext = std::make_unique<Win32::Win32OpenGLContext>();
    m_openglContext->setWindow(m_window);

    if (m_mixedRealityContext) {
        m_mixedRealityContext->setScriptWindow(m_windowElement);
        m_windowElement->setHeadsetAvailable(MixedReality::MixedRealityContext::headsetAvailable());

        m_mixedRealityContext->initializeScriptHost();
        m_spatialInput = make_unique<HoloJs::Win32::Win32SpatialInput>();
        m_spatialInput->setWindow(m_window);
        m_spatialInput->setFrameOfReference(m_mixedRealityContext->getStationaryFrameOfReference());
        m_spatialInput->setScriptWindow(m_windowElement);
        m_spatialInput->initialize();

        m_openglContext->setMixedRealityContext(m_mixedRealityContext);
    }

    RETURN_IF_FAILED(m_openglContext->initialize());

    if (m_mixedRealityContext) {
        onResize(static_cast<int>(m_mixedRealityContext->getWidth()),
                 static_cast<int>(m_mixedRealityContext->getHeight()));
    } else {
        onResize(m_width, m_height);
    }

    return S_OK;
}

void Win32HoloJsView::run()
{
    MSG msg;

    while (!m_closeRequested) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == STOP_EXECUTION) {
                m_closeRequested = true;
            } else if (msg.message == EXECUTE_SCRIPT) {
                m_host->executeImmediate(reinterpret_cast<wchar_t*>(msg.lParam), nullptr);
                delete reinterpret_cast<wchar_t*>(msg.lParam);
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        const auto isViewReady =
            !m_mixedRealityContext || (m_mixedRealityContext && m_mixedRealityContext->isCameraAvailable());

        if (isViewReady) {
            executeOneForegroundWorkItem();

            if (m_activeApp) {
                render();
            }
        }
    }

    // Drain foreground workitems queue
    {
        std::lock_guard<std::recursive_mutex> guard(m_foregroundWorkItemQueue);
        while (m_foregroundWorkItems.size() > 0) {
            executeOneForegroundWorkItem();
        }
    }

    m_timers.reset();

    DestroyWindow(m_window);
    m_window = nullptr;

    g_viewThreadId = -1;
}

void Win32HoloJsView::onError(HoloJs::ScriptHostErrorType errorType)
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

void Win32HoloJsView::render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    Windows::Graphics::Holographic::HolographicFrame ^ frame;
    if (m_mixedRealityContext) {
        // Back buffers can change from frame to frame. Validate each buffer, and recreate
        // resource views and depth buffers as needed.
        frame = m_mixedRealityContext->createHolographicFrame();

        if (frame) {
            EXIT_IF_FAILED(m_windowElement->vSync());
            glFlush();
            m_mixedRealityContext->render(frame, m_openglContext->getWebGLTexture());
            m_mixedRealityContext->present(frame);
        }
    } else {
        EXIT_IF_FAILED(m_windowElement->vSync());
        glFlush();
        EXIT_IF_FAILED(m_openglContext->swap());
    }
}

HRESULT Win32HoloJsView::onOpenGLDeviceLost()
{
    m_windowElement->deviceLost();
    RETURN_IF_FAILED(m_openglContext->initialize());
    m_windowElement->deviceRestored();

    return S_OK;
}

void Win32HoloJsView::stop() { SendMessage(m_window, WM_CLOSE, 0, 0); }

HRESULT Win32HoloJsView::executeApp(std::shared_ptr<HoloJsApp> app)
{
    return queueForegroundWorkItem(new QueuedAppStartWorkItem(app));
}

void Win32HoloJsView::runApp(shared_ptr<HoloJs::AppModel::HoloJsApp> app)
{
    if (m_activeApp) {
        releaseScriptResources();
    }

    m_activeApp = app;

    wstring windowTitle;
    if (!app->getName().empty()) {
        windowTitle = app->getName();
    } else if (!m_title.empty()) {
        windowTitle = m_title;
    }

    SetWindowText(m_window, windowTitle.c_str());

    EXIT_IF_FAILED(m_host->createExecutionContext());
    EXIT_IF_FAILED(initializeScriptResources());

    EXIT_IF_FAILED(m_host->executeLoadedApp(app));
}

HRESULT Win32HoloJsView::executeScript(const wchar_t* script)
{
    wchar_t* scriptCopy = new wchar_t[wcslen(script) + 1];
    memset(scriptCopy, 0, (wcslen(script) + 1) * sizeof(wchar_t));
    memcpy(scriptCopy, script, wcslen(script) * sizeof(wchar_t));
    PostThreadMessageW(g_viewThreadId, EXECUTE_SCRIPT, 0, reinterpret_cast<WPARAM>(scriptCopy));

    return S_OK;
}

long Win32HoloJsView::executeOnViewThread(HoloJs::IForegroundWorkItem* workItem)
{
    return queueForegroundWorkItem(workItem);
}

long Win32HoloJsView::executeInBackground(HoloJs::IBackgroundWorkItem* workItem)
{
    RETURN_IF_TRUE(m_closeRequested);

    create_task([workItem]() -> long {
        auto result = workItem->execute();
        delete workItem;

        return result;
    });

    return S_OK;
}