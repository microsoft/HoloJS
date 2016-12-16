#pragma once

namespace HologramJS
{
	namespace WebGL
	{
		class RenderingContext2D : public HologramJS::Utilities::IRelease
		{
		public:
			RenderingContext2D() {}

			virtual ~RenderingContext2D() {}

			void Release() {}

			bool OptimizedBufferAvailable() { return m_isOptimizedBitmap; }
			size_t GetOptimizedBufferSize() { return m_optimizedBitmap.size(); }
			void CopyOptimizedBitmapToBuffer(byte* buffer) { CopyMemory(buffer, m_optimizedBitmap.data(), m_optimizedBitmap.size()); }

			void drawImage3(
				HologramJS::API::ImageElement* imageElement,
				GLsizei width,
				GLsizei heigh,
				unsigned int sx,
				unsigned int sy,
				unsigned int sWidth,
				unsigned int sHeight,
				unsigned int dx,
				unsigned int dy,
				unsigned int dWidth,
				unsigned int dHeight
			);

			void drawImage1(
				HologramJS::API::ImageElement* imageElement,
				GLsizei width,
				GLsizei heigh,
				unsigned int dx,
				unsigned int dy
			);

			void drawImage2(
				HologramJS::API::ImageElement* imageElement,
				GLsizei width,
				GLsizei heigh,
				unsigned int dx,
				unsigned int dy,
				unsigned int dWidth,
				unsigned int dHeight
			);

			Platform::Array<unsigned char>^ getImageData(int sx, int sy, int sw, int sh);

			void SetSize(unsigned int width, unsigned int height)
			{
				m_height = height;
				m_width = width;
			}

		private:

			unsigned int m_height;
			unsigned int m_width;

			std::vector<byte> m_optimizedBitmap;
			bool m_isOptimizedBitmap = false;;

			Microsoft::Graphics::Canvas::CanvasRenderTarget^ m_canvasRenderTarget = nullptr;
		};
	}
}
