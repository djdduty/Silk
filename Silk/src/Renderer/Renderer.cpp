#include <Renderer/Renderer.h>
#include <Renderer/Mesh.h>
#include <Raster/Raster.h>

#include <Raster/OpenGL/OpenGLShader.h>

namespace Silk
{
    Renderer::Renderer(Rasterizer* Raster) : 
        m_ObjectList(new ObjectList()), m_UpdatedObjects(new ObjectList), m_Raster(Raster)
    {
        m_EngineUniforms   = m_Raster->CreateUniformBuffer(ShaderGenerator::IUT_ENGINE_UNIFORMS  );
        m_RendererUniforms = m_Raster->CreateUniformBuffer(ShaderGenerator::IUT_RENDERER_UNIFORMS);
    }

    Renderer::~Renderer() 
    {
        m_ObjectList->Clear();
        delete m_ObjectList;

        m_UpdatedObjects->Clear();
        delete m_UpdatedObjects;

        m_Raster->DestroyUniformBuffer(m_EngineUniforms);
        m_Raster->DestroyUniformBuffer(m_RendererUniforms);
    }

    void Renderer::Render(i32 PrimType)
    {
        i32 ShaderCount = m_ObjectList->GetShaderCount();
        for(i32 i = 0;i < ShaderCount;i++)
        {
            Shader*      Shader = m_ObjectList->GetShader        (i);
            ObjectVector Meshes = m_ObjectList->GetShaderMeshList(i);
            
            Shader->Enable();
            
            for(i32 m = 0;m < Meshes.size();m++)
            {
                RenderObject* Obj = Meshes[m];
                if(Obj->m_Mesh && Obj->m_Material && Obj->m_Enabled)
                {
                    i32 Count = Obj->m_Mesh->GetAttribute(0)->Count;
                    Obj->m_ObjectIdentifier->Render(PrimType, 0, Count);
                }
            }
            
            Shader->Disable();
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
