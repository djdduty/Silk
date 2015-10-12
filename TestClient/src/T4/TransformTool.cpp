/*
 *  TransformTool.cpp
 *  T4_Viewer
 *
 *  Created by Michael DeCicco on 10/9/15.
 *
 */

#include <T4/TransformTool.h>
#define BASE_OBJ_ID 10000

TransformTool::TransformTool(Test* Client)
{
    m_Scale = Vec3(1.0f,1.0f,1.0f);
    m_MinDispSize = 1.0f;
    m_MaterialX     = Client->GetRenderer()->CreateMaterial();
    m_MaterialY     = Client->GetRenderer()->CreateMaterial();
    m_MaterialZ     = Client->GetRenderer()->CreateMaterial();
    m_MaterialExtra = Client->GetRenderer()->CreateMaterial();
    ShaderGenerator* Gen = Client->GetRenderer()->GetShaderGenerator();
    
    Gen->Reset();
    Gen->SetShaderVersion   (330);
    Gen->SetAllowInstancing (false);
        
    Gen->SetUniformInput    (ShaderGenerator::IUT_RENDERER_UNIFORMS,true);
    Gen->SetUniformInput    (ShaderGenerator::IUT_MATERIAL_UNIFORMS,true);
    Gen->SetUniformInput    (ShaderGenerator::IUT_OBJECT_UNIFORMS  ,true);
        
    Gen->SetAttributeInput  (ShaderGenerator::IAT_POSITION,true);
    Gen->SetAttributeInput  (ShaderGenerator::IAT_NORMAL  ,true);
        
    Gen->SetAttributeOutput (ShaderGenerator::IAT_POSITION,true);
    Gen->SetAttributeOutput (ShaderGenerator::IAT_NORMAL  ,true);
    Gen->SetAttributeOutput (ShaderGenerator::IAT_COLOR   ,true);
        
    Gen->SetFragmentOutput  (ShaderGenerator::OFT_COLOR   ,true);
    Gen->SetFragmentOutput  (ShaderGenerator::OFT_POSITION,true);
    Gen->SetFragmentOutput  (ShaderGenerator::OFT_NORMAL  ,true);
    Gen->SetFragmentOutput  (ShaderGenerator::OFT_CUSTOM0 ,true);
        
        
    Gen->AddVertexModule  ("[PtSz]gl_PointSize = 5.0;[/PtSz]",0);
    Gen->AddVertexModule("[SetColor]o_Color = u_Diffuse;[/SetColor]",1);
    
    Gen->AddFragmentModule("[AlphaTest]if(sColor.a < 0.1) discard;[/AlphaTest]",1);
    Gen->AddFragmentModule(
    "[SetCustom0]"
    "int x = (u_ObjectID & int(0xff000000)) >> 24;\n"
    "int y = (u_ObjectID & int(0x00ff0000)) >> 16;\n"
    "int z = (u_ObjectID & int(0x0000ff00)) >>  8;\n"
    "int w = (u_ObjectID & int(0x000000ff)) >>  0;\n"
    "float Inv255 = 1.0 / 255.0;\n"
    "vec4 sCustom0 = vec4(float(w) * Inv255,float(z) * Inv255,float(y) * Inv255,float(x) * Inv255);\n"
    "[/SetCustom0]",1);
    
    Gen->SetLightingMode(ShaderGenerator::LM_PASS);
    
    m_MaterialX->SetShader(Gen->Generate());
    m_MaterialX->SetDiffuse(Vec4(1,0,0,1));
    m_MaterialY->SetShader(Gen->Generate());
    m_MaterialY->SetDiffuse(Vec4(0,1,0,1));
    m_MaterialZ->SetShader(Gen->Generate());
    m_MaterialZ->SetDiffuse(Vec4(0,0,1,1));
    m_MaterialExtra->SetShader(Gen->Generate());
    m_MaterialExtra->SetDiffuse(Vec4(1,1,0,1));
    
    m_TransX    = Client->LoadMesh("T4/TranslateX.object" ,m_MaterialX,Vec3(0,0,0));
    m_TransY    = Client->LoadMesh("T4/TranslateY.object" ,m_MaterialY,Vec3(0,0,0));
    m_TransZ    = Client->LoadMesh("T4/TranslateZ.object" ,m_MaterialZ,Vec3(0,0,0));
    m_TransXY   = Client->LoadMesh("T4/TranslateXY.object",m_MaterialExtra,Vec3(0,0,0));
    m_TransXZ   = Client->LoadMesh("T4/TranslateXZ.object",m_MaterialExtra,Vec3(0,0,0));
    m_TransZY   = Client->LoadMesh("T4/TranslateZY.object",m_MaterialExtra,Vec3(0,0,0));
    m_ScaleX    = Client->LoadMesh("T4/ScaleX.object"     ,m_MaterialX,Vec3(0,0,0));
    m_ScaleY    = Client->LoadMesh("T4/ScaleY.object"     ,m_MaterialY,Vec3(0,0,0));
    m_ScaleZ    = Client->LoadMesh("T4/ScaleZ.object"     ,m_MaterialZ,Vec3(0,0,0));
    m_ScaleAll  = Client->LoadMesh("T4/ScaleAll.object"   ,m_MaterialExtra,Vec3(0,0,0));
    m_RotX      = Client->LoadMesh("T4/RotateX.object"    ,m_MaterialX,Vec3(0,0,0));
    m_RotY      = Client->LoadMesh("T4/RotateY.object"    ,m_MaterialY,Vec3(0,0,0));
    m_RotZ      = Client->LoadMesh("T4/RotateZ.object"    ,m_MaterialZ,Vec3(0,0,0));
    
    m_TransX    ->SetEnabled(false);
    m_TransY    ->SetEnabled(false);
    m_TransZ    ->SetEnabled(false);
    m_TransXY   ->SetEnabled(false);
    m_TransXZ   ->SetEnabled(false);
    m_TransZY   ->SetEnabled(false);
    m_ScaleY    ->SetEnabled(false);
    m_ScaleX    ->SetEnabled(false);
    m_ScaleY    ->SetEnabled(false);
    m_ScaleZ    ->SetEnabled(false);
    m_ScaleAll  ->SetEnabled(false);
    m_RotX      ->SetEnabled(false);
    m_RotY      ->SetEnabled(false);
    m_RotZ      ->SetEnabled(false);
    
    m_TransX    ->SetObjectID(BASE_OBJ_ID +  0);
    m_TransY    ->SetObjectID(BASE_OBJ_ID +  1);
    m_TransZ    ->SetObjectID(BASE_OBJ_ID +  2);
    m_TransXY   ->SetObjectID(BASE_OBJ_ID +  3);
    m_TransXZ   ->SetObjectID(BASE_OBJ_ID +  4);
    m_TransZY   ->SetObjectID(BASE_OBJ_ID +  5);
    m_ScaleX    ->SetObjectID(BASE_OBJ_ID +  6);
    m_ScaleY    ->SetObjectID(BASE_OBJ_ID +  7);
    m_ScaleZ    ->SetObjectID(BASE_OBJ_ID +  8);
    m_ScaleAll  ->SetObjectID(BASE_OBJ_ID +  9);
    m_RotX      ->SetObjectID(BASE_OBJ_ID + 10);
    m_RotY      ->SetObjectID(BASE_OBJ_ID + 11);
    m_RotZ      ->SetObjectID(BASE_OBJ_ID + 12);
    
    m_IsUsingTransX   = m_IsUsingTransY  = m_IsUsingTransZ  =
    m_IsUsingTransXY  = m_IsUsingTransXZ = m_IsUsingTransZY =
    m_IsUsingScaleAll = m_IsUsingScaleX  = m_IsUsingScaleY  = m_IsUsingScaleZ =
    m_IsUsingRotX     = m_IsUsingRotY    = m_IsUsingRotZ    = false;
    
    m_TranslationEnabled = m_RotationEnabled = m_ScaleEnabled = false;
    
    m_Input    = Client->GetInput();
    m_Camera   = Client->GetCamera();
    m_Renderer = Client->GetRenderer();
}
TransformTool::~TransformTool()
{
}

