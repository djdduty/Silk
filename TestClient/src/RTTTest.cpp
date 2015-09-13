#include <RTTTest.h>
#include <LodePNG.h>
#include <ObjLoader.h>
#include <math.h>
#include <Utilities/Utilities.h>

#include <System/TaskManager.h>

using namespace TestClient;
namespace TestClient
{
    RTTTest::RTTTest()
    {
    }
    RTTTest::~RTTTest()
    {
    }

    void RTTTest::Initialize()
    {
        InitGUI();
        InitFlyCamera();
		InitDebugDisplay();
        
        m_ShaderGenerator->Reset();
        m_ShaderGenerator->SetShaderVersion   (330);
        m_ShaderGenerator->SetAllowInstancing (false);
        
        m_ShaderGenerator->SetUniformInput    (ShaderGenerator::IUT_RENDERER_UNIFORMS,true);
        m_ShaderGenerator->SetUniformInput    (ShaderGenerator::IUT_MATERIAL_UNIFORMS,true);
        m_ShaderGenerator->SetUniformInput    (ShaderGenerator::IUT_OBJECT_UNIFORMS  ,true);
        
        m_ShaderGenerator->SetAttributeInput  (ShaderGenerator::IAT_POSITION,true);
        m_ShaderGenerator->SetFragmentOutput  (ShaderGenerator::OFT_COLOR   ,true);
        m_ShaderGenerator->SetAttributeOutput (ShaderGenerator::IAT_POSITION,true);
        
        m_ShaderGenerator->SetParallaxFunction(ShaderGenerator::PF_RELIEF);
        
        LoadMaterial();
        LoadLights  ();
        LoadMeshes  ();
        
        SetFPSPrintFrequency(0.5f);
    }
    void RTTTest::LoadLights()
    {
        Light* L = 0;
        
        L = AddLight(LT_POINT,Vec3(0,1,-5))->GetLight();
        L->m_Attenuation.Constant    = 0.00f;
        L->m_Attenuation.Linear      = 2.10f;
        L->m_Attenuation.Exponential = 5.90f;

        L = AddLight(LT_SPOT,Vec3(0,8,0))->GetLight();
        L->m_Color                   = Vec4(1,1,1,1);
        L->m_Power                   = 0.24;
        L->m_Cutoff                  = 45.0f;
        L->m_Attenuation.Constant    = 0.00f;
        L->m_Attenuation.Linear      = 0.10f;
        L->m_Attenuation.Exponential = 0.01f;
        
        L = AddLight(LT_DIRECTIONAL,Vec3(0,10,0))->GetLight();
        L->m_Color                   = Vec4(0.9,0.8,0.6,1);
        L->m_Power                   = 0.5f;
    }
    void RTTTest::LoadMeshes()
    {
        AddMesh("RTTTest/Scene.object",m_Materials[0],Vec3(0,0,0));
        i32 mid = AddMesh("RTTTest/LightDisplay.object",m_Materials[1],Vec3(0,0,0),m_Lights.size());
        for(i32 i = mid;i <= m_Lights.size();i++) m_LightMeshes.push_back(m_Meshes[i]);
        
        mid = AddMesh("RTTTest/Scene.object",m_Materials[2],Vec3(0,0,0));
        m_Meshes[mid]->SetTransform(Translation(Vec3(0,50, 50)) * Rotation(0,90,0));
        m_Meshes[mid]->SetTextureTransform(Scale(Vec3(-1.0f,1.0f,1.0f)));
        
        mid = AddMesh("RTTTest/Scene.object",m_Materials[2],Vec3(0,0,0));
        m_Meshes[mid]->SetTransform(Translation(Vec3(0,50,-50)) * Rotation(0,90,180));
        m_Meshes[mid]->SetTextureTransform(Scale(Vec3(-1.0f,1.0f,1.0f)));
        
        mid = AddMesh("RTTTest/Scene.object",m_Materials[2],Vec3(0,0,0));
        m_Meshes[mid]->SetTransform(Translation(Vec3( 50,50,0)) * Rotation(0,90,-90));
        m_Meshes[mid]->SetTextureTransform(Scale(Vec3(-1.0f,1.0f,1.0f)));
        
        mid = AddMesh("RTTTest/Scene.object",m_Materials[2],Vec3(0,0,0));
        m_Meshes[mid]->SetTransform(Translation(Vec3(-50,50,0)) * Rotation(0,90,90));
        m_Meshes[mid]->SetTextureTransform(Scale(Vec3(-1.0f,1.0f,1.0f)));
        
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    }
    void RTTTest::LoadMaterial()
    {
        //For ground
        Material* Mat = AddMaterial(ShaderGenerator::LM_PHONG,"RTTTest/GroundDiffuse.png",
                                                              "RTTTest/GroundNormal.png");
        Mat->SetShininess(1.0f);
        Mat->SetSpecular(Vec4(1,1,1,0));
        
        //For light displays
        AddMaterial(ShaderGenerator::LM_FLAT,"RTTTest/GroundDiffuse.png");
        
        //For pillar thing
        m_RTT = m_Renderer->GetRasterizer()->CreateTexture();
        m_RTT->CreateTexture(GetPreferredInitResolution().x,GetPreferredInitResolution().y);
        m_RTT->UpdateTexture();
        m_RTTIndex = m_Textures.size();
        m_Textures.push_back(m_RTT);
        
        AddMaterial(ShaderGenerator::LM_FLAT,m_RTT,0,0);
    }

