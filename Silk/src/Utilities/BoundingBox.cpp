#include <Utilities/BoundingBox.h>
#include <Renderer/RenderObject.h>
#include <Renderer/Renderer.h>
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
    bool AABB::Intersects(const Ray& r)
    {
        f32 t0 = -100000000.0f;
        f32 t1 =  100000000.0f;
        
        Vec3 bounds[2];
        bounds[0] = -m_HalfExtents;
        bounds[1] =  m_HalfExtents;
        float tmin, tmax, tymin, tymax, tzmin, tzmax;
        if (r.Dir.x >= 0)
        {
            tmin = (bounds[0].x - r.Point.x) / r.Dir.x;
            tmax = (bounds[1].x - r.Point.x) / r.Dir.x;
        }
        else {
            tmin = (bounds[1].x - r.Point.x) / r.Dir.x;
            tmax = (bounds[0].x - r.Point.x) / r.Dir.x;
        }
        if (r.Dir.y >= 0) {
            tymin = (bounds[0].y - r.Point.y) / r.Dir.y;
            tymax = (bounds[1].y - r.Point.y) / r.Dir.y;
        }
        else {
            tymin = (bounds[1].y - r.Point.y) / r.Dir.y;
            tymax = (bounds[0].y - r.Point.y) / r.Dir.y;
        }
        if ( (tmin > tymax) || (tymin > tmax) )
        return false;
        
        if (tymin > tmin)
        tmin = tymin;
        if (tymax < tmax)
        tmax = tymax;
        if (r.Dir.z >= 0) {
        tzmin = (bounds[0].z - r.Point.z) / r.Dir.z;
        tzmax = (bounds[1].z - r.Point.z) / r.Dir.z;
        }
        else {
        tzmin = (bounds[1].z - r.Point.z) / r.Dir.z;
        tzmax = (bounds[0].z - r.Point.z) / r.Dir.z;
        }
        if ( (tmin > tzmax) || (tzmin > tmax) )
        return false;
        if (tzmin > tmin)
        tmin = tzmin;
        if (tzmax < tmax)
        tmax = tzmax;
        return ( (tmin < t1) && (tmax > t0) );
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
    bool OBB::IntersectsRay(const Ray &r,f32 &IntersectionDistance)
    {
        while(true) { printf("Warning: doesn't work.\n"); }
        Ray tRay;
        Mat4 iTrans = m_Obj->GetTransform().Inverse();
        tRay.Point = iTrans * r.Point;
        iTrans.x.w = iTrans.y.w = iTrans.z.w = 0.0f; iTrans.w.w = 1.0f;
        tRay.Dir   = iTrans * r.Dir;
        
        if(!ComputeWorldAABB().Intersects(tRay)) return false;
        m_Obj->GetRenderer()->GetDebugDrawer()->Line(tRay.Point,tRay.Point + (tRay.Dir * 1000.0f),Vec4(1,0,0,1));
        m_Obj->GetRenderer()->GetDebugDrawer()->Line(m_Obj->GetTransform().GetTranslation(),tRay.Point,Vec4(0,0,1,1));
        return true;
    }
}