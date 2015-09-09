#include <Renderer/Scene.h>
#include <Renderer/RenderObject.h>
#include <Renderer/CullingAlgorithm.h>

#include <Raster/Raster.h>

namespace Silk
{
    Scene::Scene(Renderer* r) : m_Renderer(r), m_ActiveCamera(0), m_CullingAlgorithm(0), m_ObjectList(new ObjectList())
    {
    }
    Scene::~Scene()
    {
        m_ObjectList->Clear();
        delete m_ObjectList;
    }
    
    CullingResult* Scene::PerformCulling()
    {
        for(i32 i = 0;i < m_ObjectList->GetSceneMeshes()->size();i++)
        {
            /* Reset the visibility state of all instances */
            (*m_ObjectList->GetSceneMeshes())[i]->m_VisibleInstanceCount = 0;
        }
        
        CullingResult* r = m_CullingAlgorithm->PerformCulling();
        
        for(i32 i = 0;i < m_ObjectList->GetSceneMeshes()->size();i++)
        {
            /* Update instance data */
            Mesh* m = (*m_ObjectList->GetSceneMeshes())[i];
            if(m->IsInstanced()) m->m_Obj->m_Object->UpdateInstanceData();
        }
        
        return r;
    }
    void Scene::AddRenderObject(RenderObject *Object)
    {
        if(!Object) return;
        
        Object->m_ListIndex = m_ObjectList->AddObject(Object);
        Object->m_List = m_ObjectList;
    }
    void Scene::RemoveRenderObject(RenderObject *Object)
    {
        m_ObjectList->RemoveObject(Object);
    }
};
