using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.IO;
using System.Net.Sockets;

namespace HoloJs.Spin
{
    class AppServer
    {
        Task ServerTask;
        HttpListener Listener;
        bool StopRequested;

        string BasePath;

        const string TemporaryPath = "Temporary_Listen_Addresses\\holojs\\";

        const string BaseUrl = "http://+:80/Temporary_Listen_Addresses/holojs/";

        public string GetLaunchUrl()
        {
            string localIP;
            using (Socket socket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, 0))
            {
                socket.Connect("8.8.8.8", 65530);
                IPEndPoint endPoint = socket.LocalEndPoint as IPEndPoint;
                localIP = endPoint.Address.ToString();
            }

            return string.Format("http://{0}:80/Temporary_Listen_Addresses/holojs/", localIP);
        }

        public AppServer(string appPath)
        {
            BasePath = Path.GetDirectoryName(appPath);
        }

        public void Stop()
        {
            StopRequested = true;
            Listener.Stop();

            if (ServerTask != null)
            {
                ServerTask.Wait();
                ServerTask = null;
            }
        }

        public void RunAsync()
        {
            if (ServerTask != null)
            {
                return;
            }

            StopRequested = false;
            ServerTask = new Task(() => Run());
            ServerTask.Start();
        }

        public void Run()
        {
            Listener = new HttpListener();
            Listener.Prefixes.Add(BaseUrl);

            Listener.Start();

            while(!StopRequested)
            {
                try
                {
                    HttpListenerContext context = Listener.GetContext();
                    HttpListenerRequest request = context.Request;
                    HttpListenerResponse response = context.Response;

                    var localPath = request.Url.LocalPath.Replace("/", "\\").ToLower();
                    localPath = localPath.TrimStart(new char[] { '\\' });
                    localPath = localPath.Replace(TemporaryPath.ToLower(), "");
                    var requestedFile = Path.Combine(BasePath, localPath);
                    if (File.Exists(requestedFile))
                    {
                        var fileExtension = Path.GetExtension(requestedFile);
                        SetContentTypeFromExtension(response, fileExtension);
                        var fileContent = File.ReadAllBytes(requestedFile);
                        response.ContentLength64 = fileContent.Length;
                        System.IO.Stream output = response.OutputStream;
                        output.Write(fileContent, 0, fileContent.Length);
                        output.Close();
                    }
                    else
                    {
                        response.StatusCode = (int)HttpStatusCode.NotFound;
                        response.OutputStream.Close();
                    }
                }
                catch (Exception) { }
            }
        }

        private void SetContentTypeFromExtension(HttpListenerResponse response, string extension)
        {
            if (extension.ToLower() == ".html")
            {
                response.ContentType = "text/html";
            }
            else if (extension.ToLower() == ".js")
            {
                response.ContentType = "application/x-javascript";
            }
        }
    }
}
