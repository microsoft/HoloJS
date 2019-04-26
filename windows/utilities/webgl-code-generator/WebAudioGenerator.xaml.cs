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
using System.Text.RegularExpressions;

namespace webgl_code_generator
{
    /// <summary>
    /// Interaction logic for WebAudioGenerator.xaml
    /// </summary>
    public partial class WebAudioGenerator : Window
    {
        public WebAudioGenerator()
        {
            InitializeComponent();
        }

        private void Generate_Click(object sender, RoutedEventArgs e)
        {
            var lines = Text.Text.Split(new char[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries);

            string generatedCode = "";

            var readonlyAttributePattern = new Regex(@"^readonly attribute .+ (?<parameter>.+);$");
            var attributePattern = new Regex(@"^attribute .+ (?<parameter>.+);$");
            var functionPattern = new Regex(@"^.+ (?<function>.+)\(");

            

            foreach (var line in lines)
            {
                var trimmedLine = line.Trim();

                if (readonlyAttributePattern.IsMatch(trimmedLine))
                {
                    var readonlyParamMatch = readonlyAttributePattern.Match(trimmedLine);
                    var parameterName = readonlyParamMatch.Groups["parameter"].Value;

                    generatedCode += string.Format("Object.defineProperty(this, '{0}', {{ get: function () {{ throw 'not implemented';  }} }});", parameterName) + "\r\n";
                } else if (attributePattern.IsMatch(trimmedLine))
                {
                    var paramMatch = attributePattern.Match(trimmedLine);
                    var parameterName = paramMatch.Groups["parameter"].Value;

                    generatedCode += string.Format("Object.defineProperty(this, '{0}', {{ get: function () {{ throw 'not implemented'; }}, set: function (value) {{ throw 'not implemented'; }} }});", parameterName) + "\r\n";
                } else if (functionPattern.IsMatch(trimmedLine))
                {
                    var functionMatch = functionPattern.Match(trimmedLine);
                    var functionName = functionMatch.Groups["function"].Value;
                    generatedCode += string.Format("this.{0} = function() {{ throw 'not implemented'; }};", functionName) + "\r\n";
                }
            }

            Clipboard.SetText(generatedCode);
        }
    }
}
