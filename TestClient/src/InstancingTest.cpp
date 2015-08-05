#include <InstancingTest.h>
#include <math.h>

namespace TestClient
{
    void InstancingTest::Initialize()
    {
        SpawnD          = 100.0f;
        MinaVel         = 50.0f;
        MaxaVel         = 360.0f;
        MinMass         = 1000.0f;
        MaxMass         = 100000.0f;
        VelDir          = Vec3(0,1,0);
        VelDirVar       = Vec3(0.01f,0.4f,0.01f);
        AngularDamping  = 1;
        LinearDamping   = 0.95f;
        
        LoadMesh();
        GenerateShader();
        
        for(i32 i = 0;i < ObjsSize;i++)
        {
            m_Objs[i] = m_Renderer->CreateRenderObject(ROT_MESH,false);
            m_Objs[i]->SetMesh(m_Mesh,m_Material);
            RespawnParticle(i);
            m_Renderer->GetScene()->AddRenderObject(m_Objs[i]);
        }
    }
    void InstancingTest::LoadMesh()
    {
        m_Mesh = new Mesh();
        
        static Scalar vertBuff [] =
        {
            -1, 0,-1,
            -1, 0, 1,
             1, 0,-1,
            
             1, 0,-1,
            -1, 0, 1,
             1, 0, 1,
            
            -1,-1, 0,
            -1, 1, 0,
             1,-1, 0,
            
             1,-1, 0,
            -1, 1, 0,
             1, 1, 0
        };
        
        static Scalar normBuff [] =
        {
            0,1,0,
            0,1,0,
            0,1,0,
            
            0,1,0,
            0,1,0,
            0,1,0,
            
            0,0,1,
            0,0,1,
            0,0,1,
            
            0,0,1,
            0,0,1,
            0,0,1,
        };
        
        static Scalar TexcBuff [] =
        {
            0,1,
            0,0,
            1,1,
            
            1,1,
            0,0,
            1,0,
            
            0,1,
            0,0,
            1,1,
            
            1,1,
            0,0,
            1,0
        };
        
        static u32 IndiBuff [] =
        {
            0 ,1 ,2 ,
            3 ,4 ,5 ,
            6 ,7 ,8 ,
            9 ,10,11
        };
        
        m_Mesh->SetIndexBuffer   (12,IndiBuff);
        m_Mesh->SetVertexBuffer  (12,vertBuff);
        m_Mesh->SetNormalBuffer  (12,normBuff);
        m_Mesh->SetTexCoordBuffer(12,TexcBuff);
    }
    void InstancingTest::GenerateShader()
    {
        m_ShaderGenerator->SetShaderVersion(330);
        m_ShaderGenerator->SetAllowInstancing(true);
        m_ShaderGenerator->SetUniformInput(ShaderGenerator::IUT_RENDERER_UNIFORMS,true);
        m_ShaderGenerator->SetUniformInput(ShaderGenerator::IUT_OBJECT_UNIFORMS  ,true);
        
        m_ShaderGenerator->SetAttributeInput(ShaderGenerator::IAT_POSITION       ,true);
        m_ShaderGenerator->SetAttributeInput(ShaderGenerator::IAT_NORMAL         ,true);
        m_ShaderGenerator->SetAttributeInput(ShaderGenerator::IAT_TEXCOORD       ,true);
        
        m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_NORMAL        ,true);
        m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_TEXCOORD      ,true);
        
        m_ShaderGenerator->SetTextureInput  (Material::MT_DIFFUSE                ,true);
        m_ShaderGenerator->SetFragmentOutput(ShaderGenerator::OFT_COLOR          ,true);
        
        m_ShaderGenerator->AddFragmentModule(const_cast<CString>("[NdotL]float NdotL = dot(o_Normal,vec3(0,1,0));\n\tif(NdotL < 0.0) NdotL = -NdotL;[/NdotL]"),0);
        m_ShaderGenerator->AddFragmentModule(const_cast<CString>("[SetColor]vec4 sColor = texture(u_DiffuseMap,o_TexCoord) * NdotL;[/SetColor]"),1);
        
        m_Material = m_Renderer->CreateMaterial();
        m_Material->SetShader(m_ShaderGenerator->Generate());
        m_ShaderGenerator->Reset();
    }
    void InstancingTest::RespawnParticle(i32 i)
    {
        Position[i] = Vec3(Random(-1,1),Random(-1,1),Random(-1,1)).Normalized();
        Position[i].x *= Random(-SpawnD,SpawnD);
        Position[i].y  = Random(-20.0f,20.0f);
        Position[i].z *= Random(-SpawnD,SpawnD);
        
        Vec3   Dir  = (Vec3(0,0,0) - Position[i]);
        Scalar Dist = Dir.Magnitude();
        Dir *= (1.0f / Dist);
                
        AngularVelocity[i] = MinaVel + Random(-(MaxaVel - MinaVel),MaxaVel - MinaVel);
        RotationAxis   [i] = Vec3(Random(-1,1),Random(-1,1),Random(-1,1)).Normalized();
        RotationAngle  [i] = 0.0f;
                
        Mass[i] = Random(MinMass,MaxMass);
                
        Velocity[i] = Dir.Cross(Vec3(0,1,0)).Normalized() * sqrtf(((G * (100000000000.0 * Mass[i])) / Dist));
    }
    void InstancingTest::UpdateParticle(i32 i,Scalar dt,bool SpacePressed,const Vec3& StarPos)
    {
        Scalar pMag = Position[i].Magnitude();
        if(pMag > SpawnD * 2.5f) RespawnParticle(i);
        else if(pMag < 10.0f)
        {
            Velocity[i].y = (Random(0.0f,1.0f) < 0.5f) ? Random(-1000.0f,-500.0f) : Random(500.0f,1000.0f);
            Velocity[i].x = Random(-0.0001f,0.0001f);
            Velocity[i].z = Random(-0.0001f,0.0001f);
        }
        
        Scalar StarMass = 100000000000.0f;
        Vec3 StarPt = Vec3(0,0,0);
        Vec3 Dir = (StarPt - Position[i]);
        Scalar Dist = Dir.Magnitude();
        Dir *= (1.0f / Dist);
        
        Acceleration[i] = Dir * ((G * (StarMass * Mass[i])) / (Dist * Dist));
        if(SpacePressed)
        {
            Vec3 Dir = (StarPos - Position[i]);
            Scalar Dist = Dir.Magnitude();
            StarMass = 100000000000.0f;
            Acceleration[i] += Dir * ((G * (StarMass * Mass[i])) / (Dist * Dist));
        }
        
        Velocity[i] += -(Velocity[i] - (Velocity[i] * LinearDamping)) * dt;
        Velocity[i] += Acceleration[i] * dt;
        Position[i] += Velocity    [i] * dt;
        
        AngularVelocity[i] += -(AngularVelocity[i] - (AngularVelocity[i] * AngularDamping)) * dt;
        RotationAngle  [i] +=   AngularVelocity[i] * dt;
        
        m_Objs[i]->SetTransform(Rotation(RotationAxis[i],RotationAngle[i]) * Translation(Position[i]));
    }
    void InstancingTest::Run()
    {
        Scalar a = 20.0f;

        #define StartTimer() StartTrans = m_Window->GetElapsedTime()
        #define GetTimer(TimeVar) TimeVar = ((m_Window->GetElapsedTime() - StartTrans) * 1000.0)

        while(true)
        {
            f32 FrameStart = m_Window->GetElapsedTime();
            f32 StartTrans = 0;

            f32 IsRunTime  = 0;
            f32 CameraTime = 0;
            f32 ObjectTime = 0;
            f32 RenderTime = 0;

            StartTimer();
                if(!IsRunning()) break;
            GetTimer(IsRunTime);
            
            StartTimer();
                Scalar pdt = GetDeltaTime() * 1.9f;
                if(glfwGetKey(m_Window->GetWindow(),GLFW_KEY_S) == GLFW_PRESS) pdt *= 0.01f;
                a += 0.1f * pdt;
                Mat4 Rot0 = Rotation(Vec3(1,0,0),45 * sin(a * 0.1f));
                Scalar cDist = SpawnD * 2.5f + (sin(a * 0.5f) * SpawnD * 2.5f);
                m_Camera->SetTransform(Rotation(Vec3(0,1,0),a * 5.0f) * Translation(Vec3(0,cDist * sin(a * 0.1f),cDist)) * Rot0);// + (sin(a * 0.5f) * 2.0f)))));
            GetTimer(CameraTime);

            StartTimer();
                m_Renderer->Render(PT_TRIANGLES);
            GetTimer(RenderTime);
            
            StartTimer();
                bool SpacePressed = glfwGetKey(m_Window->GetWindow(),GLFW_KEY_SPACE) == GLFW_PRESS;
                for(i32 i = 0;i < ObjsSize;i++)
                {
                    UpdateParticle(i,pdt,SpacePressed,m_CursorRay.Point + (m_CursorRay.Dir * cDist));
                    m_Objs[i]->SetTextureTransform(Rotation(Vec3(0,0,1),a));
                }
            GetTimer(ObjectTime);

            f32 FrameTime = (m_Window->GetElapsedTime() - FrameStart) * 1000.0f;
            #define Percent(Time) ((Time / FrameTime) * 100.0f)
            f32 IsRunPercent  = Percent(IsRunTime );
            f32 CameraPercent = Percent(CameraTime);
            f32 ObjectPercent = Percent(ObjectTime);
            f32 RenderPercent = Percent(RenderTime);

            if(FrameTime > 50)
            {
                printf(
                    "Total Frame Time: %0.2fms\n"
                    "\tIsRunning()        : %0.2f\%% Total: %0.2fms\n"
                    "\tCamera Transforming: %0.2f\%% Total: %0.2fms\n"
                    "\tObject Transforming: %0.2f\%% Total: %0.2fms\n"
                    "\tRender Calls       : %0.2f\%% Total: %0.2fms\n",
                    FrameTime    , 
                    IsRunPercent , IsRunTime ,
                    CameraPercent, CameraTime,
                    ObjectPercent, ObjectTime,
                    RenderPercent, RenderTime);
            }
        }
    }
    void InstancingTest::Shutdown()
    {
        for(i32 i = 0;i < ObjsSize;i++) m_Renderer->Destroy(m_Objs[i]);
        m_Renderer->GetRasterizer()->Destroy(m_Material->GetShader());
        m_Renderer->Destroy(m_Material);
        m_Mesh->Destroy();
    }
};
