#pragma once
#include <System/SilkTypes.h>
#include <Math/Math.h>
#include <algorithm>

namespace Silk
{
    class Camera
    {
        public:
            Camera(const Vec2& FoV, Scalar NearPlane, Scalar FarPlane) : //Perspective
                m_Transform(Mat4::Identity), m_InversedTransform(Mat4::Identity)
            {
                SetPerspective(FoV,NearPlane,FarPlane);
            }

            Camera(Scalar Left,Scalar Right,Scalar Top,Scalar Bottom, Scalar Near, Scalar Far) : //Ortho
                m_Transform(Mat4::Identity), m_InversedTransform(Mat4::Identity)
            {
                SetOrthographic(Left,Right,Top,Bottom,Near,Far);
            }

            Camera(Mat4 Projection) : m_Projection(Projection), m_Transform(Mat4::Identity), m_InversedTransform(Mat4::Identity) {}
            ~Camera() {}

            const Mat4& GetTransform () { return m_Transform ; }
            const Mat4& GetProjection();
        
            bool IsPerspective() const { return  m_IsPerspective; }
            bool IsOrthogonal () const { return !m_IsPerspective; }
        
            void SetTransform (Mat4 Transform) { m_Transform  = Transform ; m_TrnsChanged = true; m_InversedTransform = m_Transform.Inverse(); }
            const Mat4& GetInverseTransform() const { return m_InversedTransform; }
        
            //Plane depth of -1 means use the ones already stored
            void SetPerspective (const Vec2& FoV       ,Scalar Near = -1,Scalar Far = -1);
            void SetOrthographic(const Vec2& Dimensions,Scalar Near = -1,Scalar Far = -1);
            void SetOrthographic(Scalar Left,Scalar Right,Scalar Top,Scalar Bottom,Scalar Near = -1,Scalar Far = -1);
        
            void SetFieldOfView(const Vec2& FoV);
            void SetZClipPlanes(Scalar Near,Scalar Far);
            void SetExposure   (Scalar Exposure)     { m_Exposure   = Exposure  ; m_ExpoChanged = true; }
            void SetFocalPoint (Scalar FocalPoint)   { m_FocalPoint = FocalPoint; m_FcPtChanged = true; }
        
            bool DidProjectionUpdate () { if(m_ProjChanged) { m_ProjChanged = false; return true; }  return false; }
            bool DidTransformUpdate  () { if(m_TrnsChanged) { m_TrnsChanged = false; return true; }  return false; }
            bool DidOrthoDimsUpdate  () { if(m_OrDmChanged) { m_OrDmChanged = false; return true; }  return false; }
            bool DidFieldOfViewUpdate() { if(m_FdVwChanged) { m_FdVwChanged = false; return true; }  return false; }
            bool DidExposureUpdate   () { if(m_ExpoChanged) { m_ExpoChanged = false; return true; }  return false; }
            bool DidNearPlaneUpdate  () { if(m_nPlnChanged) { m_nPlnChanged = false; return true; }  return false; }
            bool DidFarPlaneUpdate   () { if(m_fPlnChanged) { m_fPlnChanged = false; return true; }  return false; }
            bool DidFocalPointUpdate () { if(m_FcPtChanged) { m_FcPtChanged = false; return true; }  return false; }
            
            Vec2   GetFieldOfView() const { return m_FieldOfView; }
            Scalar GetExposure   () const { return m_Exposure   ; }
            Scalar GetNearPlane  () const { return m_NearPlane  ; }
            Scalar GetFarPlane   () const { return m_FarPlane   ; }
            Scalar GetFocalPoint () const { return m_FocalPoint ; }
        
        protected:
            bool   m_UpdateProjection ;
            bool   m_IsPerspective    ;
        
            Mat4   m_InversedTransform;
        
            Mat4   m_Projection ;
            bool   m_ProjChanged;
            Mat4   m_Transform  ;
            bool   m_TrnsChanged;
            Vec4   m_OrthoDims  ;
            bool   m_OrDmChanged;
            Vec2   m_FieldOfView;
            bool   m_FdVwChanged;
            Scalar m_Exposure   ;
            bool   m_ExpoChanged;
            Scalar m_NearPlane  ;
            bool   m_nPlnChanged;
            Scalar m_FarPlane   ;
            bool   m_fPlnChanged;
            Scalar m_FocalPoint ;
            bool   m_FcPtChanged;
    };
};

