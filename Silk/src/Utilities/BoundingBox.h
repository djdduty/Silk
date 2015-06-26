#ifndef SILK_BB_H
#define SILK_BB_H

#include <Math/Math.h>

namespace Silk
{
	class AABB
	{
	public:
		AABB(Vec3 Min, Vec3 Max);
		~AABB();

		bool Contains(AABB* Other);
		bool Intersects(AABB* Other);

		Vec3 GetOrigin()
		{
			return Origin; // modify this to account for position
		}

		Vec3 GetHalfDimensions()
		{
			return HalfDim;
		}

		protected:
			friend class AABB;
			Vec3 Origin;
			Vec3 HalfDim;
	};
}

#endif