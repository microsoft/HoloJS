#pragma once

namespace HologramJS
{
	namespace Utilities
	{
		class ScriptResourceTracker;
	}
	namespace API
	{
		class ExternalObject
		{
		public:
			ExternalObject() {}

			template<typename T> bool Initialize(T* object)
			{
				Utilities::IRelease* releaseInterface = dynamic_cast<Utilities::IRelease*>(object);

				RETURN_IF_NULL(releaseInterface);

				m_object = object;
				m_objectEventsInterface = dynamic_cast<Utilities::ElementWithEvents*>(object);
				m_releaseInterface = releaseInterface;

				return true;
			}

			virtual ~ExternalObject()
			{
				if (m_releaseInterface)
				{
					delete m_releaseInterface;
					m_object = nullptr;
					m_releaseInterface = nullptr;
					m_objectEventsInterface = nullptr;
				}
			}

			PVOID Data() { return m_object; }
			Utilities::ElementWithEvents* EventsInterface() { return m_objectEventsInterface; }

			void SetResourceTracker(HologramJS::Utilities::ScriptResourceTracker* resourceTracker) { m_resourceTracker = resourceTracker; }
			HologramJS::Utilities::ScriptResourceTracker* GetResourceTracker() { return m_resourceTracker; }

		private:
			PVOID m_object = nullptr;
			Utilities::ElementWithEvents* m_objectEventsInterface = nullptr;
			Utilities::IRelease* m_releaseInterface = nullptr;

			HologramJS::Utilities::ScriptResourceTracker* m_resourceTracker;
		};
	}
}

