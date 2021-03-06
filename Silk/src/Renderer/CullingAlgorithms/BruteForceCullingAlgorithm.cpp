#include <Renderer/CullingAlgorithms/BruteForceCullingAlgorithm.h>
#include <Renderer/Renderer.h>

namespace Silk
{
    BruteForceCullingTask::BruteForceCullingTask(BruteForceCullingAlgorithm* Parent,i32 StartIndex,i32 Count) :
        m_Parent(Parent), m_StartIndex(StartIndex), m_Count(Count)
    {
        m_Type = TT_BRUTE_FORCE_CULL_TASK;
    }
    BruteForceCullingTask::~BruteForceCullingTask()
    {
    }
    void BruteForceCullingTask::Run()
    {
        for(i32 i = 0;i < m_Count;i++)
        {
            RenderObject* Obj = m_Parent->GetScene()->GetObjectList()->GetMeshList()[m_StartIndex + i];
            if(Obj->IsAlwaysVisible()) { m_VisibleObjects.push_back(Obj); continue; }
            
            const Frustum* F = m_Parent->GetScene()->GetActiveCamera()->GetFrustum();
            
            //To do: More advanced culling code, not just testing object position
            if(F->ContainsOBB(Obj->GetTransform().GetTranslation(), Obj->GetBoundingBox()))
            {
                m_VisibleObjects.push_back(Obj);
            }
            else m_Parent->SetObjectVisibility(Obj,false);
        }
    }
    
    BruteForceCullingAlgorithm::BruteForceCullingAlgorithm(Scene* s,TaskManager* t) :
        CullingAlgorithm(s), m_TaskManager(t)
    {
    }
    BruteForceCullingAlgorithm::~BruteForceCullingAlgorithm()
    {
    }
    
    CullingResult* BruteForceCullingAlgorithm::PerformCulling()
    {
        Timer tmr;
        tmr.Start();
        
        CullingResult* Result = new CullingResult();
        Result->m_VisibleObjects = new ObjectList();
        Result->m_VisibleObjects->SetIndexed(false);
        
        ObjectList *l = m_Scene->GetObjectList();
        if(l->GetMeshList().size() > m_Scene->GetRenderer()->GetMinObjectCountForMultithreadedCulling())
        {
            vector<BruteForceCullingTask*> Tasks;
            i32 ObjsPerTask = l->GetMeshList().size() / m_TaskManager->GetCoreCount();
            for(i32 i = 0;i < m_TaskManager->GetCoreCount();i++)
            {
                BruteForceCullingTask* Task = new BruteForceCullingTask(this,i * ObjsPerTask,ObjsPerTask);
                m_TaskManager->AddTask(Task,true);
                Tasks.push_back(Task);
            }
            m_TaskManager->WakeThreads   ();
            m_TaskManager->WorkUntilDone ();
            m_TaskManager->WaitForThreads();
            
            Result->m_TotalTaskDuration = 0.0f;
            i32 tobj = 0;
            for(i32 i = 0;i < m_TaskManager->GetCoreCount();i++)
            {
                for(i32 o = 0;o < Tasks[i]->m_VisibleObjects.size();o++)
                {
                    Result->m_VisibleObjects->AddObject(Tasks[i]->m_VisibleObjects[o]);
                    SetObjectVisibility(Tasks[i]->m_VisibleObjects[o],true);
                    tobj++;
                }
                
                Result->m_TotalTaskDuration += Tasks[i]->m_Duration;
            }
            
			if(Result->m_TotalTaskDuration != 0.0f)
			{
				Result->m_AverageTaskDuration = Result->m_TotalTaskDuration / Scalar(m_TaskManager->GetCoreCount());
				Result->m_RealDuration = tmr;
				Result->m_Efficiency = Result->m_TotalTaskDuration / Result->m_RealDuration;
			}
			else
			{
				Result->m_AverageTaskDuration = 0.0f;
				Result->m_RealDuration = tmr;
				Result->m_Efficiency = 0.0f;
			}
		}
        else
        {
            const Frustum* ClipPlanes = m_Scene->GetActiveCamera()->GetFrustum();
            for(i32 i = 0;i < l->GetMeshList().size();i++)
            {
                bool IsVisible = false;
                
                RenderObject* Obj = m_Scene->GetObjectList()->GetMeshList()[i];
                if(Obj->IsAlwaysVisible()) { Result->m_VisibleObjects->AddObject(Obj); continue; }
                
                Mat4 t = Obj->GetTransform();

                if(ClipPlanes->ContainsOBB(Vec3(t.x.w,t.y.w,t.z.w), Obj->GetBoundingBox())) IsVisible = true;
            
                SetObjectVisibility(Obj,IsVisible);
                if(IsVisible) Result->m_VisibleObjects->AddObject(Obj);
            }
            
            Result->m_RealDuration = tmr;
            Result->m_Efficiency = 0.0f;
        }
        
        return Result;
    }
};
