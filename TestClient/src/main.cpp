#include <Test.h>
#include <InstancingTest.h>
using namespace TestClient;

int main(int ArgC,char *ArgV[])
{
    InstancingTest* Test = new InstancingTest();
    
    Test->Init();
    Test->Run();
    Test->Shutdown();
    
    delete Test;
    return 0;
}
