#include <Renderer/DeferredRenderer.h>
#include <Renderer/Mesh.h>
#include <Renderer/Texture.h>

#include <Raster/Raster.h>
#include <Raster/OpenGL/OpenGLShader.h>

namespace Silk
{
    //Deferred
    void DeferredRenderer::RenderObjects(ObjectList *List,PRIMITIVE_TYPE PrimType)
    {
        SilkObjectVector Lights = List->GetLightList();

        i32 ShaderCount = List->GetShaderCount();

        SilkObjectVector MeshesRendered;
        i32 ActualObjectCount = 0;
        i32 VertexCount       = 0;
        i32 TriangleCount     = 0;

        for(i32 i = 0;i < ShaderCount;i++)
        {
            Shader* Shader = List->GetShader(i);
            if(!Shader) continue;

            Shader->Enable();

            SilkObjectVector Meshes = List->GetShaderMeshList(i);
            for(i32 m = 0;m < Meshes.size();m++)
            {
                RenderObject* Obj = Meshes[m];
                if(Obj->IsInstanced() && Obj->m_Mesh->m_LastFrameRendered == m_Stats.FrameID) continue;
                else if(Obj->IsInstanced()) Obj = (*Obj->GetMesh()->GetInstanceList())[0];

                Obj->m_Mesh->m_LastFrameRendered = m_Stats.FrameID;

                if(Obj->m_Mesh && Obj->m_Material && Obj->m_Enabled)
                {
                    //Pass material uniforms
                    Material* Mat = Obj->GetMaterial();
                    /*if(Mat->HasUpdated())*/ Shader->UseMaterial(Obj->GetMaterial());

                    //Pass object uniforms
                    if(Shader->UsesUniformInput(ShaderGenerator::IUT_OBJECT_UNIFORMS))
                    {
                        if(Obj->IsInstanced())
                        {
                            Mat4 tmp = Obj->GetTransform();
                            Obj->SetTransform(Mat4::Identity);
                            Obj->UpdateUniforms();
                            Shader->PassUniforms(Obj->GetUniformSet()->GetUniforms());
                            Obj->SetTransform(tmp);
                        }
                        else
                        {
                            Obj->UpdateUniforms();
                            Shader->PassUniforms(Obj->GetUniformSet()->GetUniforms());
                        }
                    }

                    //To do: Batching
                    i32 Count = 0;
                    if(Obj->m_Mesh->IsIndexed()) Count = Obj->m_Mesh->GetIndexCount();
                    else Count = Obj->m_Mesh->GetVertexCount();

                    PRIMITIVE_TYPE p = Obj->m_Mesh->PrimitiveType == PT_COUNT ? PrimType : Obj->m_Mesh->PrimitiveType;
                    Obj->m_Object->Render(Obj,p,0,Count);

                    i32 vc = Obj->m_Mesh->GetVertexCount();
                    i32 tc = 0;
                    if(p == PT_TRIANGLES     ) tc = vc / 3;
                    if(p == PT_TRIANGLE_STRIP
                    || p == PT_TRIANGLE_FAN  ) tc = vc - 2;

                    if(Obj->IsInstanced())
                    {
                        i32 InstanceCount = Obj->GetMesh()->m_VisibleInstanceCount;
                        ActualObjectCount +=      InstanceCount;
                        VertexCount       += vc * InstanceCount;
                        TriangleCount     += tc * InstanceCount;
                    }
                    else
                    {
                        ActualObjectCount++;
                        VertexCount   += vc;
                        TriangleCount += tc;
                    }

                    MeshesRendered.push_back(Obj);
                }
            }

            Shader->Disable();
        }

        for(i32 i = 0;i < MeshesRendered.size();i++)
        {
            MeshesRendered[i]->GetUniformSet()->GetUniforms()->ClearUpdatedUniforms();

            if(m_DebugDrawer)
            {
                if(MeshesRendered[i]->IsInstanced())
                {
                    i32 ic = 0;
                    const vector<RenderObject*>* iList = MeshesRendered[i]->m_Mesh->GetInstanceList();
                    for(i32 c = 0;c < iList->size();c++)
                    {
                        if((*iList)[c]->m_CulledInstanceIndex == -1) continue;

                        m_DebugDrawer->Transform((*iList)[c]->GetTransform());
                        m_DebugDrawer->AABB     (MeshesRendered[i]->GetTransform(),(*iList)[c]->GetBoundingBox().ComputeWorldAABB(),Vec4(1,1,1,1));
                        ic++;
                    }
                }
                else
                {
                    m_DebugDrawer->Transform(MeshesRendered[i]->GetTransform());
                    m_DebugDrawer->AABB     (MeshesRendered[i]->GetTransform(),MeshesRendered[i]->GetBoundingBox().ComputeWorldAABB(),Vec4(1,1,1,1));
                    m_DebugDrawer->OBB      (MeshesRendered[i]->GetBoundingBox(),Vec4(1,0,0,1));
                }
            }
        }

        m_Stats.DrawCalls      += MeshesRendered.size();
        m_Stats.VisibleObjects += ActualObjectCount;
        m_Stats.VertexCount    += VertexCount;
        m_Stats.TriangleCount  += TriangleCount;

        m_SceneOutput->Disable();
        RenderTexture(m_SceneOutput->GetAttachment(ShaderGenerator::OUTPUT_FRAGMENT_TYPE::OFT_NORMAL));
    }
};
