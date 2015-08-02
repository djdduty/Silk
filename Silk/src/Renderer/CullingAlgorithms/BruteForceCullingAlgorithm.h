#pragma once
#include <Renderer/CullingAlgorithm.h>
#include <System/TaskManager.h>

namespace Silk
{
    class Scene;
    class BruteForceCullingAlgorithm;
    
    class BruteForceCullingTask : public Task
    {
        public:
            BruteForceCullingTask(BruteForceCullingAlgorithm* Parent,i32 StartIndex,i32 Count);
            ~BruteForceCullingTask();
        
            virtual void Run();
        
            BruteForceCullingAlgorithm* m_Parent;
            i32 m_StartIndex;
            i32 m_Count;
            vector<RenderObject*> m_VisibleObjects;
    };
    
    class BruteForceCullingAlgorithm : public CullingAlgorithm
    {
        public:
            BruteForceCullingAlgorithm(Scene* s,TaskManager* t);
            ~BruteForceCullingAlgorithm();
        
            void SetTaskManager(TaskManager* T);
            TaskManager* GetTaskManager() const { return m_TaskManager; }
        
            virtual CullingResult* PerformCulling();
            virtual const char* GetAlgorithmName() const { return "Brute Force"; }
        
        protected:
            TaskManager* m_TaskManager;
    };
};

