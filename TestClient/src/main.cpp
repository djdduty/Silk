#ifdef __APPLE__
#include <unistd.h>
#else
#include <direct.h>
#endif

#include <Test.h>
#include <InstancingTest.h>
#include <LightingTest.h>
using namespace TestClient;

int main(int ArgC,char *ArgV[])
{

    //InstancingTest* Test = new InstancingTest();
    LightingTest* Test = new LightingTest();
    
    Test->Init    ();
    Test->Run     ();
    Test->Shutdown();
    
    delete Test;
    
    return 0;
}
