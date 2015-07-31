#include <Renderer/SyncInstanceTransformsTask.h>
#include <Raster/Raster.h>

namespace Silk
{
    void SyncInstanceTransformsTask::Run()
    {
        for(i32 i = 0;i < m_Count;i++)
        {
            RenderObject* Obj = (*m_Mesh->GetInstanceList())[m_StartIndex + i];
            if(Obj->GetCulledInstanceIndex() == -1) continue;
            
            Obj->GetObject()->SetInstanceTransform       (Obj->GetCulledInstanceIndex(),Obj->GetTransform       ());
            Obj->GetObject()->SetInstanceNormalTransform (Obj->GetCulledInstanceIndex(),Obj->GetNormalTransform ());
            Obj->GetObject()->SetInstanceTextureTransform(Obj->GetCulledInstanceIndex(),Obj->GetTextureTransform());
        }
    }
};
