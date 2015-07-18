#include <Renderer/Scene.h>
#include <Renderer/RenderObject.h>
#include <Renderer/CullingAlgorithm.h>

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
        return m_CullingAlgorithm->PerformCulling();
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
