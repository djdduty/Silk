#pragma once
#include <Math/Math.h>
#include <Math/Plane.h>
#include <Utilities/BoundingBox.h>
#include <Renderer/RenderObject.h>

namespace Silk
{
    class Camera;
    
    class Frustum
    {
        public:
            enum FRUSTUM_PLANE
            {
                FP_LEFT,
                FP_RIGHT,
                FP_TOP,
                FP_BOTTOM,
                FP_NEAR,
                FP_FAR
            };
        
            Frustum() { }
            ~Frustum() { }
        
            void Set(Camera* c);
            Plane GetPlane(FRUSTUM_PLANE p) const { return m_Planes[p]; }
        
            bool ContainsPoint(const Vec3& Pt) const;
            bool ContainsOBB(const Vec3& Pt, const OBB& BoundingBox) const;
        
        protected:
            Plane m_Planes[6];
    };
}