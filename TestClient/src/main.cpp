#include <iostream>
#include <stdio.h>
#include <cstdlib>

#include <Window.h>

#ifdef WIN32
    #include <vld.h>
    #include <Windows.h>
#endif

#include <Silk.h>
#include <Raster/OpenGL/OpenGLRasterizer.h>

using namespace TestClient;

int main(int ArgC,char *ArgV[])
{
    Window* Win = new Window();
    if(Win->Create(Vec2(800,600),"Test Silk Client",true))
    {
        Rasterizer* r = Win->GetRasterizer();
        ((OpenGLRasterizer*)r)->SetClearBuffers(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        while(!Win->GetCloseRequested())
        {
            Win->PollEvents();

            r->ClearActiveFramebuffer();

            Win->SwapBuffers();
        }
    }
    
    delete Win;
    return 0;
}
