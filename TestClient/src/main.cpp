#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <math.h>

#include <Window.h>

#ifdef WIN32
   //#include <vld.h>
#endif

#include <Silk.h>
#include <Raster/OpenGL/OpenGLRasterizer.h>
#include <Raster/OpenGL/OpenGLShader.h>
#include <Raster/OpenGL/OpenGLTexture.h>

#include <Renderer/Mesh.h>
using namespace Silk;

using namespace TestClient;

#define ObjsSize 20000

typedef struct _Ray
{
    Vec3 Point;
    Vec3 Dir;
} Ray;
Ray unProject
	(
		const Vec3& win,
		const Mat4& v,
		const Mat4& p,
		const Vec4& viewport
	)
	{
        Vec4 ndc;
		ndc.x = (((win.x - viewport.x) / viewport.z) * 2.0f) - 1.0f;
		ndc.y = (((win.y - viewport.y) / viewport.w) * 2.0f) - 1.0f;
        ndc.y *= -1.0f;
        ndc.z =  win.z;
        ndc.w =  1.0f;
        Mat4 vo = v;
        vo[0][3] = v[1][3] = v[2][3] = 0.0f;
        Mat4 m = (vo * p).Inverse();
        Vec4 rVal = ndc;
        
        Scalar W = rVal.x * m.x.w +
                   rVal.y * m.y.w +
                   rVal.z * m.z.w +
                   rVal.w * m.w.w;
        Scalar invW = 1.0f / W;
        
        Vec4    Result((rVal.x * m.x.x + rVal.y * m.y.x + rVal.z * m.z.x + m.w.x) * invW,
                       (rVal.x * m.x.y + rVal.y * m.y.y + rVal.z * m.z.y + m.w.y) * invW,
                       (rVal.x * m.x.z + rVal.y * m.y.z + rVal.z * m.z.z + m.w.z) * invW,
                       (rVal.x * m.x.w + rVal.y * m.y.w + rVal.z * m.z.w + m.w.w) * invW);
        
        //Result *= (1.0f / Result.w);
    
        Vec4 world = Result;//(Cam->GetOrientation(false).ToMatrix() * c->GetProjection()).Inverse() * ndc;
        
        Ray r;
        r.Point = Vec3(v[0][3],v[1][3],v[2][3]);
        r.Dir   = (world.xyz()).Normalized();
        return r;
	}

