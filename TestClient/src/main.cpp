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
        g.SetUniformInput(ShaderGenerator::IUT_RENDERER_UNIFORMS,true);
        g.SetUniformInput(ShaderGenerator::IUT_OBJECT_UNIFORMS  ,true);
        g.SetAttributeInput(ShaderGenerator::IAT_POSITION       ,true);
        g.SetAttributeInput(ShaderGenerator::IAT_TEXCOORD       ,true);
        g.SetTextureInput  (Material::MT_DIFFUSE                ,true);
        g.SetFragmentOutput(ShaderGenerator::OFT_COLOR          ,true);
        
        g.AddVertexModule  (const_cast<CString>("[SetTexCoords]o_TexCoord = a_TexCoord;[/SetTexCoords]"),0);
        //g.AddVertexModule  (const_cast<CString>("[SetPosition]gl_Position = vec4(a_Position,1.0);[/SetPosition]"),1);
        g.AddFragmentModule(const_cast<CString>("[SetColor]f_Color = texture(u_DiffuseMap,o_TexCoord);[/SetColor]"),0);
        
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
        
        mesh->SetVertexBuffer  (12,vertBuff );
        mesh->SetTexCoordBuffer(12,TexcBuff );

        RenderObject* Objs[5000];
        Material* mat = Render->CreateMaterial();
        mat->SetShader(g.Generate());
        for(i32 i = 0;i < 5000;i++)
        {
            Objs[i] = Render->CreateRenderObject(ROT_MESH,false);
            Objs[i]->SetMesh(mesh,mat);
            Objs[i]->SetTransform(Translation(Vec3(Random(-500,500),0,Random(-500,500))));
            Render->AddRenderObject(Objs[i]);
        }
        
        Scalar Aspect = r->GetContext()->GetResolution().y / r->GetContext()->GetResolution().x;
        
        Camera* Cam = new Camera(Vec2(60.0f,60.0f * Aspect),0.01f,100.0f);
        Cam->SetExposure(1.0f);
        Cam->SetFocalPoint(1.0f);
        Render->SetActiveCamera(Cam);
        
        Mat4 cTrans = Translation(Vec3(0,2,-10));
        Scalar a = 0.0f;
        while(!Win->GetCloseRequested())
        {
            Win->PollEvents();
            Aspect = r->GetContext()->GetResolution().y / r->GetContext()->GetResolution().x;
            Cam->SetFieldOfView(Vec2(60.0f,60.0f * Aspect));
            
            r->ClearActiveFramebuffer();
            a += 0.01f;
            Mat4 Rot0 = Rotation(Vec3(0,1,0),a);
            Rot0     *= Rotation(Vec3(1,0,0),10.0f);
            Cam->SetTransform(Rot0 * Translation(Vec3(0,1,10 + a)));
            
            for(i32 i = 0;i < 5000;i++) Objs[i]->SetTransform(Objs[i]->GetTransform() * Rotation(Vec3(1,0,0),0.01f));
            
            Render->Render(GL_TRIANGLES);
            
            Win->SwapBuffers();
        }

        Render->GetRasterizer()->Destroy(mat->GetShader());
        Render->Destroy(mat);
        for(i32 i = 0;i < 5000;i++) Render->Destroy(Objs[i]);
        delete Render;
        delete mesh;
    }
    
    delete Win;
    return 0;
}
