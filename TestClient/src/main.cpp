#include <iostream>
#include <stdio.h>
#include <cstdlib>

#include <Window.h>

#ifdef WIN32
    //#include <vld.h>
#endif

#include <Silk.h>
#include <Raster/OpenGL/OpenGLRasterizer.h>

#include <Renderer/Mesh.h>
#include <Raster/OpenGL/OpenGLShader.h>

#include <Renderer/ShaderSystem.h>
#include <Renderer/Renderer.h>
using namespace Silk;

using namespace TestClient;

int main(int ArgC,char *ArgV[])
{
    Window* Win = new Window();
    if(Win->Create(Vec2(800,600),"Test Silk Client",true))
    {
        Rasterizer* r = Win->GetRasterizer();
        ((OpenGLRasterizer*)r)->SetClearBuffers(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Renderer* Rend = new Renderer(r);
        
        ShaderGenerator g(Rend);
        g.SetShaderVersion(330);
        g.SetAttributeInput(ShaderGenerator::IAT_POSITION       ,true);
        g.SetAttributeInput(ShaderGenerator::IAT_COLOR          ,true);
        g.SetFragmentOutput(ShaderGenerator::OFT_COLOR          ,true);
        g.SetUniformInput  (ShaderGenerator::IUT_USER_UNIFORMS  ,true);
        g.SetLightingMode  (ShaderGenerator::LM_FLAT                 );
        
        UniformBuffer* ub = Rend->GetRasterizer()->CreateUniformBuffer(ShaderGenerator::IUT_USER_UNIFORMS);
        ub->SetUniform(ub->DefineUniform("Test"),Vec4(0,0,1,1));
        g.SetUserUniformBuffer(ub);
    
        g.AddVertexModule("[SetColor]o_Color = a_Color * Test;[/SetColor]",0);
        g.AddVertexModule("[SetPosition]gl_Position = vec4(a_Position,1.0);[/SetPosition]",0);

        Win->PollEvents();
        
        ub->InitializeBuffer();
        
        Mesh* mesh = new Mesh();
        Vec3 vertBuff [3] = { Vec3(0, 0.75f, 0), Vec3(0.75f, -0.75f, 0), Vec3(-0.75f, -0.75f, 0) };
        Vec4 colorBuff[3] = { Vec4(1,0,0,1), Vec4(0,1,0,1), Vec4(0,0,1,1) };
        mesh->SetVertexBuffer(3,vertBuff );
        mesh->SetColorBuffer (3,colorBuff);

        Renderer* Render = new Renderer(r);
        RenderObject* rObj = Render->CreateRenderObject(ROT_MESH,false);
        Material* mat = new Material();
        mat->SetShader(g.Generate());
        mat->GetShader()->AddUniformBuffer(ub);
        rObj->SetMesh(mesh, mat);
        Render->AddRenderObject(rObj);
        

        while(!Win->GetCloseRequested())
        {
            Win->PollEvents();

            r->ClearActiveFramebuffer();
            
            Render->Render(GL_TRIANGLES);
            
            Win->SwapBuffers();
        }

        delete Render;
        delete rObj;
        delete mat;
        delete mesh;
    }
    
    delete Win;
    return 0;
}
