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
    #ifdef __APPLE__
    
    string Path(ArgV[0]);
    for(size_t i = Path.size() - 1;i > 0;i--)
    {
        if(Path[i] == '/') { Path[i] = 0; break; }
        Path.pop_back();
    }
    chdir(Path.c_str());
    chdir("../Resources");
    
    char cwd[256];
    getcwd(cwd,256);
    
    #endif

    //InstancingTest* Test = new InstancingTest();
    LightingTest* Test = new LightingTest();
    
    Test->Init    ();
    Test->Run     ();
    Test->Shutdown();
    
    delete Test;
    
    return 0;
}