bool TransformTool::OnObjClick(i32 ObjIdx)
{
    if(ObjIdx < BASE_OBJ_ID || ObjIdx > BASE_OBJ_ID + 12) return false;
    i32 Idx = ObjIdx - BASE_OBJ_ID;
    
    m_IsUsingTransX   = m_IsUsingTransY  = m_IsUsingTransZ  =
    m_IsUsingTransXY  = m_IsUsingTransXZ = m_IsUsingTransZY =
    m_IsUsingScaleAll = m_IsUsingScaleX  = m_IsUsingScaleY  = m_IsUsingScaleZ =
    m_IsUsingRotX     = m_IsUsingRotY    = m_IsUsingRotZ    = false;
    
    switch(Idx)
    {
        case 0 : { m_IsUsingTransX   = true; break; }
        case 1 : { m_IsUsingTransY   = true; break; }
        case 2 : { m_IsUsingTransZ   = true; break; }
        case 3 : { m_IsUsingTransXY  = true; break; }
        case 4 : { m_IsUsingTransXZ  = true; break; }
        case 5 : { m_IsUsingTransZY  = true; break; }
        case 6 : { m_IsUsingScaleAll = true; break; }
        case 7 : { m_IsUsingScaleX   = true; break; }
        case 8 : { m_IsUsingScaleY   = true; break; }
        case 9 : { m_IsUsingScaleZ   = true; break; }
        case 10: { m_IsUsingRotX     = true; break; }
        case 11: { m_IsUsingRotY     = true; break; }
        case 12: { m_IsUsingRotZ     = true; break; }
    }
    
    m_LastIntersection = ComputeIntersectionPoint(true);
    
    return true;
}
void TransformTool::OnCursorMove()
{
    if(!m_TranslationEnabled && !m_RotationEnabled && !m_ScaleEnabled) return;
    
    Vec2 cPos = m_Input->GetCursorPosition();
    
    Vec4 Viewport = m_Renderer->GetRasterizer()->GetViewport();
    Mat4 v = m_Camera->GetTransform();
    Mat4 p = m_Camera->GetProjection();
    Ray CursorRay = UnProject(Vec3(cPos,0),v,p,Vec4(Viewport[0],Viewport[1],Viewport[2],Viewport[3]));
    
    Vec3 LastLastIntersection = m_LastIntersection;
    m_LastIntersection = ComputeIntersectionPoint(false);

    if(m_IsUsingTransX  || m_IsUsingTransY  || m_IsUsingTransZ
    || m_IsUsingTransXY || m_IsUsingTransXZ || m_IsUsingTransZY)
    {
        Vec3 Delta = m_LastIntersection - LastLastIntersection;
        m_Position += Delta;
        Update();
    }
    
    if(m_IsUsingRotX || m_IsUsingRotY || m_IsUsingRotZ)
    {
        Vec3 a = m_LastIntersection;
        Vec3 b = LastLastIntersection;
        Vec3 cross = a.Cross(b);
        if(cross.MagnitudeSq() != 0.0f)
        {
            m_Renderer->GetDebugDrawer()->Line(m_TempAxis.Point,m_TempAxis.Point + a,Vec4(1,0,0,1));
            m_Renderer->GetDebugDrawer()->Line(m_TempAxis.Point,m_TempAxis.Point + b,Vec4(0,0,1,1));
            m_Renderer->GetDebugDrawer()->Line(m_TempAxis.Point,m_TempAxis.Point + cross,Vec4(0,1,0,1));
            
            Scalar angle = acosf(a.Dot(b)) * PI_UNDER_180;
            if(m_TempAxis.Dir.Dot(cross) > 0)
            {
                angle = -angle;
            }
            
            m_Rotation *= Quat(m_TempAxis.Dir,angle);
            
            Update();
        }
    }
}

