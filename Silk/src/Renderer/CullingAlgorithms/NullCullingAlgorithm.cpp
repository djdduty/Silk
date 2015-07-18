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
        Result->m_RealDuration = 0.0f;
        Result->m_VisibleObjects = new ObjectList(*m_Scene->GetObjectList());
        return Result;
    }
};
