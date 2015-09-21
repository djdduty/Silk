#include <LightingTest.h>
#include <LodePNG.h>
#include <ObjLoader.h>
#include <math.h>

namespace TestClient
{
    LightingTest::LightingTest()
    {
    }
    LightingTest::~LightingTest()
    {
    }

    void LightingTest::Initialize()
    {
        InitGUI();
        InitFlyCamera();
        //InitDebugDisplay();

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
        LoadLight   ();
        LoadMesh    ();

        SetFPSPrintFrequency(0.5f);
    }
    void LightingTest::LoadLight()
    {
        Light* L = 0;

        L = AddLight(LT_POINT,Vec3(0,1,-5))->GetLight();
        L->m_Attenuation.Constant    = 0.00f;
        L->m_Attenuation.Linear      = 2.10f;
        L->m_Attenuation.Exponential = 5.90f;
        
        /*
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
        */
    }
    void LightingTest::LoadMesh()
    {
        m_Meshes[AddMesh("LightingTest/Scene.object",m_Materials[0],Vec3(0,0,0))]->SetTransform(Scale(10.0f));
        
        i32 mid = AddMesh("LightingTest/LightDisplay.object",m_Materials[0],Vec3(0,0,0),m_Lights.size());
        for(i32 i = mid;i <= m_Lights.size();i++) m_LightMeshes.push_back(m_Meshes[i]);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    }
    void LightingTest::LoadMaterial()
    {
        //For ground
        Material* Mat = AddMaterial(ShaderGenerator::LM_PASS,"Common/GroundDiffuse.png",
                                                             "Common/GroundNormal.png");
        Mat->SetShininess(1.0f);
        Mat->SetSpecular(1.0f);

        //For light displays
        AddMaterial(ShaderGenerator::LM_FLAT,"Common/GroundDiffuse.png");
        
        //Light pass materials
        DeferredRenderer* r = (DeferredRenderer*)m_Renderer;
        
        Material* Pt = m_Renderer->CreateMaterial();
        Pt->LoadMaterial(Load("Silk/PointLight.mtrl"));
        Material* Sp = m_Renderer->CreateMaterial();
        Sp->LoadMaterial(Load("Silk/SpotLight.mtrl"));
        Material* Dr = m_Renderer->CreateMaterial();
        Dr->LoadMaterial(Load("Silk/DirectionalLight.mtrl"));
        
        r->SetPointLightMaterial(Pt);
        r->SetSpotLightMaterial(Sp);
        r->SetDirectionalLightMaterial(Dr);
    }

    void LightingTest::Run()
    {
        Mat4 t = Translation(Vec3(0,4,9)) * RotationX(20.0f);
        m_Camera->SetTransform(t);

        m_Meshes[0]->SetTextureTransform(Scale(25.0f));

        Scalar a = 0.0f;
        m_TaskManager->GetTaskContainer()->SetAverageTaskDurationSampleCount(10);
        m_TaskManager->GetTaskContainer()->SetAverageThreadTimeDifferenceSampleCount(10);


        while(IsRunning())
        {
            //a += GetDeltaTime();
        }
    }

    void LightingTest::Shutdown()
    {
    }
};
