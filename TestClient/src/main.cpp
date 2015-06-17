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

using namespace TestClient;

int main(int ArgC,char *ArgV[])
{
    Window* Win = new Window();
    OpenGLRasterizerContext* Ctx = new OpenGLRasterizerContext();
    Ctx->m_Format.SetGrayscale(8,8);
    if(Win->Create(Ctx,Vec2(800,600),"Test Silk Client",true))
    {
        Rasterizer* r = Win->GetRasterizer();
        ((OpenGLRasterizer*)r)->SetClearBuffers(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        r->SetClearColor(Vec4(1.0,0.0,1.0,1.0));
        
        while(!Win->GetCloseRequested())
        {
            Win->PollEvents();
            
            r->SetViewport(0,0,800,600);
            r->ClearActiveFramebuffer();

            Win->SwapBuffers();
        }
    }
    
    delete Win;
    delete Ctx;
    return 0;
}
