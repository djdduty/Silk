#include <Math/Math.h>
#include <Renderer/Camera.h>

namespace Silk
{
    void Frustum::Set(Camera* c)
    {
        Mat4 pv = (c->GetProjection()/* * c->GetTransform().Inverse()*/).Transpose();
        m_Offset = c->GetTransform().GetTranslation();
        
		m_Planes[FP_LEFT  ] = Plane(Vec4(pv.x.w + pv.x.x,
                                         pv.y.w + pv.y.x,
                                         pv.z.w + pv.z.x,
                                         pv.w.w + pv.w.x));
        
		m_Planes[FP_RIGHT ] = Plane(Vec4(pv.x.w - pv.x.x,
                                         pv.y.w - pv.y.x,
                                         pv.z.w - pv.z.x,
                                         pv.w.w - pv.w.x));
        
		m_Planes[FP_TOP   ] = Plane(Vec4(pv.x.w - pv.x.y,
                                         pv.y.w - pv.y.y,
                                         pv.z.w - pv.z.y,
                                         pv.w.w - pv.w.y));
        
		m_Planes[FP_BOTTOM] = Plane(Vec4(pv.x.w + pv.x.y,
                                         pv.y.w + pv.y.y,
                                         pv.z.w + pv.z.y,
                                         pv.w.w + pv.w.y));
                                         
		m_Planes[FP_NEAR  ] = Plane(Vec4(pv.x.w + pv.x.z,
                                         pv.y.w + pv.y.z,
                                         pv.z.w + pv.z.z,
                                         pv.w.w + pv.w.z));
        
		m_Planes[FP_FAR   ] = Plane(Vec4(pv.x.w - pv.x.z,
                                         pv.y.w - pv.y.z,
                                         pv.z.w - pv.z.z,
                                         pv.w.w - pv.w.z));
        
        m_Planes[0].Normalize();
        m_Planes[1].Normalize();
        m_Planes[2].Normalize();
        m_Planes[3].Normalize();
        m_Planes[4].Normalize();
        m_Planes[5].Normalize();
    }
    bool Frustum::ContainsPoint(const Vec3& Pt) const
    {
        for(i32 i = 0;i < 6;i++)
        {
            if(m_Planes[i].DistanceToPoint(Pt + m_Offset) < 0)
            {
                return false;
            }
        }
        return true;
    }
}