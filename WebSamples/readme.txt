tl;dr
This website contains HoloJS sample apps. To test it, copy its contents to your web host and point your browser to it. To view the immersive WebAR links, install the sample viewer app.

Site structure:

Index.html contains links to sample apps. There are 2 types of links:
	HTTP links (flat): these launch the app directly in any browser (flat view)
	Web-AR linnks (immersive): the links use the web-ar protocol; links launch in the web-ar viewer app. A sample viewer app is provided.

Folders contain samples apps and the required frameworks to run them..
	<app>.json lists, as with local HoloJS app, the scripts that make up the app.
	<app>.html contains the html for the HTTP version of the HoloJS app. It's an empty document with links to the same scripts listed in the JSON file.
	<app>.js contains the main script for the app.
	
Besides the "in browser" flat links, there are 2 options to run HoloJS apps directly from the web in immersive mode:

Option 1:
	In any HoloJS app, load the script by its fully qualified URL. Example: "http://<your_host>/photosphere.json"

Option 2:
	Install the HoloJsWebViewer app (or modify it to suit your needs)
	In any browser, navigate to http://<your_host>/index.html and click on any of the WebAR links.
	The links use the web-ar:// protocol which the viewer app registered for. The viewer activates and runs the app on the clicked link.