#include <Utilities/BoundingBox.h>

namespace Silk
{
	AABB::AABB(Vec3 Min, Vec3 Max)
	{
		HalfDim = Vec3(fabs(Max.x - Min.x) / 2, fabs(Max.y - Min.y) / 2, fabs(Max.z - Min.z) / 2);
		Origin = Vec3(0,0,0);
	}

	AABB::~AABB()
	{

	}

	bool AABB::Contains(AABB* Other)
	{
		Vec3 OMin = Other->GetOrigin() - Other->HalfDim;
		Vec3 OMax = Other->GetOrigin() + Other->HalfDim;
		Vec3 Max = GetOrigin() + HalfDim;
		Vec3 Min = GetOrigin() - HalfDim;
		if ((OMin.x < Min.x) || (OMin.y < Min.y) || (OMin.z < Min.z)) return false;
		if ((OMax.x > Max.x) || (OMax.y > Max.y) || (OMax.z > Max.z)) return false;
		return true;
	}

	bool AABB::Intersects(AABB* Other)
	{
		if (   (fabs(GetOrigin().x - Other->GetOrigin().x) >= (HalfDim.x + Other->HalfDim.x))
			|| (fabs(GetOrigin().y - Other->GetOrigin().y) >= (HalfDim.y + Other->HalfDim.y))
			|| (fabs(GetOrigin().z - Other->GetOrigin().z) >= (HalfDim.z + Other->HalfDim.z)))
			return false;

		return true;
	}
}