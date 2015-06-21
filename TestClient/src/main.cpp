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

        Renderer* Rend = new Renderer(r);
        
        ShaderGenerator g(Rend);
        g.SetShaderVersion(330);
        g.SetAttributeInput(ShaderGenerator::IAT_POSITION       ,true);
        g.SetAttributeInput(ShaderGenerator::IAT_COLOR          ,true);
        g.SetFragmentOutput(ShaderGenerator::OFT_COLOR          ,true);
        g.SetUniformInput  (ShaderGenerator::IUT_USER_UNIFORMS  ,true);
        g.SetLightingMode  (ShaderGenerator::LM_PHONG                );
        
        UniformBuffer* ub = Rend->GetRasterizer()->CreateUniformBuffer(ShaderGenerator::IUT_USER_UNIFORMS);
        ub->SetUniform(ub->DefineUniform("Test0"),Vec4(1,0,0,1));
        ub->SetUniform(ub->DefineUniform("Test1"),Vec4(0,1,0,1));
        ub->SetUniform(ub->DefineUniform("Test2"),Vec4(0,0,1,1));
        
        vector<Vec4>c; c.push_back(Vec4(1,1,1,1)); c.push_back(Vec4(0,1,0,1)); c.push_back(Vec4(0,0,1,1));
        ub->SetUniform(ub->DefineUniform("Test3"),c);
        
        vector<Light*> Lts;
        for(i32 i = 0;i < 45;i++)
        {
            Light* Lt = new Light(LT_POINT);
            Lt->m_Position = Vec4(0,0,0,0);
            Lt->m_Direction = Vec4(0,0,0,0);
            Lt->m_Color = Vec4(0.0,1.0,0.0,1.0);
            Lt->m_Cutoff = 0.0f;
            Lt->m_Soften = 1.0f;
            Lt->m_Power = 1.0f;
            Lt->m_Attenuation.Constant    = 1.0f;
            Lt->m_Attenuation.Linear      = 1.0f;
            Lt->m_Attenuation.Exponential = 1.0f;
            Lts.push_back(Lt);
        }
        Lts[0]->m_Color = Vec4(1,0,0,1);
        Lts[1]->m_Color = Vec4(0,1,0,1);
        Lts[2]->m_Color = Vec4(0,0,1,1);
        Lts[3]->m_Color = Vec4(1,1,0,1);
        Lts[4]->m_Color = Vec4(1,1,1,1);
        Lts[20]->m_Color = Vec4(0,1,1,1);
        
        ub->SetUniform(ub->DefineUniform("Test4"),Lts);
        
        g.SetUserUniformBuffer(ub);
    
        g.AddVertexModule(const_cast<CString>("[SetPosition]gl_Position = vec4(a_Position,1.0);[/SetPosition]"),0);
        g.AddVertexModule(const_cast<CString>("[SetColor]o_Color = Test4[20].Color;[/SetColor]"),1);

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

        Render->GetRasterizer()->DestroyUniformBuffer(ub);
        Render->GetRasterizer()->DestroyShader(mat->GetShader());
        delete Render;
        delete rObj;
        delete mat;
        delete mesh;
        for(i32 i = 0;i < 20;i++) delete Lts[i];
    }
    
    delete Win;
    return 0;
}
