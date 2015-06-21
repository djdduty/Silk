#include <iostream>
#include <stdio.h>
#include <cstdlib>

#include <Window.h>

#ifdef WIN32
   //#include <vld.h>
#endif

#include <Silk.h>
#include <Raster/OpenGL/OpenGLRasterizer.h>
#include <Raster/OpenGL/OpenGLShader.h>
#include <Raster/OpenGL/OpenGLTexture.h>

#include <Renderer/ShaderSystem.h>
#include <Renderer/Renderer.h>

#include <Renderer/Mesh.h>
using namespace Silk;

using namespace TestClient;

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
        
        ShaderGenerator g(Render);
        g.SetShaderVersion(330);
        g.SetAttributeInput(ShaderGenerator::IAT_POSITION       ,true);
        g.SetAttributeInput(ShaderGenerator::IAT_TEXCOORD       ,true);
        g.SetTextureInput  (Material::MT_DIFFUSE                ,true);
        g.SetFragmentOutput(ShaderGenerator::OFT_COLOR          ,true);
    
        g.AddVertexModule(const_cast<CString>("[SetTexCoords]o_TexCoord = a_TexCoord;[/SetTexCoords]"),0);
        g.AddVertexModule(const_cast<CString>("[SetPosition]gl_Position = vec4(a_Position,1.0);[/SetPosition]"),1);
        
        Win->PollEvents();
        
        Mesh* mesh = new Mesh();
        Vec3 vertBuff [] =
        {
            Vec3(-0.375000,-0.000000,0.312500),
            Vec3(0.375000,-0.000000,0.312500),
            Vec3(-0.375000,-0.000000,0.225000),
            
            Vec3(-0.375000,-0.000000,0.225000),
            Vec3(0.375000,-0.000000,0.312500),
            Vec3(-0.225000,-0.000000,0.225000),
            
            Vec3(0.375000,-0.000000,0.312500),
            Vec3(0.375000,-0.000000,0.225000),
            Vec3(-0.225000,-0.000000,0.225000),

            Vec3(-0.375000,-0.000000,0.225000),
            Vec3(0.375000,0.000000,-0.225000),
            Vec3(0.225000,0.000000,-0.225000),

            Vec3(-0.375000,-0.000000,0.225000),
            Vec3(-0.225000,-0.000000,0.225000),
            Vec3(0.375000,0.000000,-0.225000),
            
            Vec3(-0.375000,0.000000,-0.225000),
            Vec3(0.225000,0.000000,-0.225000),
            Vec3(-0.375000,0.000000,-0.312500),

            Vec3(0.375000,0.000000,-0.225000),
            Vec3(0.375000,0.000000,-0.312500),
            Vec3(0.225000,0.000000,-0.225000),
            
            Vec3(0.225000,0.000000,-0.225000),
            Vec3(0.375000,0.000000,-0.312500),
            Vec3(-0.375000,0.000000,-0.312500),
        };
        
        Vec2 TexcBuff [] =
        {
            Vec2(0.000077,0.083507),
            Vec2(0.999909,0.083423),
            Vec2(0.000000,0.200000),
            
            Vec2(0.000000,0.200000),
            Vec2(0.999909,0.083423),
            Vec2(0.200000,0.200000),
            
            Vec2(0.999909,0.083423),
            Vec2(1.000000,0.200000),
            Vec2(0.200000,0.200000),

            Vec2(0.000000,0.200000),
            Vec2(1.000000,0.800000),
            Vec2(0.800000,0.800000),

            Vec2(0.000000,0.200000),
            Vec2(0.200000,0.200000),
            Vec2(1.000000,0.800000),
            
            Vec2(0.000000,0.800000),
            Vec2(0.800000,0.800000),
            Vec2(0.000091,0.916577),
            
            Vec2(1.000000,0.800000),
            Vec2(0.999923,0.916493),
            Vec2(0.800000,0.800000),
            
            Vec2(0.800000,0.800000),
            Vec2(0.999923,0.916493),
            Vec2(0.000091,0.916577),
        };
        
        for(i32 i = 0;i < 24;i++)
        {
            vertBuff[i].y = vertBuff[i].z;
            vertBuff[i].z = 0;
            swap(vertBuff[i].x,vertBuff[i].y);
            vertBuff[i].x *= 0.8f;
            
            vertBuff[i] *= 2.5f;
        }
        
        mesh->SetVertexBuffer  (24,vertBuff );
        mesh->SetTexCoordBuffer(24,TexcBuff );

        RenderObject* rObj = Render->CreateRenderObject(ROT_MESH,false);
        Material* mat = new Material();
        mat->SetShader(g.Generate());
        rObj->SetMesh(mesh, mat);
        Render->AddRenderObject(rObj);
        

        while(!Win->GetCloseRequested())
        {
            Win->PollEvents();

            r->ClearActiveFramebuffer();
            
            Render->Render(GL_TRIANGLES);
            
            Win->SwapBuffers();
        }

        Render->GetRasterizer()->DestroyShader(mat->GetShader());
        delete Render;
        delete rObj;
        delete mat;
        delete mesh;
    }
    
    delete Win;
    return 0;
}