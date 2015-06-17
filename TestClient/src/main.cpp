#include <iostream>
#include <stdio.h>
#include <cstdlib>

#include <Window.h>

#ifdef WIN32
    //#include <vld.h>
    #include <Windows.h>
#endif

#include <Silk.h>

using namespace TestClient;

int main(int ArgC,char ArgV[])
{
    Window* Win = new Window();
    Win->Create(Vec2(800,600), "Test silk Client");
    while(!Win->GetCloseRequested()) {
        Win->PollEvents();

        Win->SwapBuffers();
    }
    Win->Destroy();
    delete Win;
}
