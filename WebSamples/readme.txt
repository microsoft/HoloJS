This website contains HoloJS sample apps that can run directly from the web.

As with local HoloJS app, a JSON file lists the scripts required to run an app.
Index.html contains links to these JSON files. The links use the web-ar protocol; this allows loading the apps into a viewer app directly from the browser.

There are 2 options to run HoloJS apps from the web.

Option 1:
	In your app, run the script by its fully qualified URL. Example: "http://<your_host>/photosphere.json"

Option 2:
	Install the HoloJsWebViewer app (or modify it to suit your needs)
	In a browser, navigate to http://<your_host>/index.html and click on any of the links.
	The links are on the web-ar:// protocol which the viewer app registered for. The viewer activates and runs the app on the clicked link