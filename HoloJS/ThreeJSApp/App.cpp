//
// This file demonstrates how to initialize EGL in a Windows Store app, using ICoreWindow.
//

#include "pch.h"
#include "app.h"
#include <WindowsNumerics.h>

using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation::Numerics;
using namespace Windows::Graphics::Display;
using namespace Windows::Graphics::Holographic;
using namespace Windows::Perception::Spatial;
using namespace Microsoft::WRL;
using namespace Platform;

using namespace ThreeJSApp;

// Implementation of the IFrameworkViewSource interface, necessary to run our app.
ref class SimpleApplicationSource sealed : Windows::ApplicationModel::Core::IFrameworkViewSource
{
public:
	virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView()
	{
        // Run a basic HoloJS app
        // If custom functionality is desired in the native app, copy the BasicHoloJsApp code here and make\
        // changes as needed (look at SampleApp which does not use the BasicHoloJsApp class)
		return ref new HologramJS::BasicHoloJsApp(ref new String(L"http://holojs.azurewebsites.net/fbxloader/app.json"));
	}
};

// The main function creates an IFrameworkViewSource for our app, and runs the app.
[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
	auto simpleApplicationSource = ref new SimpleApplicationSource();
	CoreApplication::Run(simpleApplicationSource);
	return 0;
}