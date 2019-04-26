using CommandLine;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace spin
{
    [Verb("run", HelpText = "Run a HoloJs app.")]
    class RunAppOptions
    {
        [Option('u', "uri", Required = false, HelpText = "The URL to run.", SetName ="source")]
        public string Uri { get; set; }

        [Option('u', "path", Required = false, HelpText = "The local path to run.", SetName ="source")]
        public string Path { get; set; }

        [Option('d', "debug", Default = false, Required = false, HelpText = "Enable script debugging. The script will not run until a debugger attaches.")]
        public bool Debug { get; set; }
    }

    [Verb("open", HelpText = "Opens a HoloJs app in Visual Studio Code")]
    class OpenAppOptions
    {
        [Option('p', "path", Required = true, HelpText = "Path to HoloJs app")]
        public string Path { get; set; }
    }

    [Verb("serve", HelpText = "Serve a HoloJs app over HTTP")]
    class ServeAppOptions
    {
        [Option('p', "path", Required = true, HelpText = "The app folder to serve")]
        public string Path { get; set; }

        [Option('q', "qr", Default = false, Required = false, HelpText = "Shows a QR code with the HTTP address of the app")]
        public bool ShowQr { get; set; }
    }

    [Verb("new", HelpText = "Create a new HoloJs app")]
    class AppCreationOptions
    {
        [Option('p', "path", Required = true, HelpText = "Path to save the HoloJs app. The content of the directory will be erased.")]
        public string Path { get; set; }

        [Option('t', "three", Default = true, Required = false, HelpText = "Create a new app based on ThreeJs.")]
        public bool ThreeJs { get; set; }

        [Option('o', "overwrite", Default = false, Required = false, HelpText = "Overwrite the existing directory if it already exists.")]
        public bool Overwrite { get; set; }
    }

    [Verb("edit", HelpText = "Edit the content of a HoloJs app")]
    class AppEditingOptions
    {
        [Option('p', "path", Required = true, HelpText = "Path to the Holojs app.")]
        public string Path { get; set; }

        [Option('t', "name", Required = false, HelpText = "Sets the app name.")]
        public string Name { get; set; }

        [Option('s', "add-script", Required = false, HelpText = "Add a new script to the HoloJs app.")]
        public string AddScriptPath { get; set; }

        [Option('s', "remove-script", Required = false, HelpText = "Remove a script from the HoloJs app.")]
        public string RemoveScriptPath { get; set; }

        [Option('s', "add-resource", Required = false, HelpText = "Add a new resource to the HoloJs app.")]
        public string AddResourcePath { get; set; }

        [Option('s', "remove-resource", Required = false, HelpText = "Remove a resource from the HoloJs app.")]
        public string RemoveResourcePath { get; set; }

    }

    [Verb("publish", HelpText = "Publish a HoloJs app as a single XRSX file.")]
    class AppPublishingOptions
    {
        [Option('s', "source", Required = true, HelpText = "Path to the HoloJs app to publish.")]
        public string SourcePath { get; set; }

        [Option('d', "destination", Required = true, HelpText = "Destination XRSX file.")]
        public string DestinationPath { get; set; }

        [Option('o', "overwrite", Default = false, Required = false, HelpText = "Overwrite the existing XRSX file if it already exists.")]
        public bool Overwrite { get; set; }
    }
}
