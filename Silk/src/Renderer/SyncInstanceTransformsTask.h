#pragma once
#include <System/SilkTypes.h>
#include <System/TaskManager.h>
#include <Math/Math.h>
#include <Renderer/RenderObject.h>

#include <string>
using namespace std;

namespace Silk
{
    class SyncInstanceTransformsTask : public Task
    {
        public:
            SyncInstanceTransformsTask(Mesh* Mesh,i32 StartIdx,i32 Count) :
                m_Mesh(Mesh), m_StartIndex(StartIdx), m_Count(Count)
            {
                m_Type = TT_SYNC_INSTANCE_TRANSFORMS;
            }
            ~SyncInstanceTransformsTask()
            {
            }
        
            virtual void Run();
        
        
        protected:
            Mesh* m_Mesh;
            i32 m_StartIndex;
            i32 m_Count;
    };
};

