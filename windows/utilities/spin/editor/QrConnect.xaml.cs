using System;
using System.Collections.Generic;
using System.Diagnostics;
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
using ZXing;

namespace HoloJs.Spin
{
    /// <summary>
    /// Interaction logic for QrConnect.xaml
    /// </summary>
    public partial class QrConnect : Window
    {
        public QrConnect()
        {
            InitializeComponent();
        }

        public void SetUrl(string url)
        {
            var bitmapRenderer = new ZXing.Rendering.WriteableBitmapRenderer();
            bitmapRenderer.Background = System.Windows.Media.Colors.White;
            bitmapRenderer.Foreground = System.Windows.Media.Colors.Black;

            ZXing.BarcodeWriterPixelData qrWriter = new ZXing.BarcodeWriterPixelData()
            {
                Format = BarcodeFormat.QR_CODE,
                Options = new ZXing.Common.EncodingOptions()
                {
                    Height = 1024,
                    Width = 1024
                }
            };

            var bitmap = bitmapRenderer.Render(qrWriter.Encode(url), BarcodeFormat.QR_CODE, url);

            QrImage.Source = bitmap;

            UrlText.Text = "spin.exe --uri " + url;
        }

        private void CopyToClipboard_Click(object sender, RoutedEventArgs e)
        {
            Clipboard.SetText(UrlText.Text);
        }

        private void EmbeddedUrl_RequestNavigate(object sender, System.Windows.Navigation.RequestNavigateEventArgs e)
        {
            Process.Start(new ProcessStartInfo(e.Uri.AbsoluteUri));
            e.Handled = true;
        }
    }
}
