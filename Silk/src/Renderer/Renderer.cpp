#include <Renderer/Renderer.h>
#include <Renderer/Mesh.h>

namespace Silk
{
    Renderer::Renderer(Rasterizer* Raster) : 
        m_ObjectList(new ObjectList()), m_UpdatedObjects(new ObjectList), m_Raster(Raster), m_EngineUniforms(new UniformBuffer()), m_RendererUniforms(new UniformBuffer())
    {}

    Renderer::~Renderer() 
    {
        m_ObjectList->Clear();
        delete m_ObjectList;

        m_UpdatedObjects->Clear();
        delete m_UpdatedObjects;

        delete m_EngineUniforms;
        delete m_RendererUniforms;
    }

    void Renderer::Render(i32 PrimType)
    {
        ObjectVector l = m_ObjectList->GetMeshList();
        for(i32 i = 0; i < l.size(); i++) {
            RenderObject* Obj = l[i];
            if(Obj->m_Mesh && Obj->m_Material && Obj->m_Enabled) 
            {
                i32 Count = Obj->m_Mesh->GetAttribute(0)->Count;
                Obj->m_ObjectIdentifier->Render(PrimType, 0, Count);
            }
        }

        m_UpdatedObjects->Clear();
    }

    RenderObject* Renderer::CreateRenderObject(RENDER_OBJECT_TYPE Rot, bool AddToScene)
    {
        RenderObject* Object = new RenderObject(Rot, this, m_Raster->CreateObjectIdentifier());
        if(!Object)
            return nullptr;

        if(AddToScene)
            AddRenderObject(Object);

        return Object;
    }
};
