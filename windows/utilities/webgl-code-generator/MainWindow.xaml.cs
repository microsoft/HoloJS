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
using System.IO;

namespace webgl_code_generator
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        /*JsValueRef m_createContext = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK staticCreateContext(JsValueRef callee,
                                                    bool isConstructCall,
                                                    JsValueRef* arguments,
                                                    unsigned short argumentCount,
                                                    PVOID callbackData);
    JsValueRef createContext(JsValueRef* arguments, unsigned short argumentCount);*/

        private void GenerateHeader_Click(object sender, RoutedEventArgs e)
        {
            GeneratedCode.Text = "";
            using (var file = File.OpenText("webgl-functions.txt"))
            {
                while (!file.EndOfStream)
                {
                    var functionName = file.ReadLine();
                    var FunctionName = functionName.Remove(0, 1);
                    FunctionName = FunctionName.Insert(0, functionName.Substring(0, 1).ToUpperInvariant());
                    GeneratedCode.Text += string.Format("JsValueRef m_{0} = JS_INVALID_REFERENCE;\r\n", functionName);
                    GeneratedCode.Text += string.Format("static JsValueRef CHAKRA_CALLBACK static{0}(JsValueRef callee,bool isConstructCall,JsValueRef * arguments,unsigned short argumentCount,PVOID callbackData)\r\n", FunctionName);
                    GeneratedCode.Text += string.Format("{{ return reinterpret_cast<WebGLProjections*>(callbackData)->{0}(arguments, argumentCount); }}\r\n", functionName);
                    GeneratedCode.Text += string.Format("JsValueRef {0}(JsValueRef* arguments, unsigned short argumentCount);\r\n", functionName);

                    GeneratedCode.Text += string.Format("\r\n", functionName);
                }
            }

            Clipboard.SetText(GeneratedCode.Text);
        }

        private void GenerateProjections_Click(object sender, RoutedEventArgs e)
        {
            GeneratedCode.Text = "";
            using (var file = File.OpenText("webgl-functions.txt"))
            {
                while (!file.EndOfStream)
                {
                    var functionName = file.ReadLine();
                    var FunctionName = functionName.Remove(0, 1);
                    FunctionName = FunctionName.Insert(0, functionName.Substring(0, 1).ToUpperInvariant());

                    GeneratedCode.Text += string.Format("RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(L\"{0}\", L\"webgl\", static{1}, this, &m_{0}));\r\n", functionName, FunctionName);
                    

                    GeneratedCode.Text += string.Format("\r\n", functionName);
                }
            }

            Clipboard.SetText(GeneratedCode.Text);
        }

        private void GenerateWebAudio_Click(object sender, RoutedEventArgs e)
        {
            var webAudioWindow = new WebAudioGenerator();
            webAudioWindow.Show();
            this.Close();
        }
    }
}
