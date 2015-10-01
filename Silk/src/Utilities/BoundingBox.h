#pragma once
#include <Math/Math.h>

namespace Silk
{
	class AABB
	{
        public:
            AABB(Vec3 Min,Vec3 Max) { Set(Min,Max); }
            ~AABB();
        
            void Set(const Vec3& Min,const Vec3& Max);


            bool Contains(const AABB& Other);
            bool Intersects(const AABB& Other);
            bool Intersects(const Ray& r);

            Vec3 GetCenter    () const { return m_Center            ; }
            Vec3 GetExtents   () const { return m_HalfExtents       ; }
            Vec3 GetDimensions() const { return m_HalfExtents * 2.0f; }
        
        protected:
            friend class OBB;
            Vec3 m_HalfExtents;
            Vec3 m_Center;
	};
    
    class RenderObject;
    class OBB
    {
        public:
            OBB(RenderObject* Obj);
            ~OBB();
        
            AABB ComputeWorldAABB();
        
            AABB GetLocalAABB() const;
            RenderObject* GetObject() const { return m_Obj; }
        
            bool IntersectsRay(const Ray& r,f32& IntersectionDistance);
        
        protected:
            friend class RenderObject;
            RenderObject* m_Obj;
            AABB m_ModelSpaceBounds;
    };
}