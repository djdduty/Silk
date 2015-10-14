#include <Silk.h>
using namespace Silk;

#include <T4/Turok4.h>
using namespace Turok4;

void LoadActorToEngine(Renderer* r,Shader* s,const Mat4& Transform,i32 ID,Actor* a,vector<RenderObject*>& OutObjs);
void DebugPrintActorInfo();