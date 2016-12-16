#pragma once

namespace HologramJS
{
	namespace Utilities
	{
		class IRelease
		{
		public:
			virtual void Release() = 0;

			virtual ~IRelease() {}
		};
	}
}
