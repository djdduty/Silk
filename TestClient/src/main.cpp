#ifdef __APPLE__
#include <unistd.h>
#elif _WIN32
#include <direct.h>
#endif

#include <Test.h>

#include <InstancingTest.h>
#include <LightingTest.h>
#include <NormalMappingTest.h>
#include <ParallaxMappingTest.h>
#include <CullingTest.h>
#include <UITest.h>
#include <RTTTest.h>
#include <PostProcessingTest.h>
#include <SSAOTest.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

#include <T4_Viewer.h>
using namespace TestClient;

bool dirExists(const char *path)
{
    struct stat info;

    if(stat( path, &info ) != 0)
        return false;
    else if(info.st_mode & S_IFDIR)
        return true;
    else
        return false;
}

int main(int ArgC,char *ArgV[])
{
    string Path(ArgV[0]);
    for(size_t i = Path.size() - 1;i > 0;i--)
    {
        if(Path[i] == '/' || Path[i] == '\\') { break; }
        Path.pop_back();
    }
    string SubFolder = "../Resources";
    if(dirExists((Path+"Resources").c_str()))
        SubFolder = "Resources";
    Path[Path.size()-1] = 0;
    chdir(Path.c_str());
    chdir(SubFolder.c_str());
    
    char cwd[256];
    getcwd(cwd,256);
    
    //Test* Test = new InstancingTest();
    //Test* Test = new LightingTest();
    //Test* Test = new NormalMappingTest();
    //Test* Test = new ParallaxMappingTest();
    //Test* Test = new CullingTest();
    //Test* Test = new UITest();
    //Test* Test = new RTTTest();
    //Test* Test = new PostProcessingTest();
    Test* Test = new SSAOTest();
	//Test* Test = new T4_Viewer(ArgC,ArgV);
    
    Test->Init    ();
    Test->Run     ();
    Test->Shutdown();
    
    delete Test;
    
    return 0;
}
