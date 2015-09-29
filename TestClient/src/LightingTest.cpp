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

        SetFPSPrintFrequency(0.5f);
    }
    void LightingTest::LoadLight()
    {
        Light* L = 0;
        
        for(i32 x = -5; x < 5; x++)
        {
            for(i32 z = -5; z < 5; z++)
            {
                f32 randr = rand() % 101;
                randr *= 0.01;
                f32 randg = rand() % 101;
                randg *= 0.01;
                f32 randb = rand() % 101;
                randb *= 0.01;
                L = AddLight(LT_POINT,Vec3(x*10+5,5,z*10-10))->GetLight();
                L->m_Color                   = Vec4(randr,randg,randb,1);
                L->m_Power                   = 1.5f;
                L->m_Attenuation.Constant    = 0.00f;
                L->m_Attenuation.Linear      = 0.10f;
                L->m_Attenuation.Exponential = 0.05f;
            }
        }
    }
    void LightingTest::LoadMesh()
    {
        m_Meshes[AddMesh("LightingTest/Scene.object",m_Materials[0],Vec3(0,0,0))]->SetTransform(Scale(10.0f));

        DeferredRenderer* r = (DeferredRenderer*)m_Renderer;
        RenderObject* Point = m_Meshes[AddMesh("Silk/bsphere.object",r->GetPointLightMaterial(),Vec3(0,0,0))];
        RenderObject* Spot  = m_Meshes[AddMesh("Silk/SpotLight.object" ,r->GetSpotLightMaterial (),Vec3(0,0,0))];
        
        r->SetPointLightObject(Point);
        r->SetSpotLightObject(Spot);
        
        Point->SetEnabled(false);
        Spot->SetEnabled(false);
        
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    }
    void LightingTest::LoadMaterial()
    {
        //For ground
        Material* Mat = AddMaterial(ShaderGenerator::LM_PASS,"Common/GroundDiffuse.png","Common/GroundNormal.png");
        
        Mat->SetShininess(0.1f);
        Mat->SetSpecular(0.3f);

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
        m_Final = m_Renderer->CreateMaterial();
        m_Final->LoadMaterial(Load("Silk/FinalDeferredPass.mtrl"));
		m_NoFxaa = m_Renderer->CreateMaterial();
        m_NoFxaa->LoadMaterial(Load("Silk/FinalDeferredPassNoFxaa.mtrl"));
        
        r->SetPointLightMaterial(Pt);
        r->SetSpotLightMaterial(Sp);
        r->SetDirectionalLightMaterial(Dr);
        r->SetFinalPassMaterial(m_Final);
    }

    void LightingTest::Run()
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
				((DeferredRenderer*)m_Renderer)->SetFinalPassMaterial(m_NoFxaa);
			else
				((DeferredRenderer*)m_Renderer)->SetFinalPassMaterial(m_Final);
            
            a += GetDeltaTime();
            //m_Lights[0]->GetLight()->m_Color = Vec4(ColorFunc(a),1.0f);
            //m_Lights[0]->GetLight()->m_Power = 8.0f + (sin(a) * 5.0f);
            
            //m_Lights[0]->SetTransform(Translation(Vec3(OscillationBase.x + (OscillationRange.x * cos(a * OscillationSpeedMultiplier.x)),
                                                       //OscillationBase.y + (OscillationRange.y * sin(a * OscillationSpeedMultiplier.y)),
                                                       //OscillationBase.z + (OscillationRange.z * cos(a * OscillationSpeedMultiplier.z)))));
            
            m_Lights[2]->SetTransform(Translation(Vec3(0,25,0)) * Rotation(Vec3(1,0,0),a * 15.0f));
        }
    }

    void LightingTest::Shutdown()
    {
    }
};
