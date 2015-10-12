/*
 *  TransformTool.h
 *  T4_Viewer
 *
 *  Created by Michael DeCicco on 10/9/15.
 *
 */

#pragma once
#include <Silk.h>
using namespace Silk;

#include <Test.h>
using namespace TestClient;

class TransformTool
{
    public:
        TransformTool(Test* Client);
        ~TransformTool();
    
        //Returns false if ObjIdx does not match the object index of any of the display objects
        bool OnObjClick(i32 ObjIdx);
        void OnCursorMove();
        void OnClickUp();
    
        void ToggleTranslate();
        void ToggleRotate   ();
        void ToggleScale    ();
    
        bool IsEnabled() const { return m_TranslationEnabled || m_RotationEnabled || m_ScaleEnabled; }
    
        void SetTransform(const Mat4& m);
        Mat4 GetTransform() const;
        Vec3 GetPosition() const { return m_Position; }
        Quat GetRotation() const { return m_Rotation; }
        Vec3 GetScale   () const { return m_Scale   ; }
    
        void SetDisplayScale(Scalar Sc) { m_DispScale = Sc; Update(); }
        void SetMinDisplaySize(Scalar Sz) { m_MinDispSize = Sz; Update(); }
    
        void Update();
    
        Vec3 ComputeIntersectionPoint(bool OverwriteTempAxis);
    
    protected:
        InputManager* m_Input;
        Camera* m_Camera;
        Renderer* m_Renderer;
        Scalar m_MinDispSize;
        Scalar m_DispScale;
    
        Vec3 m_Position;
        Vec3 m_Scale;
        Quat m_Rotation;
    
        Vec3 m_LastIntersection;
        Ray m_TempAxis;
    
        bool m_IsUsingTransX;
        bool m_IsUsingTransY;
        bool m_IsUsingTransZ;
        bool m_IsUsingTransXY;
        bool m_IsUsingTransXZ;
        bool m_IsUsingTransZY;
        bool m_IsUsingScaleAll;
        bool m_IsUsingScaleX;
        bool m_IsUsingScaleY;
        bool m_IsUsingScaleZ;
        bool m_IsUsingRotX;
        bool m_IsUsingRotY;
        bool m_IsUsingRotZ;
    
        bool m_TranslationEnabled;
        bool m_RotationEnabled;
        bool m_ScaleEnabled;
    
        Material* m_MaterialX;
        Material* m_MaterialY;
        Material* m_MaterialZ;
        Material* m_MaterialExtra;
        RenderObject* m_TransX;
        RenderObject* m_TransY;
        RenderObject* m_TransZ;
        RenderObject* m_TransXY;
        RenderObject* m_TransXZ;
        RenderObject* m_TransZY;
        RenderObject* m_ScaleAll;
        RenderObject* m_ScaleX;
        RenderObject* m_ScaleY;
        RenderObject* m_ScaleZ;
        RenderObject* m_RotX;
        RenderObject* m_RotY;
        RenderObject* m_RotZ;
};
