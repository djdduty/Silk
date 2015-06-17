#include <iostream>
#include <stdio.h>
#include <cstdlib>

#include <Window.h>

#ifdef WIN32
    //#include <vld.h>
    #include <Windows.h>
#endif

<<<<<<< HEAD
#include <Silk.h>

int main(int ArgC,char *ArgV[])
=======
using namespace TestClient;

int main(int ArgC,char ArgV[])
>>>>>>> b0808e305c71ff0ad013f5f2c5c6f9949586f030
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