    void RTTTest::Run()
    {
        Mat4 t = Translation(Vec3(0,4,9)) * RotationX(20.0f);
        m_Camera->SetTransform(t);
         
		m_Meshes[0]->SetTextureTransform(Scale(GROUND_TEXTURE_SCALE));
        
        Scalar a = 0.0f;
        m_TaskManager->GetTaskContainer()->SetAverageTaskDurationSampleCount(10);
        m_TaskManager->GetTaskContainer()->SetAverageThreadTimeDifferenceSampleCount(10);
    
        while(IsRunning())
        {
            a += GetDeltaTime();
            
            Mat4 r = Rotation(Vec3(0,1,0),a * 2.0f);
            //m_Meshes[0]->SetTransform(RotationZ(a));
            
            //m_Materials[0]->SetParallaxScale    (0.1f + (sin(a * 0.2) * 0.025f));
            
            //Point
            //m_Lights[0]->GetLight()->m_Attenuation.Exponential = 1.9f + (sin(a * 0.2f) * 0.5f);
            m_Lights[0]->GetLight()->m_Color = Vec4(ColorFunc(a * 0.1f),1.0f);
            m_Lights[0]->GetLight()->m_Power = 15.0f + (sin(a * 0.1f) * 5.0f);
            m_Lights[0]->SetTransform(Translation(Vec3(3,4 + (sin(a * 0.1f) * 3.0f),0)));
            
            //Spot
            //m_Lights[1]->SetTransform(Translation(Vec3(0,3,0.0f)) * Rotation(Vec3(1,0,0),-90.0f) * Rotation(Vec3(0,1,0),180.0f + (sin(a * 0.075f) * 95.0f)));
            //m_Lights[1]->GetLight()->m_Soften = (sin(a * 0.1f) * 0.5f) + 0.5f;
            
            //Directional
            m_Lights[2]->SetTransform(Rotation(Vec3(0,0,1),90 + (a * 7.5f)) * Rotation(Vec3(1,0,0),-90.0f));
            
            for(i32 i = 0;i < m_Lights.size();i++)
            {
                m_LightMeshes[i]->SetTransform(m_Lights[i]->GetTransform() * Scale(0.5f));
            }
            
            m_Renderer->RenderTexture(m_Textures[m_RTTIndex]);
        }
    }

    void RTTTest::Shutdown()
    {
    }
};
