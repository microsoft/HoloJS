using CommandLine;
using HoloJs.Spin;
using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;

namespace spin
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        AppServer DebugHttpServer;
        QrConnect QRConnectWindow;

        protected override void OnStartup(StartupEventArgs e)
        {
            CommandLine.Parser.Default.ParseArguments<
                RunAppOptions, ServeAppOptions, AppCreationOptions, AppPublishingOptions, AppEditingOptions, OpenAppOptions>(e.Args)
                .WithParsed<RunAppOptions>(opts => RunHoloJsApp(opts))
                .WithParsed<ServeAppOptions>(opts => ServeHoloJsApp(opts))
                .WithParsed<AppCreationOptions>(opts => CreateNewHoloJsApp(opts))
                .WithParsed<AppPublishingOptions>(opts => PublishHoloJsApp(opts))
                .WithParsed<AppEditingOptions>(opts => EditHoloJsApp(opts))
                .WithParsed<OpenAppOptions>(opts => OpenHoloJsApp(opts));

            this.Shutdown();
        }

        private void RunHoloJsApp(RunAppOptions options)
        {
            HoloJs.DotNet.HoloJsScriptHost holoJsHost = new HoloJs.DotNet.HoloJsScriptHost();
            HoloJs.DotNet.ViewConfiguration viewConfiguration = new HoloJs.DotNet.ViewConfiguration();
            viewConfiguration.enableVoiceCommands = true;
            if (options.Debug)
            {
                holoJsHost.EnableDebugger();
            }

            holoJsHost.Initialize(viewConfiguration);

            if (!string.IsNullOrEmpty(options.Path))
            {
                holoJsHost.StartUri(System.IO.Path.GetFullPath(options.Path));
            } else
            {
                holoJsHost.StartUri(options.Uri);
            }
        }

        private void CreateNewHoloJsApp(AppCreationOptions opts)
        {
            if (System.IO.Directory.Exists(opts.Path) && !opts.Overwrite)
            {
                Console.WriteLine("The directory already exists. Use --overwrite to force creating a new app in it.");
                return;
            }

            var generator = new ProjectGenerator(opts.Path);
            generator.Generate();

            RunVsCode(System.IO.Path.GetFullPath(opts.Path));
        }

        private void ServeHoloJsApp(ServeAppOptions opts)
        {
            XrsPackage xrsApp = null;
            try
            {
                xrsApp = XrsPackage.Open(opts.Path);
            }
            catch (Exception)
            {
                Console.WriteLine("Cannot open the specified HoloJs app. Make sure the source points to a valid Spin app.");
                return;
            }

            DebugHttpServer = new AppServer(opts.Path);

            var appUrl = DebugHttpServer.GetLaunchUrl() + xrsApp.XsrFileName;

            Console.WriteLine("HTTP server running for " + appUrl + ". Ctrl - C to stop.");
            Console.CancelKeyPress += Console_CancelKeyPress;

            if (opts.ShowQr)
            {
                DebugHttpServer.RunAsync();

                QRConnectWindow = new QrConnect();
                QRConnectWindow.SetUrl(appUrl);
                QRConnectWindow.ShowDialog();
            }
            else
            {
                DebugHttpServer.Run();
            }

            Console.WriteLine("HTTP server exited");
        }

        private void PublishHoloJsApp(AppPublishingOptions opts)
        {
            XrsPackage xrsApp = null;
            try
            {
                xrsApp = XrsPackage.Open(opts.SourcePath);
            }
            catch(Exception)
            {
                Console.WriteLine("Cannot open the specified HoloJs app. Make sure the source points to a valid Spin app.");
                return;
            }

            if (System.IO.File.Exists(opts.DestinationPath) && !opts.Overwrite)
            {
                Console.WriteLine("a XRSX file with this name already exists. Use --overwrite to overwrite it.");
                return;
            }

            Console.WriteLine("Writing XRSX file ...");

            xrsApp.CreatePackage(opts.DestinationPath);

            Console.WriteLine("Done!");
        }

        private void EditHoloJsApp(AppEditingOptions opts)
        {
            XrsPackage xrsApp = null;
            try
            {
                xrsApp = XrsPackage.Open(opts.Path);
            }
            catch (Exception)
            {
                Console.WriteLine("Cannot open the specified HoloJs app. Make sure the path to a valid Spin app.");
                return;
            }

            if (!string.IsNullOrEmpty(opts.Name))
            {
                xrsApp.name = opts.Name;
            }

            if (!string.IsNullOrEmpty(opts.AddScriptPath))
            {
                xrsApp.AddScriptFile(opts.AddScriptPath);
            }

            if (!string.IsNullOrEmpty(opts.AddResourcePath))
            {
                xrsApp.AddResourceFile(opts.AddResourcePath);
            }

            if (!string.IsNullOrEmpty(opts.RemoveScriptPath))
            {
                xrsApp.RemoveScript(opts.RemoveScriptPath);
            }

            if (!string.IsNullOrEmpty(opts.RemoveResourcePath))
            {
                xrsApp.RemoveResource(opts.RemoveResourcePath);
            }

            xrsApp.Save();
        }

        private void OpenHoloJsApp(OpenAppOptions opts)
        {
            XrsPackage xrsApp = null;
            try
            {
                xrsApp = XrsPackage.Open(opts.Path);
            }
            catch (Exception)
            {
                Console.WriteLine("Cannot open the specified HoloJs app. Make sure the path to a valid Spin app.");
                return;
            }

            var appDirectory = System.IO.Path.GetDirectoryName(System.IO.Path.GetFullPath(opts.Path));
            RunVsCode(appDirectory);
        }

        private void Console_CancelKeyPress(object sender, ConsoleCancelEventArgs e)
        {
            if (DebugHttpServer != null)
            {
                DebugHttpServer.Stop();
            }

            if (QRConnectWindow != null)
            {
                QRConnectWindow.Close();
            }

            e.Cancel = true;
        }

        private void ShowHelp(IEnumerable<Error> errors)
        {
            this.Shutdown();
        }

        private void RunVsCode(string folder, string file = null)
        {
            var startInfo = new System.Diagnostics.ProcessStartInfo("vscode://file/" + folder);
            System.Diagnostics.Process.Start(startInfo);
        }
    }
}
