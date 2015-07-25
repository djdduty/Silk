#include <Renderer/CullingAlgorithms/NullCullingAlgorithm.h>
#include <Renderer/Renderer.h>

namespace Silk
{
    NullCullingAlgorithm::NullCullingAlgorithm(Scene* s) : CullingAlgorithm(s)
    {
    }
    NullCullingAlgorithm::~NullCullingAlgorithm()
    {
    }
    
    CullingResult* NullCullingAlgorithm::PerformCulling()
    {
        CullingResult* Result = new CullingResult();
        Result->m_AverageTaskDuration = 0.0f;
        Result->m_VisibleObjects = new ObjectList();
        Result->m_VisibleObjects->SetIndexed(false);
        
        Timer tmr;
        tmr.Start();
        for(i32 i = 0;i < m_Scene->GetObjectList()->GetMeshList().size();i++)
        {
            Result->m_VisibleObjects->AddObject(m_Scene->GetObjectList()->GetMeshList()[i]);
        }
        Result->m_RealDuration = tmr;
        return Result;
    }
};
