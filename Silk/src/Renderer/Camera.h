#pragma once
#include <System/SilkTypes.h>
#include <Math/Math.h>

namespace Silk
{
    class Camera
    {
        public:
            Camera(const Vec2& FoV, Scalar NearPlane, Scalar FarPlane) : //Perspective
                m_Transform(Mat4::Identity)
            {
                SetPerspective(FoV,NearPlane,FarPlane);
            }

            Camera(Scalar Left,Scalar Right,Scalar Top,Scalar Bottom, Scalar Near, Scalar Far) : //Ortho
                m_Transform(Mat4::Identity)
            {
                SetOrthographic(Left,Right,Top,Bottom,Near,Far);
            }

            Camera(Mat4 Projection) : m_Projection(Projection), m_Transform(Mat4::Identity) {}
            ~Camera() {}

            Mat4 GetTransform () { return m_Transform ; }
            Mat4 GetProjection();
        
            bool IsPerspective() const { return  m_IsPerspective; }
            bool IsOrthogonal () const { return !m_IsPerspective; }
        
            void SetTransform (Mat4 Transform) { m_Transform  = Transform ; }
        
            //Plane depth of -1 means use the ones already stored
            void SetPerspective (const Vec2& FoV       ,Scalar Near = -1,Scalar Far = -1);
            void SetOrthographic(const Vec2& Dimensions,Scalar Near = -1,Scalar Far = -1);
            void SetOrthographic(Scalar Left,Scalar Right,Scalar Top,Scalar Bottom,Scalar Near = -1,Scalar Far = -1);
        
            void SetFieldOfView(const Vec2& FoV);
            void SetZClipPlanes(Scalar Near,Scalar Far);
            void SetExposure   (Scalar Exposure)     { m_Exposure   = Exposure  ; }
            void SetFocalPoint (Scalar FocalPoint)   { m_FocalPoint = FocalPoint; }
            
            Vec2   GetFieldOfView() const { return m_FieldOfView; }
            Scalar GetExposure   () const { return m_Exposure   ; }
            Scalar GetNearPlane  () const { return m_NearPlane  ; }
            Scalar GetFarPlane   () const { return m_FarPlane   ; }
            Scalar GetFocalPoint () const { return m_FocalPoint ; }
        
        protected:
            bool   m_UpdateProjection;
            bool   m_IsPerspective   ;
        
            Mat4   m_Projection ;
            Mat4   m_Transform  ;
            Vec4   m_OrthoDims  ;
            Vec2   m_FieldOfView;
            Scalar m_Exposure   ;
            Scalar m_NearPlane  ;
            Scalar m_FarPlane   ;
            Scalar m_FocalPoint ;
    };
};

