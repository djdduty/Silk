#include <SSAOTest.h>
#include <LodePNG.h>
#include <ObjLoader.h>
#include <math.h>

namespace TestClient
{
    SSAOTest::SSAOTest()
    {
    }
    SSAOTest::~SSAOTest()
    {
    }

    void SSAOTest::Initialize()
    {
        InitGUI();
		InitRenderGUI();
        InitFlyCamera();

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
        //InitDebugDisplay();
        
        m_Camera->SetZClipPlanes(0.001f,200.0f);
        m_CamPos = Vec3(0,60,20);

        SetFPSPrintFrequency(10.0f);
        
        InitSSAO();
        SetSSAORadius    (0.5f);
        SetSSAOIntensity (1.0f);
        SetSSAONoiseScale(4);
    }
    void SSAOTest::LoadLight()
    {
        Light* L = 0;
        
        i32 lc = 10;
        for(i32 i = 0;i < lc;i++)
        {
            Scalar Fraction = Scalar(i) * (1.0f / Scalar(lc));
            Scalar th = Fraction * (2.0f * PI);
            L = AddLight(LT_POINT,Vec3(cos(th) * 20.0f,70,sin(th) * 20.0f))->GetLight();
            L->m_Color                   = Vec4(ColorFunc(Fraction),1.0);//Vec4(1,1,1,1);
            L->m_Power                   = 1.2f;
            L->m_Attenuation.Constant    = 0.00f;
            L->m_Attenuation.Linear      = 0.10f;
            L->m_Attenuation.Exponential = 0.001f;
        }
    }
    void SSAOTest::LoadMesh()
    {
        m_Meshes[AddMesh("SSAOTest/Scene.object",m_Materials[0],Vec3(0,0,0))]->SetTransform(Scale(10.0f));

        DeferredRenderer* r = (DeferredRenderer*)m_Renderer;
        RenderObject* Point = m_Meshes[AddMesh("Silk/PointLight.object",r->GetPointLightMaterial(),Vec3(0,0,0))];
        RenderObject* Spot  = m_Meshes[AddMesh("Silk/SpotLight.object" ,r->GetSpotLightMaterial (),Vec3(0,0,0))];
        
        r->SetPointLightObject(Point);
        r->SetSpotLightObject(Spot);
        
        Point->SetEnabled(false);
        Spot->SetEnabled(false);
        
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    }
    void SSAOTest::LoadMaterial()
    {
        //For ground
        Material* Mat = AddMaterial(ShaderGenerator::LM_PASS,"SSAOTest/Scene.png");
        
        Mat->SetShininess(0.9f);
        Mat->SetSpecular(0.5f);

        //For light displays
        AddMaterial(ShaderGenerator::LM_FLAT,"SSAOTest/Scene.png");
        
        //Light pass materials
        DeferredRenderer* r = (DeferredRenderer*)m_Renderer;
        
        Material* Pt = m_Renderer->CreateMaterial();
        Pt->LoadMaterial(Load("Silk/PointLight.mtrl"));
        Material* Sp = m_Renderer->CreateMaterial();
        Sp->LoadMaterial(Load("Silk/SpotLight.mtrl"));
		Material* Dr = m_Renderer->CreateMaterial();
        Dr->LoadMaterial(Load("Silk/DirectionalLight.mtrl"));
        m_Final = m_Renderer->CreateMaterial();
        m_Final->LoadMaterial(Load("Silk/FinalDeferredPass.mtrl"));
		m_NoFxaa = m_Renderer->CreateMaterial();
        m_NoFxaa->LoadMaterial(Load("Silk/FinalDeferredPassNoFxaa.mtrl"));
        
        r->SetPointLightMaterial(Pt);
        r->SetSpotLightMaterial(Sp);
        r->SetDirectionalLightMaterial(Dr);
        r->SetFinalPassMaterial(m_Final);
    }
    
    void SSAOTest::Run()
    {
        Mat4 t = Translation(Vec3(0,4,9)) * RotationX(20.0f);
        m_Camera->SetTransform(t);

        m_Meshes[0]->SetTextureTransform(Scale(25.0f));

        Scalar a = 0.0f;
        m_TaskManager->GetTaskContainer()->SetAverageTaskDurationSampleCount(10);
        m_TaskManager->GetTaskContainer()->SetAverageThreadTimeDifferenceSampleCount(10);
        
        Vec3 OscillationSpeedMultiplier = Vec3(0.25f,0.5f,0.5f) * 0.1f;
        Vec3 OscillationBase  = Vec3( 0,40, 0);
        Vec3 OscillationRange = Vec3(40,40,40);
        
        while(IsRunning())
        {
			if(m_InputManager->IsButtonDown(BTN_LEFT_MOUSE))
			{
				//((DeferredRenderer*)m_Renderer)->SetFinalPassMaterial(m_NoFxaa);
				m_Renderer->SetUsePostProcessing(false);
			} else {
				//((DeferredRenderer*)m_Renderer)->SetFinalPassMaterial(m_Final);
				m_Renderer->SetUsePostProcessing(true);
			}
            
            a += GetDeltaTime();
            
            /*
            vector<Vec3> SSAOKernel;
            for(i32 i = 0;i < SSAO_KERNEL_SIZE;i++)
            {
                SSAOKernel.push_back(RandomVec(1.0f));
                SSAOKernel[i].z = abs(SSAOKernel[i].z);
            }
            
            SSAOInputs->SetUniform(0,SSAOKernel      );
            SSAOInputs->SetUniform(1,SSAO_KERNEL_SIZE);
            */
            
            //SSAOInputs->SetUniform(2,(sin(a) * 0.5f) + 0.5f);
            //SSAOInputs->SetUniform(3,(sin(a) * 0.5f) + 1.0f);
            
        }
    }

    void SSAOTest::Shutdown()
    {
    }
};
