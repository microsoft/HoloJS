using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace HoloJs.DotNet
{

    static class HoloJsScriptHostInterop
    {
        [DllImport("libholojs-native.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr holoJsScriptHost_create();

        [DllImport("libholojs-native.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        public static extern void holoJsScriptHost_delete(IntPtr scriptHost);

        [DllImport("libholojs-native.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        public static extern void holoJsScriptHost_setViewWindow(IntPtr scriptHost, IntPtr window);

        [DllImport("libholojs-native.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        public static extern int holoJsScriptHost_initialize(IntPtr scriptHost, ref ViewConfiguration viewConfig);

        [DllImport("libholojs-native.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        public static extern int holoJsScriptHost_start(IntPtr scriptHost, string script);

        [DllImport("libholojs-native.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        public static extern int holoJsScriptHost_startUri(IntPtr scriptHost, string appUri);

        [DllImport("libholojs-native.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        public static extern int holoJsScriptHost_execute(IntPtr scriptHost, string script);

        [DllImport("libholojs-native.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        public static extern int holoJsScriptHost_executeUri(IntPtr scriptHost, string scriptUri);

        [DllImport("libholojs-native.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        public static extern int holoJsScriptHost_executeImmediate(IntPtr scriptHost, string script);

        [DllImport("libholojs-native.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        public static extern int holoJsScriptHost_stopExecution(IntPtr scriptHost);

        [DllImport("libholojs-native.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        public static extern void holoJsScriptHost_enableDebugger(IntPtr scriptHost);
    }

    public enum ViewMode : int
    {
        None = 0,
	    Default,
	    Flat,
	    FlatEmbedded,
	    VR
    };

    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct ViewConfiguration
    {
        public ViewMode mode;
        public bool enableVoiceCommands;
        public bool enableQrCodeNavigation;
    }

    public class HoloJsScriptHost : IDisposable
    {
        private IntPtr NativeHoloJsScriptHost;
        public HoloJsScriptHost()
        {
            NativeHoloJsScriptHost = HoloJsScriptHostInterop.holoJsScriptHost_create();
        }

        public void Dispose()
        {
            HoloJsScriptHostInterop.holoJsScriptHost_delete(NativeHoloJsScriptHost);
        }

        public void Initialize(ViewConfiguration viewConfig)
        {
            if (HoloJsScriptHostInterop.holoJsScriptHost_initialize(NativeHoloJsScriptHost, ref viewConfig) < 0)
            {
                throw new Exception("failed to initialize");
            }
        }

        public void SetViewWindow(IntPtr windowHandle)
        {
            HoloJsScriptHostInterop.holoJsScriptHost_setViewWindow(NativeHoloJsScriptHost, windowHandle);
        }

        public void StartUri(string appUri)
        {
            if (HoloJsScriptHostInterop.holoJsScriptHost_startUri(NativeHoloJsScriptHost, appUri) < 0)
            {
                throw new Exception("failed to start");
            }
        }


        public void Start(string script)
        {
            if (HoloJsScriptHostInterop.holoJsScriptHost_start(NativeHoloJsScriptHost, script) < 0)
            {
                throw new Exception("failed to start");
            }
        }

        public void Execute(string script)
        {
            if (HoloJsScriptHostInterop.holoJsScriptHost_execute(NativeHoloJsScriptHost, script) < 0)
            {
                throw new Exception("failed to start");
            }
        }
        public void ExecuteUri(string scriptUri)
        {
            if (HoloJsScriptHostInterop.holoJsScriptHost_executeUri(NativeHoloJsScriptHost, scriptUri) < 0)
            {
                throw new Exception("failed to start");
            }
        }
        public void ExecuteImmediate(string script)
        {
            if (HoloJsScriptHostInterop.holoJsScriptHost_executeImmediate(NativeHoloJsScriptHost, script) < 0)
            {
                throw new Exception("failed to start");
            }
        }

        public void StopExecution()
        {
            if (HoloJsScriptHostInterop.holoJsScriptHost_stopExecution(NativeHoloJsScriptHost) < 0)
            {
                throw new Exception("failed to start");
            }
        }

        public void EnableDebugger()
        {
            HoloJsScriptHostInterop.holoJsScriptHost_enableDebugger(NativeHoloJsScriptHost);
        }
    }
}
