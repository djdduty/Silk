#include <Math/Math.h>
#include <Renderer/Camera.h>

namespace Silk
{
    void Frustum::Set(Camera* c)
    {
        Mat4 pv = c->GetProjection().Inverse() * c->GetTransform().Inverse();
        
        m_Planes[FP_RIGHT ].Normal.x = pv.w.x + pv.x.x;
        m_Planes[FP_RIGHT ].Normal.y = pv.w.y + pv.x.y;
        m_Planes[FP_RIGHT ].Normal.z = pv.w.z + pv.x.z;
        m_Planes[FP_RIGHT ].Offset   = pv.w.w + pv.x.w;
        
        m_Planes[FP_LEFT  ].Normal.x = pv.w.x - pv.x.x;
        m_Planes[FP_LEFT  ].Normal.y = pv.w.y - pv.x.y;
        m_Planes[FP_LEFT  ].Normal.z = pv.w.z - pv.x.z;
        m_Planes[FP_LEFT  ].Offset   = pv.w.w - pv.x.w;
        
        m_Planes[FP_TOP   ].Normal.x = pv.w.x - pv.y.x;
        m_Planes[FP_TOP   ].Normal.y = pv.w.y - pv.y.y;
        m_Planes[FP_TOP   ].Normal.z = pv.w.z - pv.y.z;
        m_Planes[FP_TOP   ].Offset   = pv.w.w - pv.y.w;
        
        m_Planes[FP_BOTTOM].Normal.x = pv.w.x + pv.y.x;
        m_Planes[FP_BOTTOM].Normal.y = pv.w.y + pv.y.y;
        m_Planes[FP_BOTTOM].Normal.z = pv.w.z + pv.y.z;
        m_Planes[FP_BOTTOM].Offset   = pv.w.w + pv.y.w;
        
        m_Planes[FP_FAR   ].Normal.x = pv.w.x + pv.z.x;
        m_Planes[FP_FAR   ].Normal.y = pv.w.y + pv.z.y;
        m_Planes[FP_FAR   ].Normal.z = pv.w.z + pv.z.z;
        m_Planes[FP_FAR   ].Offset   = pv.w.w + pv.z.w;
        
        m_Planes[FP_NEAR  ].Normal.x = pv.w.x - pv.z.x;
        m_Planes[FP_NEAR  ].Normal.y = pv.w.y - pv.z.y;
        m_Planes[FP_NEAR  ].Normal.z = pv.w.z - pv.z.z;
        m_Planes[FP_NEAR  ].Offset   = pv.w.w - pv.z.w;
        
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
            if(m_Planes[i].DistanceToPoint(Pt) < 0)
            {
                return false;
            }
        }
        return true;
    }
}