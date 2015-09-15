#include <PostProcessingTest.h>
#include <LodePNG.h>
#include <ObjLoader.h>
#include <math.h>
#include <Utilities/Utilities.h>

#include <System/TaskManager.h>

using namespace TestClient;
namespace TestClient
{
    PostProcessingTest::PostProcessingTest()
    {
    }
    PostProcessingTest::~PostProcessingTest()
    {
    }

    void PostProcessingTest::Initialize()
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
        
        /*
        PostProcessingEffect* Effect = new PostProcessingEffect(m_Renderer);
        Effect->LoadEffect(Load("Common/SMAA.ppe"));
        m_Renderer->SetUsePostProcessing(true);
        m_Renderer->AddPostProcessingEffect(Effect);
        */
    }
    void PostProcessingTest::LoadLights()
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
    void PostProcessingTest::LoadMeshes()
    {
        AddMesh("PostProcessingTest/Scene.object",m_Materials[0],Vec3(0,0,0));
        i32 mid = AddMesh("PostProcessingTest/LightDisplay.object",m_Materials[1],Vec3(0,0,0),m_Lights.size());
        for(i32 i = mid;i <= m_Lights.size();i++) m_LightMeshes.push_back(m_Meshes[i]);
        
        mid = AddMesh("RTTTest/Scene.object",m_Materials[0],Vec3(0,0,0));
        m_Meshes[mid]->SetTransform(Translation(Vec3(0,50, 50)) * Rotation(0,90,0));
        
        mid = AddMesh("RTTTest/Scene.object",m_Materials[0],Vec3(0,0,0));
        m_Meshes[mid]->SetTransform(Translation(Vec3(0,50,-50)) * Rotation(0,90,180));
        
        mid = AddMesh("RTTTest/Scene.object",m_Materials[0],Vec3(0,0,0));
        m_Meshes[mid]->SetTransform(Translation(Vec3( 50,50,0)) * Rotation(0,90,-90));
        
        mid = AddMesh("RTTTest/Scene.object",m_Materials[0],Vec3(0,0,0));
        m_Meshes[mid]->SetTransform(Translation(Vec3(-50,50,0)) * Rotation(0,90,90));
        
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    }
    void PostProcessingTest::LoadMaterial()
    {
        //For ground
        Material* Mat = AddMaterial(ShaderGenerator::LM_PHONG,"PostProcessingTest/GroundDiffuse.png",
                                                              "PostProcessingTest/GroundNormal.png");
        Mat->SetShininess(1.0f);
        Mat->SetSpecular(Vec4(1,1,1,0));
        
        //For light displays
        AddMaterial(ShaderGenerator::LM_FLAT,"PostProcessingTest/GroundDiffuse.png");
    }

    void PostProcessingTest::Run()
    {
        Mat4 t = Translation(Vec3(0,4,9)) * RotationX(20.0f);
        m_Camera->SetTransform(t);
         
		m_Meshes[0]->SetTextureTransform(Scale(GROUND_TEXTURE_SCALE));
        
        Scalar a = 0.0f;
        m_TaskManager->GetTaskContainer()->SetAverageTaskDurationSampleCount(10);
        m_TaskManager->GetTaskContainer()->SetAverageThreadTimeDifferenceSampleCount(10);
        
        //Particle stuff
        Material* pMat = m_Renderer->CreateMaterial();
        pMat->LoadMaterial(Load("PostProcessingTest/Particles.mtrl"));
        m_Materials.push_back(pMat);
        
        RenderObject* ParticleCloud = m_Renderer->CreateRenderObject(ROT_MESH);
        Mesh* m = new Mesh();
        
        vector<Vec3> InitialPositions;
        vector<Vec3> InitialVelocities;
        vector<Vec4> Colors;
        vector<Vec2> TimeVars;
        
        
        for(i32 i = 0;i < PARTICLE_COUNT;i++)
        {
            Scalar EmitTime = Random(PARTICLE_MIN_EMIT_TIME,PARTICLE_MAX_EMIT_TIME);
            Scalar EmitTimeNormalized = (EmitTime - PARTICLE_MIN_EMIT_TIME) / (PARTICLE_MAX_EMIT_TIME - PARTICLE_MIN_EMIT_TIME);
            
            InitialPositions.push_back(RandomVec(PARTICLE_SPAWN_RADIUS).Normalized() + PARTICLE_SPAWN_POS);
            InitialVelocities.push_back(Quat(PARTICLE_VELOCITY_ROT_AXIS,EmitTimeNormalized * 360.0f * PARTICLE_VELOCITY_ROTATIONS).ToMat() * Random(PARTICLE_VELOCITY_MIN,PARTICLE_VELOCITY_MAX));
            TimeVars.push_back(Vec2(Random(PARTICLE_MIN_DURATION,PARTICLE_MAX_DURATION),EmitTime));
            Colors.push_back(Vec4(ColorFunc(EmitTimeNormalized),1.0f));
        }
        
        m->SetVertexBuffer(PARTICLE_COUNT,&InitialPositions[0].x);
        m->SetNormalBuffer(PARTICLE_COUNT,&InitialVelocities[0].x);
        m->SetColorBuffer(PARTICLE_COUNT,&Colors[0].x);
        m->SetTexCoordBuffer(PARTICLE_COUNT,&TimeVars[0].x);
        m->PrimitiveType = PT_POINTS;
        ParticleCloud->SetMesh(m,pMat);
        ParticleCloud->SetAlwaysVisible(true);
        m_Renderer->GetScene()->AddRenderObject(ParticleCloud);
        
        glEnable(GL_PROGRAM_POINT_SIZE);
        
    
        while(IsRunning())
        {
            a += GetDeltaTime();
            
            //Update time
            pMat->GetUserUniforms()->SetUniform(0,a * 0.9f);
            
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

    void PostProcessingTest::Shutdown()
    {
    }
};