void TransformTool::OnClickUp()
{
    m_IsUsingTransX   = m_IsUsingTransY  = m_IsUsingTransZ  =
    m_IsUsingTransXY  = m_IsUsingTransXZ = m_IsUsingTransZY =
    m_IsUsingScaleAll = m_IsUsingScaleX  = m_IsUsingScaleY  = m_IsUsingScaleZ =
    m_IsUsingRotX     = m_IsUsingRotY    = m_IsUsingRotZ    = false;
}

void TransformTool::ToggleTranslate()
{
    m_TranslationEnabled = !m_TranslationEnabled;
    m_TransX->SetEnabled(m_TranslationEnabled);
    m_TransY->SetEnabled(m_TranslationEnabled);
    m_TransZ->SetEnabled(m_TranslationEnabled);
    m_TransXY->SetEnabled(m_TranslationEnabled);
    m_TransXZ->SetEnabled(m_TranslationEnabled);
    m_TransZY->SetEnabled(m_TranslationEnabled);
}
void TransformTool::ToggleRotate()
{
    m_RotationEnabled = !m_RotationEnabled;
    m_RotX->SetEnabled(m_RotationEnabled);
    m_RotY->SetEnabled(m_RotationEnabled);
    m_RotZ->SetEnabled(m_RotationEnabled);
}
void TransformTool::ToggleScale()
{
    m_ScaleEnabled = !m_ScaleEnabled;
    m_ScaleX  ->SetEnabled(m_ScaleEnabled);
    m_ScaleY  ->SetEnabled(m_ScaleEnabled);
    m_ScaleZ  ->SetEnabled(m_ScaleEnabled);
    m_ScaleAll->SetEnabled(m_ScaleEnabled);
}

