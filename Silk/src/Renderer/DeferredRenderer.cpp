#include <Renderer/DeferredRenderer.h>
#include <Renderer/Mesh.h>
#include <Renderer/Texture.h>

#include <Raster/Raster.h>
#include <Raster/OpenGL/OpenGLShader.h>

namespace Silk
{
    DeferredRenderer::DeferredRenderer(Rasterizer* Raster,TaskManager* TaskMgr) :
        Renderer(Raster, TaskMgr), m_PointLightObj(0), m_SpotLightObj(0), m_DirectionalLightObj(0),
        m_PointLightMat(0), m_SpotLightMat(0), m_DirectionalLightMat(0), m_FinalPassMat(0), m_LightAccumulationBuffer(0)
    {
        m_LightAccumulationBuffer = m_Raster->CreateTexture();
        OnResolutionChanged();
    }
    DeferredRenderer::~DeferredRenderer()
    {
        m_LightAccumulationBuffer->Destroy();
        if(m_PointLightMat      ) m_PointLightMat      ->Destroy();
        if(m_SpotLightMat       ) m_SpotLightMat       ->Destroy();
        if(m_DirectionalLightMat) m_DirectionalLightMat->Destroy();
        if(m_FinalPassMat       ) m_FinalPassMat       ->Destroy();
    }
    void DeferredRenderer::RenderObjects(ObjectList *List,PRIMITIVE_TYPE PrimType,bool SendLighting)
    {
        m_SceneOutput->EnableTarget();
        /*
         * To do:
         * Determine which lights affect which objects using a scene octree
         * then call Object->GetUniformSet()->SetLights(ObjectLightVector);
         */
        
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
                if(m_DebugDrawer && Obj == m_DebugDrawer->GetObject()) continue;
                if(!Obj->IsEnabled()) continue;
                if(Obj->IsInstanced() && Obj->m_Mesh->m_LastFrameRendered == m_Stats.FrameID) continue;
                else if(Obj->IsInstanced()) Obj = (*Obj->GetMesh()->GetInstanceList())[0];
                
                Obj->m_Mesh->m_LastFrameRendered = m_Stats.FrameID;
            
                if(Obj->m_Mesh && Obj->m_Material && Obj->m_Enabled)
                {
                    RenderSingleObject(Obj);
                    
                    MeshesRendered.push_back(Obj);
                }
            }
            
