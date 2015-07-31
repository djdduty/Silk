#include <Renderer/CullingAlgorithm.h>
#include <Renderer/RenderObject.h>
#include <Raster/Raster.h>

namespace Silk
{
    void CullingAlgorithm::SetObjectVisibility(RenderObject *Obj,bool Visibility)
    {
        if(Visibility)
        {
            if(Obj->GetMesh())
            {
                Obj->m_CulledInstanceIndex = Obj->GetMesh()->m_VisibleInstanceCount;
                Obj->GetMesh()->m_VisibleInstanceCount++;
            }
            else
            {
            }
        }
        else
        {
            if(Obj->GetMesh())
            {
                Obj->m_CulledInstanceIndex = -1;
            }
            else
            {
            }
        }
        
    }
    CullingResult::CullingResult()
    {
    }
    CullingResult::~CullingResult()
    {
        if(m_VisibleObjects) delete m_VisibleObjects;
    }
};
