#include <Renderer/Camera.h>

namespace Silk
{
    const Mat4& Camera::GetProjection()
    {
        if(m_UpdateProjection)
        {
            m_UpdateProjection = false;
            m_ProjChanged = true;
            if(m_IsPerspective) m_Projection = PerspectiveMultiFov(m_FieldOfView.x,m_FieldOfView.y,m_NearPlane,m_FarPlane);
            else m_Projection = Orthogonal(m_OrthoDims.x,m_OrthoDims.x + m_OrthoDims.z,m_OrthoDims.y,m_OrthoDims.y + m_OrthoDims.w,m_NearPlane,m_FarPlane);
            m_Frustum.Set(this);
        }
        return m_Projection;
    }
    
    void Camera::SetPerspective(const Vec2& FoV,Scalar Near,Scalar Far)
    {
        SetFieldOfView(FoV);
        SetZClipPlanes(Near,Far);
        m_IsPerspective = true;
        m_UpdateProjection = true;
    }
    void Camera::SetOrthographic(const Vec2& Dimensions,Scalar Near,Scalar Far)
    {
        SetZClipPlanes(Near,Far);
        m_OrthoDims = Vec4(-Dimensions.x * 0.5f,-Dimensions.y * 0.5f,Dimensions.x,Dimensions.y);
        m_IsPerspective = false;
        m_UpdateProjection = true;
        m_OrDmChanged = true;
    }
    void Camera::SetOrthographic(Scalar Left,Scalar Right,Scalar Top,Scalar Bottom,Scalar Near,Scalar Far)
    {
        SetZClipPlanes(Near,Far);
        m_OrthoDims = Vec4(Left,Top,Right - Left,Bottom - Top);
        m_IsPerspective = false;
        m_UpdateProjection = true;
        m_OrDmChanged = true;
    }
    void Camera::SetFieldOfView(const Vec2 &FoV)
    {
        m_FieldOfView = FoV;
        m_IsPerspective = true;
        m_UpdateProjection = true;
        m_FdVwChanged = true;
    }
    void Camera::SetZClipPlanes(Scalar Near,Scalar Far)
    {
        if(Near != -1) m_NearPlane   = max(Near,0.00001f);
        if(Far  != -1) m_FarPlane    = max(Near + 1.0f,Far);
        m_UpdateProjection = true;
        m_nPlnChanged = m_fPlnChanged = true;
    }
};
