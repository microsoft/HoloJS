using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System.IO.Compression;

namespace HoloJs.Spin
{
    public class XrsPackage
    {
        [JsonProperty]
        private List<string> scripts;

        [JsonProperty]
        private List<string> resources;

        [JsonIgnore]
        public IReadOnlyList<string> Scripts => scripts.AsReadOnly();

        [JsonIgnore]
        public IReadOnlyList<string> Resources => resources.AsReadOnly();

        [JsonIgnore]

        public string AppBasePath => Path.GetDirectoryName(AppPath);

        public string XsrFileName => Path.GetFileName(AppPath);

        [JsonIgnore]
        public string AppPath;

        public string name;

        public void AddScript(string script)
        {
            if (!scripts.Contains(script.ToLower()))
            {
                scripts.Add(script.ToLower());
            }
        }

        public void RemoveScript(string script)
        {
            if (scripts.Contains(script.ToLower()))
            {
                scripts.Remove(script.ToLower());
            }
        }

        public void AddResource(string resourcePath)
        {
            if (!resources.Contains(resourcePath.ToLower()))
            {
                resources.Add(resourcePath.ToLower());
            }
        }

        public void RemoveResource(string resourcePath)
        {
            if (resources.Contains(resourcePath.ToLower()))
            {
                resources.Remove(resourcePath.ToLower());
            }
        }

        public int ShiftScriptApp(string script)
        {
            var lowerScript = script.ToLower();
            var index = scripts.IndexOf(lowerScript);
            if (index > 0)
            {
                scripts.RemoveAt(index);
                scripts.Insert(index - 1, lowerScript);

                return index - 1;
            }

            return index;
        }

        public int ShiftScriptDown(string script)
        {
            var lowerScript = script.ToLower();
            var index = scripts.IndexOf(lowerScript);
            if (index < (scripts.Count - 1))
            {
                scripts.RemoveAt(index);
                scripts.Insert(index + 1, lowerScript);
                return index + 1;
            }

            return index;
        }

        public XrsPackage()
        {
            scripts = new List<string>();
            resources = new List<string>();
            name = "";
        }

        public static XrsPackage CreateNew(string appJson)
        {
            var returnValue = new XrsPackage();
            returnValue.AppPath = Path.GetFullPath(appJson);

            return returnValue;
        }

        public static XrsPackage Open(string appJson)
        {
            var jsonContent = File.ReadAllText(appJson);
            var returnValue = JsonConvert.DeserializeObject<XrsPackage>(jsonContent);
            returnValue.AppPath = Path.GetFullPath(appJson);

            returnValue.scripts =  returnValue.scripts.ConvertAll(e => e.ToLower());
            returnValue.resources =  returnValue.resources.ConvertAll(e => e.ToLower());

            return returnValue;
        }

        public string GetScriptPath(string scriptRelativePath)
        {
            return Path.Combine(AppBasePath, scriptRelativePath);
        }

        public void Save(string appsJson)
        {
            var jsonContent = JsonConvert.SerializeObject(this);
            File.WriteAllText(appsJson, jsonContent);
        }

        public void Save()
        {
            Save(AppPath);
        }

        private string AddFile(string filePath)
        {
            var scriptDirectoryName = System.IO.Path.GetDirectoryName(filePath);
            var appBasePath = AppBasePath;
            string relativePath;

            if (scriptDirectoryName.StartsWith(appBasePath))
            {
                // the script is in a subdirectory of the the app json; no copy required
                relativePath = filePath.Replace(appBasePath, "");
                relativePath = relativePath.Trim(new char[] { '\\', '/' });
            }
            else
            {
                var scriptFileName = System.IO.Path.GetFileName(filePath);
                System.IO.File.Copy(filePath, System.IO.Path.Combine(appBasePath, scriptFileName));
                relativePath = scriptFileName;
            }

            return relativePath.Replace('\\', '/');

        }

        public void AddResourceFile(string filePath)
        {
            var relativePath = AddFile(filePath);

            if (!resources.Contains(relativePath))
            {
                resources.Add(relativePath.ToLower());
            }
        }

        public void AddScriptFile(string filePath)
        {
            var relativePath = AddFile(filePath);
            
            if (!scripts.Contains(relativePath))
            {
                scripts.Add(relativePath.ToLower());
            }
        }

        public void CreatePackage(string destinationFile)
        {
            Save();

            using (var packageFile = File.Create(destinationFile))
            {
                using (var archive = new ZipArchive(packageFile, ZipArchiveMode.Create, true))
                {
                    var filesToPackage = new List<string>(scripts);
                    filesToPackage.AddRange(resources);
                    filesToPackage.Add(AppPath);
                    foreach(var file in filesToPackage)
                    {
                        ZipArchiveEntry entry;
                        if (file.Equals(AppPath))
                        {
                            entry = archive.CreateEntry("app.xrs");
                        }
                        else
                        {
                            entry = archive.CreateEntry(file.ToLower());
                        }

                        var scriptFileSource = Path.Combine(AppBasePath, file);

                        using (var scriptFile = File.Open(scriptFileSource, FileMode.Open))
                        {
                            using (var entryStream = entry.Open())
                            {
                                scriptFile.CopyTo(entryStream);
                            }
                        }
                    }
                    
                }
            }
        }
    }
}