int main(int ArgC,char *ArgV[])
{
    Window* Win = new Window();
    if(Win->Create(Vec2(800,600),"Test Silk Client",true))
    {
        Rasterizer* r = Win->GetRasterizer();
        ((OpenGLRasterizer*)r)->SetClearBuffers(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        Win->PollEvents();

        Renderer* Render = new Renderer(r);
        r->SetRenderer(Render);
        
        ShaderGenerator* g = new ShaderGenerator(Render);
        g->SetShaderVersion(330);
        g->SetAllowInstancing(true);
        g->SetUniformInput(ShaderGenerator::IUT_RENDERER_UNIFORMS,true);
        g->SetUniformInput(ShaderGenerator::IUT_OBJECT_UNIFORMS  ,true);
        
        g->SetAttributeInput(ShaderGenerator::IAT_POSITION       ,true);
        g->SetAttributeInput(ShaderGenerator::IAT_NORMAL         ,true);
        g->SetAttributeInput(ShaderGenerator::IAT_TEXCOORD       ,true);
        
        g->SetAttributeOutput(ShaderGenerator::IAT_NORMAL        ,true);
        g->SetAttributeOutput(ShaderGenerator::IAT_TEXCOORD      ,true);
        
        g->SetTextureInput  (Material::MT_DIFFUSE                ,true);
        g->SetFragmentOutput(ShaderGenerator::OFT_COLOR          ,true);
        
        g->AddFragmentModule(const_cast<CString>("[NdotL]float NdotL = dot(o_Normal,vec3(0,1,0));\n\tif(NdotL < 0.0) NdotL = -NdotL;[/NdotL]"),0);
        g->AddFragmentModule(const_cast<CString>("[SetColor]f_Color = texture(u_DiffuseMap,o_TexCoord) * NdotL;[/SetColor]"),1);
        
        Win->PollEvents();
        
        Mesh* mesh = new Mesh();
        
        Scalar vertBuff [] =
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
        
        Scalar normBuff [] =
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
        
        Scalar TexcBuff [] =
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
        
        u32 IndiBuff [] =
        {
            0 ,1 ,2 ,
            3 ,4 ,5 ,
            6 ,7 ,8 ,
            9 ,10,11
        };
        
        mesh->SetIndexBuffer   (12,IndiBuff);
        mesh->SetVertexBuffer  (12,vertBuff);
        mesh->SetNormalBuffer  (12,normBuff);
        mesh->SetTexCoordBuffer(12,TexcBuff);

        RenderObject* Objs[ObjsSize];
        Material* mat = Render->CreateMaterial();
        mat->SetShader(g->Generate());
        
        Vec3 Velocity[ObjsSize];
        Vec3 Acceleration[ObjsSize];
        Vec3 Position[ObjsSize];
        
        Vec3 RotationAxis[ObjsSize];
        Scalar RotationAngle[ObjsSize];
        Scalar AngularVelocity[ObjsSize];
        Scalar Mass[ObjsSize];
        
        Scalar SpawnD  = 300.0f;
        Scalar MinVel  = 60.0f;
        Scalar MaxVel  = 180.0f;
        Scalar MinaVel = 50.0f;
        Scalar MaxaVel = 360.0f;
        Scalar MinMass = 1000.0f;
        Scalar MaxMass = 100000.0f;
        Vec3 VelDir(0,1,0);
        Vec3 VelDirVar(0.01f,0.4f,0.01f);
        Scalar AngularDamping = 1;
        Scalar LinearDamping  = 0.95f;
        
        f64 G = 0.000000000066742;
        
        for(i32 i = 0;i < ObjsSize;i++)
        {
            Objs[i] = Render->CreateRenderObject(ROT_MESH,false);
            Objs[i]->SetMesh(mesh,mat);
            
            Position[i] = Vec3(Random(-1,1),Random(-1,1),Random(-1,1)).Normalized();
            Position[i].x *= Random(-SpawnD,SpawnD);
            Position[i].y = Random(-20.0f,20.0f);
            Position[i].z *= Random(-SpawnD,SpawnD);
            
            Vec3 Dir = (Vec3(0,0,0) - Position[i]);
            Scalar Dist = Dir.Magnitude();
            Dir *= (1.0f / Dist);
                    
            AngularVelocity[i] = MinaVel + Random(-(MaxaVel - MinaVel),MaxaVel - MinaVel);
            RotationAxis[i] = Vec3(Random(-1,1),Random(-1,1),Random(-1,1)).Normalized();
            RotationAngle[i] = 0.0f;
                    
            Mass[i] = Random(MinMass,MaxMass);
                    
            Velocity[i] = Dir.Cross(Vec3(0,1,0)).Normalized() * sqrtf(((G * (100000000000.0 * Mass[i])) / Dist));
            Render->AddRenderObject(Objs[i]);
            
        }
        
        Scalar Aspect = r->GetContext()->GetResolution().y / r->GetContext()->GetResolution().x;
        
        Camera* Cam = new Camera(Vec2(60.0f,60.0f * Aspect),0.001f,2000.0f);
        Cam->SetExposure(1.0f);
        Cam->SetFocalPoint(1.0f);
        Render->SetActiveCamera(Cam);
        
        Mat4 cTrans = Translation(Vec3(0,4,-10));
        Scalar a = 20.0f;
        f32 ElapsedTime = 0.0;
        f32 LastTime = Win->GetElapsedTime();
        i32 FrameCounter = 0;

        #define StartTimer() StartTrans = Win->GetElapsedTime()
        #define GetTimer(TimeVar) TimeVar = (Win->GetElapsedTime() - StartTrans)*1000.0

        while(!Win->GetCloseRequested())
        {
            f32 FrameStart = Win->GetElapsedTime();
            f32 StartTrans = 0;
            f32 TransDelta = 0;

            f32 PollTime   = 0;
            f32 CameraTime = 0;
            f32 ObjectTime = 0;
            f32 RenderTime = 0;
            f32 SwapTime   = 0;

            StartTimer();
            Win->PollEvents();
            GetTimer(PollTime);
            
            StartTimer();
            f32 DeltaTime = Win->GetElapsedTime() - LastTime;
            LastTime = Win->GetElapsedTime();
            Scalar pdt = DeltaTime * 1.9f;
            if(glfwGetKey(Win->GetWindow(),GLFW_KEY_S) == GLFW_PRESS) pdt *= 0.01f;
            
            r->ClearActiveFramebuffer();
            
            a += 0.1f * pdt;
            Mat4 Rot0 = Rotation(Vec3(1,0,0),45 * sin(a * 0.1f));
            Scalar Dist = SpawnD * 2.5f + (sin(a * 0.5f) * SpawnD * 2.5f);
            Cam->SetTransform(Rotation(Vec3(0,1,0),a * 5.0f) * Translation(Vec3(0,Dist * sin(a * 0.1f),Dist)) * Rot0);// + (sin(a * 0.5f) * 2.0f)))));
            GetTimer(CameraTime);

            StartTimer();
            Render->Render(GL_TRIANGLES);
            GetTimer(RenderTime);
            
            f64 x,y;
            glfwGetCursorPos(Win->GetWindow(),&x,&y);
            
            GLint Viewport[4];
            glGetIntegerv(GL_VIEWPORT,Viewport);
            
            Mat4 v = Cam->GetTransform();
            Mat4 p = Cam->GetProjection();
            
            Ray r = unProject(Vec3(x,y,0),v,p,Vec4(Viewport[0],Viewport[1],Viewport[2],Viewport[3]));
            Vec3 WorldPt = r.Point + (r.Dir * Dist);
            
            StartTimer();
            for(i32 i = 0;i < ObjsSize;i++)
            {
                Scalar pMag = Position[i].Magnitude();
                if(pMag > SpawnD * 2.5f)
                {
                    Position[i] = Vec3(Random(-1,1),Random(-1,1),Random(-1,1)).Normalized();
                    Position[i].x *= Random(-SpawnD,SpawnD);
                    Position[i].y = Random(-20.0f,20.0f);
                    Position[i].z *= Random(-SpawnD,SpawnD);
            
                    if(glfwGetKey(Win->GetWindow(),GLFW_KEY_SPACE) != GLFW_PRESS) WorldPt = Vec3(0,0,0);
                    Vec3 Dir = (WorldPt - Position[i]);
                    Scalar Dist = Dir.Magnitude();
                    Dir *= (1.0f / Dist);
                    
                    AngularVelocity[i] = MinaVel + Random(-(MaxaVel - MinaVel),MaxaVel - MinaVel);
                    RotationAxis[i] = Vec3(Random(-1,1),Random(-1,1),Random(-1,1)).Normalized();
                    RotationAngle[i] = 0.0f;
                    
                    Mass[i] = Random(MinMass,MaxMass);
                    
                    Velocity[i] = Dir.Cross(Vec3(0,1,0)).Normalized() * sqrtf(((G * (100000000000.0 * Mass[i])) / Dist));
                }
                else if(pMag < 15.0f)
                {
                    Velocity[i].y = (Random(0.0f,1.0f) < 0.5f) ? Random(-1000.0f,-500.0f) : Random(500.0f,1000.0f);
                    Velocity[i].x = Random(-0.001f,0.001f);
                    Velocity[i].z = Random(-0.001f,0.001f);
                }
                
                Scalar StarMass = 100000000000.0f;
                Vec3 StarPt = Vec3(0,0,0);
                Vec3 Dir = (StarPt - Position[i]);
                Scalar Dist = Dir.Magnitude();
                Dir *= (1.0f / Dist);
                
                Acceleration[i] = Dir * ((G * (StarMass * Mass[i])) / (Dist * Dist));
                if(glfwGetKey(Win->GetWindow(),GLFW_KEY_SPACE) == GLFW_PRESS)
                {
                    StarPt = WorldPt;
                    Vec3 Dir = (StarPt - Position[i]);
                    Scalar Dist = Dir.Magnitude();
                    StarMass = 1000000000.0f;
                    Acceleration[i] += Dir * ((G * (StarMass * Mass[i])) / (Dist * Dist));
                }
                
                Velocity[i] += -(Velocity[i] - (Velocity[i] * LinearDamping)) * pdt;
                Velocity[i] += Acceleration[i] * pdt;
                Position[i] += Velocity[i] * pdt;
                
                AngularVelocity[i] += -(AngularVelocity[i] - (AngularVelocity[i] * AngularDamping)) * pdt;
                RotationAngle  [i] += AngularVelocity[i] * pdt;
                
                if(i == 0) Position[0] = WorldPt;
                Objs[i]->SetTransform(Translation(Position[i]) * Rotation(RotationAxis[i],RotationAngle[i]));
                Objs[i]->SetTextureTransform(Rotation(Vec3(0,0,1),a));
            }
            GetTimer(ObjectTime);

            StartTimer();
            Win->SwapBuffers();
            GetTimer(SwapTime);

            f32 FrameTime = (Win->GetElapsedTime() - FrameStart) * 1000;

            #define Percent(Time) (Time / FrameTime) * 100.0
            f32 PollPercent   = Percent(PollTime  );
            f32 CameraPercent = Percent(CameraTime);
            f32 ObjectPercent = Percent(ObjectTime);
            f32 RenderPercent = Percent(RenderTime);
            f32 SwapPercent   = Percent(SwapTime  );

            if(FrameTime > 50)
            {
                printf(
                    "Total Frame Time: %0.2fms\n"
                    "\tPoll Events        : %0.2f\%% Total: %0.2fms\n"
                    "\tCamera Transforming: %0.2f\%% Total: %0.2fms\n"
                    "\tObject Transforming: %0.2f\%% Total: %0.2fms\n"
                    "\tRender Calls       : %0.2f\%% Total: %0.2fms\n"
                    "\tSwapping Buffers   : %0.2f\%% Total: %0.2fms\n\n", 
                    FrameTime    , 
                    PollPercent  , PollTime  ,
                    CameraPercent, CameraTime,
                    ObjectPercent, ObjectTime,
                    RenderPercent, RenderTime,
                    SwapPercent  , SwapTime);
            }
            ElapsedTime += DeltaTime;
            FrameCounter++;
            if(ElapsedTime > 1.5f)
            {
                printf("Average FPS: %0.2f\n",f32(FrameCounter) / 1.5f);
                
                ElapsedTime  = 0.0f;
                FrameCounter = 0;
            }
        }

        Render->GetRasterizer()->Destroy(mat->GetShader());
        Render->Destroy(mat);
        for(i32 i = 0;i < ObjsSize;i++) Render->Destroy(Objs[i]);
        delete g;
        delete Render;
        delete Cam;
        mesh->Destroy();
    }
    
    delete Win;
    return 0;
}
