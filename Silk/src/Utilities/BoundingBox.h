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

            Vec3 GetCenter    () const { return m_Center            ; }
            Vec3 GetExtents   () const { return m_HalfExtents       ; }
            Vec3 GetDimensions() const { return m_HalfExtents * 2.0f; }
        
        protected:
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
        
            AABB GetLocalAABB() const { return m_ModelSpaceBounds; }
            RenderObject* GetObject() const { return m_Obj; }
        
        protected:
            friend class RenderObject;
            RenderObject* m_Obj;
            AABB m_ModelSpaceBounds;
    };
}