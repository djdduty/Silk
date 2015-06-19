#include <iostream>
#include <stdio.h>
#include <cstdlib>

#include <Window.h>

#ifdef WIN32
    //#include <vld.h>
    #include <Windows.h>
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
        UniformBuffer* RB = Rend->GetRendererUniformBuffer();
        RB->SetUniform(RB->DefineUniform("CameraPosition"),Vec3(0,10,10));
        RB->SetUniform(RB->DefineUniform("CameraDirection"),Vec3(0,-1,-1).Normalized());
        RB->SetUniform(RB->DefineUniform("NearPlane"),0.01f);
        RB->SetUniform(RB->DefineUniform("FarPlane"),10000.0f);
        RB->SetUniform(RB->DefineUniform("Exposure"),13.2f);
    
        UniformBuffer* EB = Rend->GetEngineUniformBuffer();
        EB->SetUniform(EB->DefineUniform("TimeOfDay"),0.25f);
        EB->SetUniform(EB->DefineUniform("Dummy0"),0.5f);
    
        UniformBuffer* Mat = new UniformBuffer();
        Mat->SetUniform(Mat->DefineUniform("Metalness"),0.5f);
        Mat->SetUniform(Mat->DefineUniform("Roughness"),0.5f);
        Mat->SetUniform(Mat->DefineUniform("Diffuse"  ),Vec4(0.3f,5.0f,8.0f,1.0f));
    
        UniformBuffer* User = new UniformBuffer();
        User->SetUniform(User->DefineUniform("Dummy0"),Mat4::Identity );
        User->SetUniform(User->DefineUniform("Dummy1"),Vec2(0.2f,0.7f));
        User->SetUniform(User->DefineUniform("Dummy2"),4.0f           );
        User->SetUniform(User->DefineUniform("Dummy3"),1              );
        User->SetUniform(User->DefineUniform("Dummy4"),4.0            );
    
    
        ShaderGenerator g(Rend);
        g.SetShaderVersion(330);
        g.SetUniformInput(ShaderGenerator::IUT_MATERIAL_UNIFORMS,true);
        g.SetUniformInput(ShaderGenerator::IUT_ENGINE_UNIFORMS  ,true);
        g.SetUniformInput(ShaderGenerator::IUT_RENDERER_UNIFORMS,true);
        g.SetUniformInput(ShaderGenerator::IUT_USER_UNIFORMS    ,true);
        g.SetAttributeInput(ShaderGenerator::IAT_POSITION       ,true);
        g.SetAttributeInput(ShaderGenerator::IAT_TANGENT        ,true);
        g.SetAttributeInput(ShaderGenerator::IAT_NORMAL         ,true);
        g.SetAttributeInput(ShaderGenerator::IAT_COLOR          ,true);
        g.SetAttributeInput(ShaderGenerator::IAT_TEXCOORD       ,true);
    
        g.SetMaterialUniformBuffer(Mat);
        g.SetUserUniformBuffer    (User);
    
        g.AddModule("[Test0]Some code goes here\n\tCode\n\tCode\n\tCode...[/Test0]",0);
        g.Generate();

        Win->PollEvents();

        Shader* Shdr = new OpenGLShader();
        Shdr->Load("#version 330\n in  vec3 a_Position; void main() { gl_Position = vec4(a_Position,1);}", 0,
                   "#version 330\n out vec4 o_Out0    ; void main() { o_Out0      = vec4(1,0,0,1);     }");

        Mesh* mesh = new Mesh();
        Vec3 vertBuff [3] = { Vec3(0, 0.75f, 0), Vec3(0.75f, -0.75f, 0), Vec3(-0.75f, -0.75f, 0) };
        mesh->SetVertexBuffer(3, vertBuff);

        Renderer* Render = new Renderer(r);
        RenderObject* rObj = Render->CreateRenderObject(ROT_MESH);
        Material* mat = new Material();
        rObj->SetMesh(mesh, mat);

        while(!Win->GetCloseRequested())
        {
            Win->PollEvents();

            r->ClearActiveFramebuffer();
            
            Shdr->Enable();
            Render->Render(GL_TRIANGLES);
            Shdr->Disable();
            
            Win->SwapBuffers();
        }

        delete Render;
        delete rObj;
        delete mat;
        delete mesh;
        delete Shdr;
    }
    
    delete Win;
    return 0;
}
