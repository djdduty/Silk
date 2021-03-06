#include <CullingTest.h>
#include <LodePNG.h>
#include <ObjLoader.h>
#include <math.h>
#include <Utilities/Utilities.h>

#include <System/TaskManager.h>

using namespace TestClient;
namespace TestClient
{
    CullingTest::CullingTest()
    {
    }
    CullingTest::~CullingTest()
    {
    }

    void CullingTest::Initialize()
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
    void CullingTest::LoadLights()
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
    void CullingTest::LoadMeshes()
    {
        AddMesh("CullingTest/Scene.object",m_Materials[0],Vec3(0,0,0));
        i32 mid = AddMesh("CullingTest/LightDisplay.object",m_Materials[1],Vec3(0,0,0),m_Lights.size());
        for(i32 i = mid;i <= m_Lights.size();i++) m_LightMeshes.push_back(m_Meshes[i]);
        
        m_Meshes[AddMesh("CullingTest/LightDisplay.object",m_Materials[1],Vec3(0,0,100))]->SetTransform(Translation(Vec3(0,0,-100)) * Scale(10.0f));

		mid = AddMesh("CullingTest/CullObject.object",m_Materials[2],Vec3(0,2,0),NUM_OF_CULL_OBJECTS);
		for(i32 i = 0;i < NUM_OF_CULL_OBJECTS;i++)
		{
			m_Meshes[mid + i]->SetTransform(Translation(Vec3(Random(-SPAWN_CUBE_SIZE,SPAWN_CUBE_SIZE) * 0.5f,Random(0,SPAWN_CUBE_SIZE * 0.5f),Random(-SPAWN_CUBE_SIZE,SPAWN_CUBE_SIZE) * 0.5f)) * Scale(CULL_OBJECT_SCALE));
		}
        
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    }
    void CullingTest::LoadMaterial()
    {
        //For ground
        Material* Mat = AddMaterial(ShaderGenerator::LM_PHONG,"CullingTest/GroundDiffuse.png",
                                                              "CullingTest/GroundNormal.png");
        Mat->SetShininess(1.0f);
        Mat->SetSpecular(1.0f);
        
        Mat->SetMinParallaxLayers(MIN_PARALLAX_LAYERS);
        Mat->SetMaxParallaxLayers(MAX_PARALLAX_LAYERS);
        Mat->SetParallaxScale(0.01f);
        
        //For light displays
        AddMaterial(ShaderGenerator::LM_FLAT,"CullingTest/GroundDiffuse.png");
        
        //For cull objects
        AddMaterial(ShaderGenerator::LM_FLAT,"CullingTest/GroundDiffuse.png"); //Use instancing
        
        //AddMaterial(ShaderGenerator::LM_PHONG,"CullingTest/GroundDiffuse.png",
        //                                      "CullingTest/GroundNormal.png" ); //Non-instanced
    }

    void CullingTest::Run()
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
        }
    }

    void CullingTest::Shutdown()
    {
    }
};
