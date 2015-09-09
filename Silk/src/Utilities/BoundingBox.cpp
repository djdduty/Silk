#include <Utilities/BoundingBox.h>
#include <Renderer/RenderObject.h>
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
		/* 
         *       1-------3
         *      /|      /|
         *     5_______7 |
         *     | 0_____|_2
         *     |/      |/
         *     4_______6
         *
         */
		Vec3 HalfExtents = m_ModelSpaceBounds.GetExtents();
		Mat4 tt = m_Obj->GetTransform().Transpose();
        Vec3 Box[8] =
        {
			tt * ((Vec3(-1.0f,-1.0f, 1.0f) * HalfExtents) + m_ModelSpaceBounds.GetCenter()),
            tt * ((Vec3(-1.0f, 1.0f, 1.0f) * HalfExtents) + m_ModelSpaceBounds.GetCenter()),
            tt * ((Vec3( 1.0f,-1.0f, 1.0f) * HalfExtents) + m_ModelSpaceBounds.GetCenter()),
            tt * ((Vec3( 1.0f, 1.0f, 1.0f) * HalfExtents) + m_ModelSpaceBounds.GetCenter()),
            
            tt * ((Vec3(-1.0f,-1.0f,-1.0f) * HalfExtents) + m_ModelSpaceBounds.GetCenter()),
            tt * ((Vec3(-1.0f, 1.0f,-1.0f) * HalfExtents) + m_ModelSpaceBounds.GetCenter()),
            tt * ((Vec3( 1.0f,-1.0f,-1.0f) * HalfExtents) + m_ModelSpaceBounds.GetCenter()),
            tt * ((Vec3( 1.0f, 1.0f,-1.0f) * HalfExtents) + m_ModelSpaceBounds.GetCenter()),
        };

		Vec3 Min,Max;
        Min = Max = Box[0];
        for(i32 i = 0;i < 8;i++)
        {
			Vec3 Vert = Box[i];
            if(Vert.x < Min.x) Min.x = Vert.x;
            if(Vert.y < Min.y) Min.y = Vert.y;
            if(Vert.z < Min.z) Min.z = Vert.z;
            
            if(Vert.x > Max.x) Max.x = Vert.x;
            if(Vert.y > Max.y) Max.y = Vert.y;
            if(Vert.z > Max.z) Max.z = Vert.z;
        }
            

        return AABB(Min,Max);
    }

    AABB OBB::GetLocalAABB() const
    {
        AABB Ret = AABB(m_ModelSpaceBounds.GetCenter() - m_ModelSpaceBounds.GetExtents(),
                        m_ModelSpaceBounds.GetCenter() + m_ModelSpaceBounds.GetExtents());
        Ret.m_HalfExtents *= m_Obj->GetTransform().GetScale();
        return Ret;
    }
}