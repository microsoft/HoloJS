//
// This file demonstrates how to initialize EGL in a Windows Store app, using ICoreWindow.
//

#include "pch.h"
#include <WindowsNumerics.h>

using namespace Windows::ApplicationModel::Core;
using namespace Platform;

// Implementation of the IFrameworkViewSource interface, necessary to run our app.
ref class SimpleApplicationSource sealed : IFrameworkViewSource
{
public:
	virtual IFrameworkView^ CreateView()
	{
        // Run a basic HoloJS app
        // If custom functionality is desired in the native app, copy the BasicHoloJsApp code here and make\
        // changes as needed (look at SampleApp which does not use the BasicHoloJsApp class)
		return ref new HologramJS::BasicHoloJsApp(ref new String(L"http://holojs.azurewebsites.net/fbxloader/app.json"));
	}
};

// The main function creates an IFrameworkViewSource for our app, and runs the app.
[Platform::MTAThread]
int main(Array<Platform::String^>^)
{
	auto simpleApplicationSource = ref new SimpleApplicationSource();
	CoreApplication::Run(simpleApplicationSource);
	return 0;
}