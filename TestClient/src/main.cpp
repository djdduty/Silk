#include <unistd.h>

#include <Test.h>
#include <InstancingTest.h>
#include <LightingTest.h>
using namespace TestClient;

int main(int ArgC,char *ArgV[])
{
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

    //InstancingTest* Test = new InstancingTest();
    LightingTest* Test = new LightingTest();
    
    Test->Init    ();
    Test->Run     ();
    Test->Shutdown();
    
    delete Test;
    
    return 0;
}
