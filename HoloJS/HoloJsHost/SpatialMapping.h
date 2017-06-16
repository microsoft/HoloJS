#pragma once
#include <list>
namespace HologramJS
{
	namespace Input
	{
		class SpatialMapping
		{
		public:
			SpatialMapping();
			~SpatialMapping();

			void EnableSpatialMapping(Windows::Perception::Spatial::SpatialStationaryFrameOfReference^ frameOfReference);
			void DisableSpatialMapping() { m_frameOfReference = nullptr; }

			void SetScriptCallback(JsValueRef scriptCallback) { m_scriptCallback = scriptCallback; }

			JsValueRef GetSpatialData();

			void ProcessOneSpatialMappingDataUpdate();

		private:

			JsValueRef m_scriptCallback = JS_INVALID_REFERENCE;

			Windows::Perception::Spatial::SpatialStationaryFrameOfReference^ m_frameOfReference;
			bool m_surfaceAccessAllowed = false;
			Windows::Perception::Spatial::Surfaces::SpatialSurfaceObserver^ m_surfaceObserver;
			Windows::Perception::Spatial::Surfaces::SpatialSurfaceMeshOptions^ m_surfaceMeshOptions;

			bool CreateSurfaceObserver();

			void ProcessSurface(Windows::Perception::Spatial::Surfaces::SpatialSurfaceInfo^ surface);

			std::mutex m_processingLock;

			std::list<std::vector<short int>> m_indexBuffers;
			std::list<std::vector<float>> m_vertexBuffers;
			std::list<std::vector<byte>> m_normalBuffers;
			std::list<std::vector<float>> m_originToSurfaces;
			std::list<std::vector<byte>> m_meshIds;

			bool m_spatialMappingDataAvailable = false;
		};


	}
}

