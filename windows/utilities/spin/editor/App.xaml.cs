using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;

namespace assembler_studio
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        public string AppToOpen = null;

        protected override void OnStartup(StartupEventArgs e)
        {
            if (e.Args.Length > 0)
            {
                if (e.Args[0] == "/e")
                {
                    if (e.Args.Length > 1)
                    {
                        AppToOpen = e.Args[1];
                    }
                } else if (e.Args[0] == "/r" && e.Args.Length > 1)
                {
                    System.Diagnostics.Process.Start("spin.exe", string.Format("--uri \"{0}\"", e.Args[1]));
                    base.Shutdown();
                } 
                else if (System.IO.File.Exists(e.Args[0]))
                {
                    System.Diagnostics.Process.Start("spin.exe", string.Format("--uri \"{0}\"", e.Args[0]));
                    base.Shutdown();
                }
            }
            base.OnStartup(e);
        }
    }
}
