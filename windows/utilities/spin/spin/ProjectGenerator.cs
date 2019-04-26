using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace HoloJs.Spin
{
    enum HoloJsProjectType
    {
        ThreeJs,
        BabylonJs,
        WebGL
    }

    class ProjectGenerator
    {
        private readonly string ProjectName;
        private readonly string ProjectPath;
        private readonly HoloJsProjectType ProjectType;

        private const string LaunchJsonTemplateSource = @"templates\launch.json";
        private const string ThreeJsSampleAppPath = @"templates\three_template.js";

        private string XrsFilePath => Path.Combine(ProjectPath, XrsFileName);
        private string XrsFileName => ProjectName + ".json";

        private string LaunchJsonPath => Path.Combine(ProjectPath, ".vscode");
        private string LaunchJsonFile => Path.Combine(LaunchJsonPath, "launch.json");

        public string SpinInstallLocation => System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetEntryAssembly().Location);

        private XrsPackage AppPackage;

        public ProjectGenerator(string targetDirectory, HoloJsProjectType projectType = HoloJsProjectType.ThreeJs)
        {
            ProjectName = Path.GetFileName(targetDirectory);
            projectType = ProjectType;
            ProjectPath = targetDirectory;
            AppPackage = XrsPackage.CreateNew(XrsFilePath);
        }

        public void Generate()
        {
            if (Directory.Exists(ProjectPath)) {
                Directory.Delete(ProjectPath, true);
            }

            Directory.CreateDirectory(ProjectPath);
            CreateLaunchJson();

            switch(ProjectType)
            {
                case HoloJsProjectType.ThreeJs:
                    CopyThreeJsFiles();
                    break;
                default:
                    throw new NotImplementedException();
            }

            AppPackage.Save();
        }

        private void CopyThreeJsFiles()
        {
            // Add ThreeJs files first
            var threeJsDestination = Path.Combine(ProjectPath, "three.js");
            var threeJsSource = Path.Combine(SpinInstallLocation, @"templates\three.js");
            File.Copy(threeJsSource, threeJsDestination);
            AppPackage.AddScript("three.js");

            // Add the main script file last
            var mainScriptFileName = ProjectName + ".js";
            var appMainDestinationFile = Path.Combine(ProjectPath, mainScriptFileName);
            var appMainSourceFile = Path.Combine(SpinInstallLocation, ThreeJsSampleAppPath);
            File.Copy(appMainSourceFile, appMainDestinationFile);
            AppPackage.AddScript(ProjectName + ".js");
        }

        private void CreateLaunchJson()
        {
            if (Directory.Exists(LaunchJsonPath))
            {
                Directory.Delete(LaunchJsonPath);
            }

            Directory.CreateDirectory(LaunchJsonPath);
            LaunchJson.WriteForApp(LaunchJsonFile, XrsFileName);
        }
    }

    class LaunchJsonConfiguration
    {
        public string name = "Debug in HoloJs";
        public string type = "node";
        public string request = "launch";
        public string cwd = "${workspaceFolder}";
        public string runtimeExecutable;
        public List<string> runtimeArgs;
        public int port = 9229;
        public string protocol = "inspector";

        public LaunchJsonConfiguration()
        {
            runtimeArgs = new List<string>();
        }
    }

    class LaunchJson
    {
        public string version = "0.2.0";
        public List<LaunchJsonConfiguration> configurations;

        public static void WriteForApp(string destination, string xrsPath)
        {
            var launchJson = new LaunchJson();

            var defaultConfiguration = new LaunchJsonConfiguration();
            defaultConfiguration.runtimeArgs.Add("run");
            defaultConfiguration.runtimeArgs.Add("--path");
            defaultConfiguration.runtimeArgs.Add(xrsPath);
            defaultConfiguration.runtimeArgs.Add("--debug");
            defaultConfiguration.runtimeExecutable = System.Reflection.Assembly.GetEntryAssembly().Location;

            launchJson.configurations.Add(defaultConfiguration);

            File.WriteAllText(destination, Newtonsoft.Json.JsonConvert.SerializeObject(launchJson));
        }

        private LaunchJson()
        {
            configurations = new List<LaunchJsonConfiguration>();
        }
    }
}
