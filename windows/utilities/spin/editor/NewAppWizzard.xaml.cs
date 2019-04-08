using Microsoft.Win32;
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
using System.Windows.Shapes;

namespace HoloJs.Spin
{
    /// <summary>
    /// Interaction logic for NewAppWizzard.xaml
    /// </summary>
    public partial class NewAppWizzard : Window
    {
        public ScriptApp CreatedApp;

        public NewAppWizzard()
        {
            InitializeComponent();
            DestinationPath.Text = System.IO.Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.DesktopDirectory), "app.xrs");
        }

        private void CreateApp_Click(object sender, RoutedEventArgs e)
        {
            if(IsThreeJs.IsChecked == true)
            {
                var appGenerator = new ThreeJsAppGenerator(DestinationPath.Text);
                CreatedApp = appGenerator.CreateThreeJsApp();
            }
            else if (IsBabylonJs.IsChecked == true)
            {
                var appGenerator = new BabylonJsAppGenerator(DestinationPath.Text);
                CreatedApp = appGenerator.CreateThreeJsApp();
            }
            else if (IsBlank.IsChecked == true)
            {
                var appJsonPath = DestinationPath.Text;
                CreatedApp = new ScriptApp();
                CreatedApp.AppPath = appJsonPath;
                CreatedApp.Save(appJsonPath);
            }

            CreatedApp.name = AppName.Text;

            this.DialogResult = true;
            this.Close();
        }

        private void BrowseLocation_Click(object sender, RoutedEventArgs e)
        {
            var saveDialog = new SaveFileDialog();
            saveDialog.Filter = "XR Script (*.xrs)| *.xrs;";
            if (saveDialog.ShowDialog() == true)
            {
                DestinationPath.Text = saveDialog.FileName;
            }
        }

        private void AppName_TextChanged(object sender, TextChangedEventArgs e)
        {
            CreateAppButton.IsEnabled = !string.IsNullOrEmpty(AppName.Text);
        }
    }
}
