#pragma once

namespace HologramJS
{
	using float4x4 = Windows::Foundation::Numerics::float4x4;

	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class HologramScriptHost sealed
	{
	public:
		HologramScriptHost();

		virtual ~HologramScriptHost();

		bool Initialize();
		void Shutdown();

		bool EnableHolographicExperimental(Windows::Perception::Spatial::SpatialStationaryFrameOfReference^ frameOfReference);

		Windows::Foundation::IAsyncOperation<bool>^ RunLocalScriptAppAsync(Platform::String^ jsonFilePath);
		Windows::Foundation::IAsyncOperation<bool>^ RunWebScriptAppAsync(Platform::String^ jsonUri);

		void ResizeWindow(int width, int height) { m_window.Resize(width, height); }
		void VSync(float4x4 viewMatrix) { m_window.VSync(viewMatrix, *new float4x4(), *new float4x4(), *new float4x4(), *new float4x4(), *new float4x4()); } // backwards compatibility
		void VSync(float4x4 midViewMatrix, float4x4 midProjectionMatrix, float4x4 leftViewMatrix, float4x4 leftProjectionMatrix, float4x4 rightViewMatrix, float4x4 rightProjectionMatrix) { m_window.VSync(midViewMatrix, midProjectionMatrix, leftViewMatrix, leftProjectionMatrix, rightViewMatrix, rightProjectionMatrix); }

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
