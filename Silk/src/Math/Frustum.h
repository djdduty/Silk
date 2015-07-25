#pragma once
#include <Math/Math.h>
#include <Math/Plane.h>

namespace Silk
{
    class Camera;
    
    class Frustum
    {
        public:
            enum FRUSTUM_PLANE
            {
                FP_NEAR,
                FP_LEFT,
                FP_RIGHT,
                FP_TOP,
                FP_BOTTOM,
                FP_FAR
            };
        
            Frustum() { }
            ~Frustum() { }
        
            void Set(Camera* c);
            Plane GetPlane(FRUSTUM_PLANE p) const { return m_Planes[p]; }
        
            bool ContainsPoint(const Vec3& Pt) const;
        
        protected:
            Plane m_Planes[6];
    };
}