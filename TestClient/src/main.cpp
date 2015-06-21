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

        Renderer* Rend = new Renderer(r);
        
        ShaderGenerator g(Rend);
        g.SetShaderVersion(330);
        g.SetLightingMode  (ShaderGenerator::LM_FLAT                 );
        g.SetAttributeInput(ShaderGenerator::IAT_POSITION       ,true);
        g.SetAttributeInput(ShaderGenerator::IAT_COLOR          ,true);
        g.SetAttributeInput(ShaderGenerator::IAT_TEXCOORD       ,true);
        g.SetUniformInput  (ShaderGenerator::IUT_USER_UNIFORMS  ,true);
        g.SetTextureInput  (Material::MT_DIFFUSE                ,true);
        g.SetFragmentOutput(ShaderGenerator::OFT_COLOR          ,true);
    
        g.AddVertexModule(const_cast<CString>("[SetTexCoords]o_TexCoord = a_TexCoord;[/SetTexCoords]"),0);
        g.AddVertexModule(const_cast<CString>("[SetPosition]gl_Position = vec4(a_Position,1.0);[/SetPosition]"),1);
        
        g.AddFragmentModule(const_cast<CString>("[SetColor]f_Color = texture(u_DiffuseMap,o_TexCoord);[/SetColor]"),0);

        Win->PollEvents();
        
        Mesh* mesh = new Mesh();
        Vec3 vertBuff [3] = { Vec3(0, 0.75f, 0), Vec3(0.75f, -0.75f, 0), Vec3(-0.75f, -0.75f, 0) };
        Vec4 colorBuff[3] = { Vec4(1,0,0,1)    , Vec4(0,1,0,1)         , Vec4(0,0,1,1) };
        Vec2 TexcBuff [3] = { Vec2(0.5f,1.0f)  , Vec2(1.0f,0.0f)       , Vec2(0.0f,0.0f) };
        mesh->SetVertexBuffer  (3,vertBuff );
        mesh->SetColorBuffer   (3,colorBuff);
        mesh->SetTexCoordBuffer(3,TexcBuff );

        Renderer* Render = new Renderer(r);
        RenderObject* rObj = Render->CreateRenderObject(ROT_MESH,false);
        Material* mat = new Material();
        mat->SetShader(g.Generate());
        
        Texture* Tex = new OpenGLTexture();
        Tex->CreateTexture(32,32);
        for(i32 x = 0;x < 32;x++)
        {
            for(i32 y = 0;y < 32;y++)
            {
                Tex->SetPixel(Vec2(x,y),Vec4(Random(0,1),Random(0,1),Random(0,1),1));
            }
        }
        Tex->InitializeTexture();
        
        glBindTexture(GL_TEXTURE_2D,((OpenGLTexture*)Tex)->GetTextureID());
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST); // Linear Filtering
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST); // Linear Filtering
        glBindTexture(GL_TEXTURE_2D,0);
        
        mat->SetMap(Material::MT_DIFFUSE,Tex);
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
