#include <Utilities/BoundingBox.h>
#include <cmath>

namespace Silk
{
	AABB::~AABB()
	{

	}
    void AABB::Set(const Vec3& Min,const Vec3& Max)
    {
        m_HalfExtents = (Max - Min) * 0.5f;
        m_Center = Min + m_HalfExtents;
    }

	bool AABB::Contains(const AABB& Other)
	{
		Vec3 OMin = Other.m_Center - Other.m_HalfExtents;
		Vec3 OMax = Other.m_Center + Other.m_HalfExtents;
		Vec3 Max = m_Center + m_HalfExtents;
		Vec3 Min = m_Center - m_HalfExtents;
		if ((OMin.x < Min.x) || (OMin.y < Min.y) || (OMin.z < Min.z)) return false;
		if ((OMax.x > Max.x) || (OMax.y > Max.y) || (OMax.z > Max.z)) return false;
		return true;
	}

	bool AABB::Intersects(const AABB& Other)
	{
		if (   (fabs(m_Center.x - Other.m_Center.x) >= (  m_HalfExtents.x + Other.m_HalfExtents.x))
			|| (fabs(m_Center.y - Other.m_Center.y) >= (  m_HalfExtents.y + Other.m_HalfExtents.y))
			|| (fabs(m_Center.z - Other.m_Center.z) >= (  m_HalfExtents.z + Other.m_HalfExtents.z)))
			return false;

		return true;
	}
    
    OBB::OBB(RenderObject* Obj) : m_Obj(Obj), m_ModelSpaceBounds(Vec3(),Vec3())
    {
    }
    OBB::~OBB()
    {
    }

    AABB OBB::ComputeWorldAABB()
    {
        return AABB(Vec3(),Vec3());
    }
}