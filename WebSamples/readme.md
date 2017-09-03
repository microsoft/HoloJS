## Samples website

To accommodate the increasing number of samples - and to showcase HoloJs's web loading capabilities - we checked in a website for samples. A live instance of this website can be found at [http://holojs.azurewebsites.net/samples/index.html](http://holojs.azurewebsites.net/v1/samples/index.html).

Sample apps run in any WebGL capable browser (flat view) or in the provided viewer app (immersive view).

## Site structure
The site structure is very simple.
### Index.html contains links to sample apps
There are 2 types of links:
1. HTTP links (flat): launch the app directly in any browser (flat view)
2. Web-AR links (immersive): web-ar protocol; links launch in a web-ar viewer app. A sample viewer app is provided (HoloJsWebViewer)

### Folders contain samples apps and the required frameworks to run them
The folder structure for an app is:
* app.json lists, as with local HoloJS app, the scripts that make up the app.
* app.html contains the html for the HTTP version of the HoloJS app. It's an empty document with links to the same scripts listed in the JSON file.
* app.js contains the main script for the app.
* other resources required for the app

## Run immersive HoloJS apps from the web 
So far, HoloJS samples were packaged with a native app. This samples website shows how to run arbitrary HoloJS scripts from the web, without the need for packaging scripts with the native app.

Using the provided viewer app, immersive HoloJS apps can be launched directly from the browser. The viewer app registers the web-ar protocol. When 'a web-ar://' link is navigated in the browser, the viewer app is activated then loads and executes the navigated app.

A native HoloJS host app can load web scripts directly by referencing a JSON file by its fully qualified URL: http://<host>/<path>/<app>.json. All resources required to run the app can be hosted on the web.

