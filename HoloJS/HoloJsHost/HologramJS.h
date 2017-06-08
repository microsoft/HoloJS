#pragma once

namespace HologramJS
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class HologramScriptHost sealed
	{
	public:
		HologramScriptHost();

		virtual ~HologramScriptHost();

		bool Initialize();
		void Shutdown();

		bool EnableHolographicExperimental();

		Windows::Foundation::IAsyncOperation<bool>^ RunLocalScriptAppAsync(Platform::String^ jsonFilePath);
		Windows::Foundation::IAsyncOperation<bool>^ RunWebScriptAppAsync(Platform::String^ jsonUri);

		void ResizeWindow(int width, int height) { m_window.Resize(width, height); }
		void VSync(Windows::Foundation::Numerics::float4x4 viewMatrix) { m_window.VSync(viewMatrix); }

	private:
		JsRuntimeHandle m_jsRuntime = nullptr;
		JsContextRef m_jsContext = nullptr;
		JsSourceContext m_jsSourceContext = 0;

		concurrency::task<bool> RunLocalScriptApp(std::wstring jsonFilePath);
		concurrency::task<bool> RunWebScriptApp(std::wstring jsonFilePath);

		API::WindowElement m_window;
		API::System m_system;
		Utilities::EventsManager m_scriptEventsManager;
	};
}
