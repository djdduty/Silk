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

#include <Renderer/Renderer.h>

using namespace TestClient;

int main(int ArgC,char *ArgV[])
{
    Window* Win = new Window();
    if(Win->Create(Vec2(800,600),"Test Silk Client",true))
    {
        Rasterizer* r = Win->GetRasterizer();
        ((OpenGLRasterizer*)r)->SetClearBuffers(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