            Shader->Disable();
        }
        m_SceneOutput->Disable();
        
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
        
        SilkObjectVector Lights = List->GetLightList();
        
        //TODO: Abstract all GL calls
        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glCullFace(GL_FRONT);
        
        m_LightAccumulationBuffer->EnableRTT(false);
        
        for(i32 l = 0;l < Lights.size();l++)
        {
            LightPass(Lights[l]);
        }
        
        m_LightAccumulationBuffer->DisableRTT();
        
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glCullFace(GL_BACK);
        
        //Note: These GL calls need to be moved to the Renderer::RenderTexture
        //      function once the context state machine is written.
        m_SceneOutput->EnableTexture(m_FinalPassMat);
        RenderTexture(0,m_FinalPassMat);
        
        glEnable (GL_CULL_FACE);
        
        if(m_DebugDrawer)
        {
            glDisable(GL_DEPTH_TEST);
            m_DebugDrawer->GetObject()->GetMaterial()->GetShader()->Enable();
            RenderSingleObject(m_DebugDrawer->GetObject());
            m_DebugDrawer->GetObject()->GetMaterial()->GetShader()->Disable();
            glEnable(GL_DEPTH_TEST);
        }
    }
    void DeferredRenderer::LightPass(RenderObject* l)
    {
        vector<Light*>Lt;
        Lt.push_back(l->GetLight());
        
        Mat4 T = l->GetTransform();
        l->GetLight()->m_Position  = Vec4(T.GetTranslation(),1.0f);
        l->GetLight()->m_Direction = Vec4(T.GetZ(),1.0f);
    
        switch(l->GetLight()->m_Type)
        {
            case LT_POINT:
            {
                if(!m_PointLightMat) return;
                m_SceneOutput->EnableTexture(m_PointLightMat);
                RenderObject* Obj = m_PointLightObj ? m_PointLightObj : m_FSQ;
                if(Obj != m_FSQ)
                {
                    Scalar Radius = (1.0 / sqrt(l->GetLight()->m_Attenuation.Exponential * 0.01f));
                    Obj->SetTransform(Scale(Radius));
                }
                
                Obj->GetUniformSet()->SetLights(Lt);
                
                if(Obj != m_FSQ && m_DebugDrawer) m_DebugDrawer->DrawMesh(Obj->GetTransform(),Obj->GetMesh(),Vec4(0.5,0.7,1.0,1.0));

                if(Obj == m_FSQ) glDisable(GL_CULL_FACE);
                RenderTexture(0,m_PointLightMat,Obj);
                if(Obj == m_FSQ) glEnable(GL_CULL_FACE);
                break;
            }
            case LT_SPOT:
            {
                if(!m_SpotLightMat) return;
                m_SceneOutput->EnableTexture(m_SpotLightMat);
                RenderObject* Obj = m_SpotLightObj ? m_SpotLightObj : m_FSQ;
                if(Obj != m_FSQ)
                {
                    Scalar Radius = (1.0 / sqrt(l->GetLight()->m_Attenuation.Exponential * 0.001f));
                    Scalar BaseScale = tan(l->GetLight()->m_Cutoff * PI_OVER_180) * Radius * 2.0f;
                    Obj->SetTransform(T * Scale(Vec3(BaseScale,BaseScale,Radius)));
                }
                
                Obj->GetUniformSet()->SetLights(Lt);
                
                if(Obj != m_FSQ && m_DebugDrawer) m_DebugDrawer->DrawMesh(Obj->GetTransform(),Obj->GetMesh(),Vec4(0.5,0.7,1.0,1.0));

                if(Obj == m_FSQ) glDisable(GL_CULL_FACE);
                RenderTexture(0,m_SpotLightMat,Obj);
                if(Obj == m_FSQ) glEnable(GL_CULL_FACE);
                break;
            }
            case LT_DIRECTIONAL:
            {
                if(!m_DirectionalLightMat) return;
                m_SceneOutput->EnableTexture(m_DirectionalLightMat);
                RenderObject* Obj = m_DirectionalLightObj ? m_DirectionalLightObj : m_FSQ;
                
                if(Obj != m_FSQ) Obj->SetTransform(T);
                
                Obj->GetUniformSet()->SetLights(Lt);
                
                if(Obj != m_FSQ && m_DebugDrawer) m_DebugDrawer->DrawMesh(T * Obj->GetTransform(),Obj->GetMesh(),Vec4(0.5,0.7,1.0,1.0));

                if(Obj == m_FSQ) glDisable(GL_CULL_FACE);
                RenderTexture(0,m_DirectionalLightMat,Obj);
                if(Obj == m_FSQ) glEnable(GL_CULL_FACE);
                
                break;
            }
            default:
            {
                break;
            }
        }
    }
    void DeferredRenderer::RenderSingleObject(RenderObject* Obj)
    {
        //Pass material uniforms
        Material* Mat = Obj->GetMaterial();
        Shader* Shader = Mat->GetShader();
        /*if(Mat->HasUpdated())*/ Shader->UseMaterial(Mat);
        
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
        
        PRIMITIVE_TYPE p = Obj->m_Mesh->PrimitiveType;
        Obj->m_Object->Render(Obj,p,0,Count);
        
        i32 vc = Obj->m_Mesh->GetVertexCount();
        i32 tc = 0;
        if(p == PT_TRIANGLES     ) tc = vc / 3;
        if(p == PT_TRIANGLE_STRIP
        || p == PT_TRIANGLE_FAN  ) tc = vc - 2;
        
        if(Obj->IsInstanced())
        {
            i32 InstanceCount = Obj->GetMesh()->m_VisibleInstanceCount;
            m_Stats.VisibleObjects +=      InstanceCount;
            m_Stats.VertexCount    += vc * InstanceCount;
            m_Stats.TriangleCount  += tc * InstanceCount;
        }
        else
        {
            m_Stats.VisibleObjects++;
            m_Stats.VertexCount   += vc;
            m_Stats.TriangleCount += tc;
        }
        m_Stats.DrawCalls++;
    }
    
    void DeferredRenderer::SetPointLightObject        (RenderObject* Obj)
    {
        m_PointLightObj = Obj;
    }
    void DeferredRenderer::SetSpotLightObject         (RenderObject* Obj)
    {
        m_SpotLightObj = Obj;
    }
    void DeferredRenderer::SetDirectionalLightObject  (RenderObject* Obj)
    {
        m_DirectionalLightObj = Obj;
    }
    void DeferredRenderer::SetPointLightMaterial      (Material*     Mat)
    {
        if(m_PointLightMat) m_PointLightMat->Destroy();
        m_PointLightMat = Mat;
        Mat->AddRef();
    }
    void DeferredRenderer::SetSpotLightMaterial       (Material*     Mat)
    {
        if(m_SpotLightMat) m_SpotLightMat->Destroy();
        m_SpotLightMat = Mat;
        Mat->AddRef();
    }
    void DeferredRenderer::SetDirectionalLightMaterial(Material*     Mat)
    {
        if(m_DirectionalLightMat) m_DirectionalLightMat->Destroy();
        m_DirectionalLightMat = Mat;
        Mat->AddRef();
    }
    void DeferredRenderer::SetFinalPassMaterial(Material*     Mat)
    {
        if(m_FinalPassMat) m_FinalPassMat->Destroy();
        m_FinalPassMat = Mat;
        Mat->AddRef();
    }
    void DeferredRenderer::OnResolutionChanged()
    {
        Vec2 r = m_Raster->GetContext()->GetResolution();
        m_LightAccumulationBuffer->CreateTexture(r.x,r.y,Texture::PT_UNSIGNED_BYTE);
        m_LightAccumulationBuffer->UpdateTexture();
    }
};
