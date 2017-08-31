This website contains HoloJS sample apps. 

As with local HoloJS app, a JSON file lists the scripts required to run an app.
Index.html contains links to these JSON files. There are 2 types of links:
	HTTP links: these launch the app directly in any browser (flat view)
	Web-AR linnks: the links use the web-ar protocol; links launch in the web-ar viewer app. A sample viewer app is provided.
<app>.html contains the html  for the HTTP version of the HoloJS app. It's an empty document with links to the same scripts listed in the JSON file.

Besides the in browser flat mode, there are 2 options to run HoloJS apps directly from the web in immersive mode:

Option 1:
	In any HoloJS app, load the script by its fully qualified URL. Example: "http://<your_host>/photosphere.json"

Option 2:
	Install the HoloJsWebViewer app (or modify it to suit your needs)
	In any browser, navigate to http://<your_host>/index.html and click on any of the WebAR links.
	The links use the web-ar:// protocol which the viewer app registered for. The viewer activates and runs the app on the clicked link.