void TransformTool::SetTransform(const Mat4& m)
{
    m_Position = m.GetTranslation();
    m_Rotation = m.GetRotation();
    m.DebugPrint();
    m_Scale    = m.GetScale();
    Update();
}
Mat4 TransformTool::GetTransform() const
{
    return Translation(m_Position) * m_Rotation.ToMat() * Scale(m_Scale);
}
void TransformTool::Update()
{
    Mat4 TransMat = Translation(m_Position) * Scale(max(m_DispScale,m_MinDispSize));
    m_TransX    ->SetTransform(TransMat);
    m_TransY    ->SetTransform(TransMat);
    m_TransZ    ->SetTransform(TransMat);
    m_TransXY   ->SetTransform(TransMat);
    m_TransXZ   ->SetTransform(TransMat);
    m_TransZY   ->SetTransform(TransMat);

    Mat4 RotMat = Translation(m_Position) * m_Rotation.ToMat() * Scale(max(m_DispScale,m_MinDispSize));
    m_ScaleX    ->SetTransform(RotMat);
    m_ScaleY    ->SetTransform(RotMat);
    m_ScaleZ    ->SetTransform(RotMat);
    m_ScaleAll  ->SetTransform(RotMat);
    
    m_RotX      ->SetTransform(RotMat);
    m_RotY      ->SetTransform(RotMat);
    m_RotZ      ->SetTransform(RotMat);
}
Vec3 TransformTool::ComputeIntersectionPoint(bool OverwriteTempAxis)
{
    Vec2 cPos = m_Input->GetCursorPosition();
    
    Vec4 Viewport = m_Renderer->GetRasterizer()->GetViewport();
    Mat4 v = m_Camera->GetTransform();
    Mat4 p = m_Camera->GetProjection();
    Ray CursorRay = UnProject(Vec3(cPos,0),v,p,Vec4(Viewport[0],Viewport[1],Viewport[2],Viewport[3]));
    
    if(m_IsUsingTransX)
    {
        Vec3 o0,o1;
        if(OverwriteTempAxis)
        {
            Mat4 t = m_TransX->GetTransform();
            m_TempAxis.Point = t.GetTranslation();
            m_TempAxis.Dir   = t.GetX().Normalized();
        }
        ClosestPointsBetweenRays(CursorRay,m_TempAxis,o0,o1);
        
        m_Renderer->GetDebugDrawer()->Line(o0,o1,Vec4(1,1,1,1));
        
        return o1;
    }
    else if(m_IsUsingTransY)
    {
        Vec3 o0,o1;
        if(OverwriteTempAxis)
        {
            Mat4 t = m_TransY->GetTransform();
            m_TempAxis.Point = t.GetTranslation();
            m_TempAxis.Dir   = t.GetY().Normalized();
        }
        ClosestPointsBetweenRays(CursorRay,m_TempAxis,o0,o1);
        
        m_Renderer->GetDebugDrawer()->Line(o0,o1,Vec4(1,1,1,1));
        
        return o1;
    }
    else if(m_IsUsingTransZ)
    {
        Vec3 o0,o1;
        if(OverwriteTempAxis)
        {
            Mat4 t = m_TransZ->GetTransform();
            m_TempAxis.Point = t.GetTranslation();
            m_TempAxis.Dir   = t.GetZ().Normalized();
        }
        ClosestPointsBetweenRays(CursorRay,m_TempAxis,o0,o1);
        
        m_Renderer->GetDebugDrawer()->Line(o0,o1,Vec4(1,1,1,1));
        
        return o1;
    }
    else if(m_IsUsingTransXY)
    {
        if(OverwriteTempAxis)
        {
            Mat4 t = m_TransXY->GetTransform();
            m_TempAxis.Point = t.GetTranslation();
            m_TempAxis.Dir   = t.GetZ();
        }
        Plane p = Plane(m_TempAxis.Point,m_TempAxis.Dir);
        Vec3 Pt = p.GetIntersectionPoint(CursorRay);
        return Pt;
    }
    else if(m_IsUsingTransXZ)
    {
        if(OverwriteTempAxis)
        {
            Mat4 t = m_TransXZ->GetTransform();
            m_TempAxis.Point = t.GetTranslation();
            m_TempAxis.Dir   = t.GetY();
        }
        Plane p = Plane(m_TempAxis.Point,m_TempAxis.Dir);
        Vec3 Pt = p.GetIntersectionPoint(CursorRay);
        return Pt;
    }
    else if(m_IsUsingTransZY)
    {
        if(OverwriteTempAxis)
        {
            Mat4 t = m_TransZY->GetTransform();
            m_TempAxis.Point = t.GetTranslation();
            m_TempAxis.Dir   = t.GetX();
        }
        Plane p = Plane(m_TempAxis.Point,m_TempAxis.Dir);
        Vec3 Pt = p.GetIntersectionPoint(CursorRay);
        return Pt;
    }
    else if(m_IsUsingRotX)
    {
        if(OverwriteTempAxis)
        {
            Mat4 t = m_Rotation.ToMat();
            m_TempAxis.Dir   = m_Rotation * Vec3(1,0,0);
            m_TempAxis.Point = m_Position + ((m_TempAxis.Dir).Normalized() * 1.288f * max(m_DispScale,m_MinDispSize));
        }
        Plane p = Plane(m_TempAxis.Point,(m_TempAxis.Dir).Normalized());
        Vec3 Pt = p.GetIntersectionPoint(CursorRay);
        return (Pt - m_TempAxis.Point).Normalized();
    }
    else if(m_IsUsingRotY)
    {
        if(OverwriteTempAxis)
        {
            Mat4 t = m_Rotation.ToMat();
            m_TempAxis.Dir   = m_Rotation * Vec3(0,1,0);
            m_TempAxis.Point = m_Position + ((m_TempAxis.Dir).Normalized() * 1.288f * max(m_DispScale,m_MinDispSize));
        }
        Plane p = Plane(m_TempAxis.Point,(m_TempAxis.Dir).Normalized());
        Vec3 Pt = p.GetIntersectionPoint(CursorRay);
        return (Pt - m_TempAxis.Point).Normalized();
    }
    else if(m_IsUsingRotZ)
    {
        if(OverwriteTempAxis)
        {
            Mat4 t = m_Rotation.ToMat();
            m_TempAxis.Dir   = m_Rotation * Vec3(0,0,1);
            m_TempAxis.Point = m_Position + ((m_TempAxis.Dir).Normalized() * 1.288f * max(m_DispScale,m_MinDispSize));
        }
        Plane p = Plane(m_TempAxis.Point,(m_TempAxis.Dir).Normalized());
        Vec3 Pt = p.GetIntersectionPoint(CursorRay);
        return (Pt - m_TempAxis.Point).Normalized();
    }
    
    return Vec3(0,0,0);
}