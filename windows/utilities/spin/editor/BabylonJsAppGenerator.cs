using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace HoloJs.Spin
{
    class BabylonJsAppGenerator
    {
        private string AppName;
        public string ParentDirectory;
        public string AppDirectory;

        private string JsonFilePath;

        private string MainFileName => AppName + ".js";
        private string HtmlFileName => AppName + ".html";

        public BabylonJsAppGenerator(string appPath)
        {
            ParentDirectory = Path.GetDirectoryName(appPath);
            AppName = Path.GetFileNameWithoutExtension(appPath);
            AppDirectory = Path.Combine(ParentDirectory, AppName);
            JsonFilePath = Path.Combine(AppDirectory, Path.GetFileName(appPath));
        }

        private void EnsureDirectory(string path)
        {
            if (!Directory.Exists(path))
            {
                Directory.CreateDirectory(path);
            }
        }

        public ScriptApp CreateThreeJsApp()
        {
            var createdApp = ScriptApp.CreateNew(JsonFilePath);
            createdApp.name = AppName;

            // Order is important; this is the execution order
            createdApp.AddScript("babylonjs/babylon.js");
            createdApp.AddScript(MainFileName);

            // Source of the template files
            var templateSourceDirectory = Path.Combine(assembler_studio.MainWindow.LocalPath, "babylonjs-template");

            var jsCode = GetJsContent(templateSourceDirectory);
            WriteAppFile(jsCode, MainFileName, AppDirectory);

            var htmlCode = GetHtmlContent(templateSourceDirectory);
            WriteAppFile(htmlCode, HtmlFileName, AppDirectory);


            // Copy the template files to the destination, renaming files as needed
            string[] sourceFileNames = new string[] { "babylonjs/babylon.js" };
            string[] destinationFileNames = new string[] { "babylonjs/babylon.js" };
            for (int i = 0; i < sourceFileNames.Length; i++)
            {
                var sourcePath = System.IO.Path.Combine(templateSourceDirectory, sourceFileNames[i]);
                CopyAppFile(sourcePath, Path.Combine(AppDirectory, destinationFileNames[i]));
            }

            createdApp.Save();
            return createdApp;
        }

        private string GetHtmlContent(string templatePath)
        {
            var templateContent = File.ReadAllText(Path.Combine(templatePath, "template.html"));
            return templateContent.Replace("<<js_app_file_here>>", MainFileName);
        }

        private string GetJsContent(string templatePath)
        {
            var templateContent = File.ReadAllText(Path.Combine(templatePath, "template.js"));
            return templateContent.Replace("<<js_app_file_here>>", MainFileName);
        }

        private void WriteAppFile(string content, string destinationFileName, string destinationDirectory)
        {
            EnsureDirectory(destinationDirectory);
            File.WriteAllText(Path.Combine(destinationDirectory, destinationFileName), content, Encoding.UTF8);
        }

        private void CopyAppFile(string sourcePath, string destinationPath)
        {
            var destinationDirectory = Path.GetDirectoryName(destinationPath);
            EnsureDirectory(destinationDirectory);
            File.Copy(sourcePath, destinationPath, true);
        }
    }
}
