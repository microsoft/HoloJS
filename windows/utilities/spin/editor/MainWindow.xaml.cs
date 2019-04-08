using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Microsoft.Win32;
using System.Windows.Interop;
using HoloJs.Spin;
using ZXing;

namespace assembler_studio
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        HoloJs.DotNet.HoloJsScriptHost EmbeddedHoloJs = null;
        HoloJs.DotNet.ScriptRenderHost RenderHost;
        System.Diagnostics.Process ExternalViewer;

        HelpWindow ActiveHelpWindow;

        ScriptApp _ActiveApp;
        AppServer TestServer;
        ScriptApp ActiveApp
        {
            get
            {
                return _ActiveApp;
            }
            set
            {
                _ActiveApp = value;

                if (TestServer != null)
                {
                    TestServer.Stop();
                    TestServer = null;
                }

                if (value == null)
                {
                    ScriptsList.ItemsSource = null;
                }
                else
                {
                    ScriptsList.ItemsSource = value.Scripts;
                    ResourcesList.ItemsSource = value.Resources;

                    TestServer = new AppServer(ActiveApp.AppPath);
                    TestServer.RunAsync();
                }
                
                AppLoaded = value != null;
            }
        }

        public static readonly DependencyProperty AppLoadedProperty =
  DependencyProperty.Register("AppLoaded", typeof(bool), typeof(MainWindow), new UIPropertyMetadata(false));

        public bool AppLoaded
        {
            get { return (bool)this.GetValue(AppLoadedProperty); }
            set { this.SetValue(AppLoadedProperty, value); }
        }

        public static readonly DependencyProperty AppPreviewRunningProperty =
  DependencyProperty.Register("AppPreviewRunning", typeof(bool), typeof(MainWindow), new UIPropertyMetadata(false));

        public bool AppPreviewRunning
        {
            get { return (bool)this.GetValue(AppPreviewRunningProperty); }
            set { this.SetValue(AppPreviewRunningProperty, value); }
        }

        public static string LocalPath = System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetEntryAssembly().Location);

        public MainWindow()
        {
            InitializeComponent();
            RenderHost = new HoloJs.DotNet.ScriptRenderHost();
            RenderCanvasHost.Child = RenderHost;


            if (!string.IsNullOrEmpty((App.Current as App).AppToOpen))
            {
                ActiveApp = ScriptApp.Open((App.Current as App).AppToOpen);
            }
        }

        private void OpenAppMenu_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog openDialog = new OpenFileDialog();
            openDialog.Filter = "XR script (*.xrs)| *.xrs;";

            if (openDialog.ShowDialog() == true)
            {
                ActiveApp = ScriptApp.Open(openDialog.FileName);
                if (ActiveApp == null)
                {
                    MessageBox.Show("Failed to open the script app");
                }

                if (EmbeddedHoloJs != null)
                {
                    EmbeddedHoloJs.Dispose();
                    EmbeddedHoloJs = null;
                }
            }
        }

        private void StopApp_Click(object sender, RoutedEventArgs e)
        {
            StopPreview();
        }

        private void RunAppMenu_Click(object sender, RoutedEventArgs e)
        {
            if (EmbeddedHoloJs != null)
            {
                EmbeddedHoloJs.Dispose();
            }

            EmbeddedHoloJs = new HoloJs.DotNet.HoloJsScriptHost();
            EmbeddedHoloJs.SetViewWindow(RenderHost.Handle);
            HoloJs.DotNet.ViewConfiguration viewConfig = new HoloJs.DotNet.ViewConfiguration();
            viewConfig.mode = HoloJs.DotNet.ViewMode.FlatEmbedded;
            EmbeddedHoloJs.Initialize(viewConfig);
            EmbeddedHoloJs.StartUri(ActiveApp.AppPath);

            AppPreviewRunning = true;
        }

        private void RunAppVR_Click(object sender, RoutedEventArgs e)
        {
            var viewerProcess = System.IO.Path.Combine(LocalPath, "spin.exe");
            ExternalViewer = System.Diagnostics.Process.Start(viewerProcess, string.Format("--uri \"{0}\"", ActiveApp.AppPath));
        }

        private void RunOnHololens_Click(object sender, RoutedEventArgs e)
        {
            var url = TestServer.GetLaunchUrl() + ActiveApp.XsrFileName;
            var hololensConnect = new QrConnect();
            hololensConnect.SetUrl(url);
            hololensConnect.ShowDialog();
        }

        private void RunAppBrowser_Click(object sender, RoutedEventArgs e)
        {
            ExternalViewer = System.Diagnostics.Process.Start(TestServer.GetLaunchUrl() + ActiveApp.HtmlAppName);
        }

        private void ScriptsList_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            var selectedScript = ScriptsList.SelectedValue as string;
            var scriptPath = ActiveApp.GetScriptPath(selectedScript);
            System.Diagnostics.Process.Start(scriptPath);
        }

        private void RemoveScriptButton_Click(object sender, RoutedEventArgs e)
        {
            var selectedScript = ScriptsList.SelectedValue as string;
            ActiveApp.RemoveScript(selectedScript);
            ScriptsList.ItemsSource = null;
            ScriptsList.ItemsSource = ActiveApp.Scripts;
        }

        private void AddResourceButton_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog openDialog = new OpenFileDialog();
            openDialog.Filter = "All files (*.*)| *.*;";

            if (openDialog.ShowDialog() == true)
            {
                var fullFilePath = openDialog.FileName;

                ActiveApp.AddResourceFile(fullFilePath);
                ResourcesList.ItemsSource = null;
                ResourcesList.ItemsSource = ActiveApp.Resources;
            }
        }

        private void RemoveResourceButton_Click(object sender, RoutedEventArgs e)
        {
            var selectedResource = ResourcesList.SelectedValue as string;
            ActiveApp.RemoveResource(selectedResource);
            ResourcesList.ItemsSource = null;
            ResourcesList.ItemsSource = ActiveApp.Resources;
        }

        private void AddScriptButton_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog openDialog = new OpenFileDialog();
            openDialog.Filter = "JavaScript files (*.js)| *.js;";

            if (openDialog.ShowDialog() == true)
            {
                var scriptFullPath = openDialog.FileName;

                ActiveApp.AddScriptFile(scriptFullPath);
                ScriptsList.ItemsSource = null;
                ScriptsList.ItemsSource = ActiveApp.Scripts;
            }
        }

        private void StopPreview()
        {
            if (EmbeddedHoloJs != null)
            {
                EmbeddedHoloJs.Dispose();
                EmbeddedHoloJs = null;
            }

            AppPreviewRunning = false;
        }

        private void NewAppButton_Click(object sender, RoutedEventArgs e)
        {
            ScriptsList.ItemsSource = null;
            ActiveApp = null;

            var newAppWizzard = new NewAppWizzard();
            newAppWizzard.Owner = this;
            if (newAppWizzard.ShowDialog() == true)
            {
                ActiveApp = newAppWizzard.CreatedApp;
            }
        }

        private void MoveScriptUpButton_Click(object sender, RoutedEventArgs e)
        {
            var selectedScript = ScriptsList.SelectedValue as string;
            var index = ActiveApp.ShiftScriptApp(selectedScript);
            ScriptsList.ItemsSource = null;
            ScriptsList.ItemsSource = ActiveApp.Scripts;
            ScriptsList.SelectedIndex = index - 1;
            ActiveApp.Save();
        }

        private void MoveScriptDownButton_Click(object sender, RoutedEventArgs e)
        {
            var selectedScript = ScriptsList.SelectedValue as string;
            var index = ActiveApp.ShiftScriptDown(selectedScript);
            ScriptsList.ItemsSource = null;
            ScriptsList.ItemsSource = ActiveApp.Scripts;
            ScriptsList.SelectedIndex = index + 1;
            ActiveApp.Save();
        }

        private void CreateVsCodeDebugConfiguration(string basePath, string scriptAppPath)
        {
            var configDirectory = System.IO.Path.Combine(basePath, ".vscode");
            if (!System.IO.Directory.Exists(configDirectory))
            {
                System.IO.Directory.CreateDirectory(configDirectory);
            }

            var configFile = System.IO.Path.Combine(configDirectory, "launch.json");

            var launchTemplate = System.IO.File.ReadAllText(System.IO.Path.Combine(LocalPath, "launch.json"));
            var spinPath = System.IO.Path.Combine(LocalPath, "spin.exe");

            var launchContent = launchTemplate.Replace("<<app_path_here>>", scriptAppPath.Replace("\\", "\\\\"));
            System.IO.File.WriteAllText(configFile, launchContent, Encoding.UTF8);
        }

        private void OpenInVSCodeButton_Click(object sender, RoutedEventArgs e)
        {
            var appFolder = ActiveApp.AppBasePath;

            CreateVsCodeDebugConfiguration(ActiveApp.AppBasePath, ActiveApp.AppPath);

            try
            {
                System.Diagnostics.Process.Start("vscode://file/" + appFolder);
            }
            catch(Exception)
            {
                MessageBox.Show("Could not launch Visual Studio Code. Is it installed on this computer?");
            }
        }

        private void CreatePackage_Click(object sender, RoutedEventArgs e)
        {
            var saveDialog = new SaveFileDialog();
            saveDialog.Filter = "XR Script Package (*.xrsx)| *.xrsx;";
            if (saveDialog.ShowDialog() == true)
            {
                ActiveApp.CreatePackage(saveDialog.FileName);
            }
        }

        private void SaveTitleButton_Click(object sender, RoutedEventArgs e)
        {
            ActiveApp.Save();
        }

        private void Help_Click(object sender, RoutedEventArgs e)
        {
            System.Diagnostics.Process.Start("http://holojs.azurewebsites.net/help/spin-make.html");
        }

        private void _this_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            if (EmbeddedHoloJs != null)
            {
                EmbeddedHoloJs.Dispose();
            }
        }
    }
}
