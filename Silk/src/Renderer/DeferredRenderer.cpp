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
        m_PostProcessingInherited = true;
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
        //Populate gbuffer
        GeometryPass(List);
        
        //Use gbuffer for lighting
        LightPass(List->GetLightList());
        
        //TODO: Abstract all GL calls
        //Note: These GL calls need to be moved to the Renderer::RenderTexture
        //      function once the context state machine is written.
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glDisable(GL_CULL_FACE);
        
        m_SceneOutput->EnableTexture(m_FinalPassMat);
        RenderTexture(0,m_FinalPassMat);
        
        if(m_UsePostProcessing && m_Effects.size() > 0)
        {   
            for(i32 i = 0;i < m_Effects.size();i++) m_Effects[i]->Execute();
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_ALPHA);
            glBlendEquation(GL_ADD);
            for(i32 i = 0; i < m_Effects.size();i++) RenderTexture(m_Effects[i]->GetOutput());
            glDisable(GL_BLEND);
        }
        
        glEnable(GL_CULL_FACE);
        
        if(m_DebugDrawer)
        {
            m_DebugDrawer->GetObject()->GetMaterial()->GetShader()->Enable();
            RenderSingleObject(m_DebugDrawer->GetObject());
            m_DebugDrawer->GetObject()->GetMaterial()->GetShader()->Disable();
        }
        
        glEnable(GL_DEPTH_TEST);
    }
    void DeferredRenderer::GeometryPass(ObjectList *List)
    {
        m_SceneOutput->EnableTarget();
        i32 ShaderCount = List->GetShaderCount();
        
        SilkObjectVector MeshesRendered;
        
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
        
        if(m_DebugDrawer)
        {
            for(i32 i = 0;i < MeshesRendered.size();i++)
            {
                if(MeshesRendered[i]->IsInstanced())
                {
                    i32 ic = 0;
                    const vector<RenderObject*>* iList = MeshesRendered[i]->m_Mesh->GetInstanceList();
                    for(i32 c = 0;c < iList->size();c++)
                    {
                        if((*iList)[c]->m_CulledInstanceIndex == -1) continue;
                        
                        if(m_DebugDrawer->GetDebugDisplay(DebugDrawer::DD_TRANSFORM))m_DebugDrawer->Transform((*iList)[c]->GetTransform());
                        if(m_DebugDrawer->GetDebugDisplay(DebugDrawer::DD_AABB     ))m_DebugDrawer->AABB     (MeshesRendered[i]->GetTransform(),(*iList)[c]->GetBoundingBox().ComputeWorldAABB(),Vec4(1,1,1,1));
                        if(m_DebugDrawer->GetDebugDisplay(DebugDrawer::DD_OBB      ))m_DebugDrawer->OBB      ((*iList)[c]->GetBoundingBox(),Vec4(1,1,1,1));
                        ic++;
                    }
                }
                else
                {
                    if(m_DebugDrawer->GetDebugDisplay(DebugDrawer::DD_TRANSFORM)) m_DebugDrawer->Transform(MeshesRendered[i]->GetTransform());
                    if(m_DebugDrawer->GetDebugDisplay(DebugDrawer::DD_AABB     )) m_DebugDrawer->AABB     (MeshesRendered[i]->GetTransform(),MeshesRendered[i]->GetBoundingBox().ComputeWorldAABB(),Vec4(1,1,1,1));
                    if(m_DebugDrawer->GetDebugDisplay(DebugDrawer::DD_OBB      )) m_DebugDrawer->OBB      (MeshesRendered[i]->GetBoundingBox(),Vec4(1,0,0,1));
                }
            }
        }
        m_SceneOutput->Disable();
    }
    void DeferredRenderer::LightPass(const SilkObjectVector& Lights)
    {
        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glCullFace(GL_FRONT);
        
        
        vector<RenderObject*> PointLights;
        vector<RenderObject*> SpotLights;
        vector<RenderObject*> DirLights;
        
        for(i32 l = 0;l < Lights.size();l++)
        {
            if     (Lights[l]->GetLight()->m_Type == LT_POINT      ) PointLights.push_back(Lights[l]);
            else if(Lights[l]->GetLight()->m_Type == LT_SPOT       ) SpotLights .push_back(Lights[l]);
            else if(Lights[l]->GetLight()->m_Type == LT_DIRECTIONAL) DirLights  .push_back(Lights[l]);
        }
        
        
        m_LightAccumulationBuffer->EnableRTT(false);
        if(PointLights.size() > 0)
        {
            m_PointLightMat->GetShader()->Enable();
            m_SceneOutput->EnableTexture(m_PointLightMat);
            m_PointLightMat->GetShader()->UseMaterial(m_PointLightMat);
            
            for(i32 i = 0;i < PointLights.size();i++)
            {
                vector<Light*>Lt; Lt.push_back(PointLights[i]->GetLight());
                if(m_PointLightObj)
                {
                    Mat4 T = PointLights[i]->GetTransform();
                    PointLights[i]->GetLight()->m_Position  = Vec4(T.GetTranslation(),1.0f);
                    PointLights[i]->GetLight()->m_Direction = Vec4(T.GetZ(),1.0f);
        
					Vec3 Color = PointLights[i]->GetLight()->m_Color.xyz();
                    Scalar Radius = (1.0 / sqrt(PointLights[i]->GetLight()->m_Attenuation.Exponential))*3;
                    m_PointLightObj->SetTransform(T * Scale(Radius));
                    
                    if(m_DebugDrawer) m_DebugDrawer->DrawMesh(T,m_PointLightObj->GetMesh(),Vec4(Color,1.0f));
                
                    m_PointLightObj->GetUniformSet()->SetLights(Lt);
                    
                    glEnable(GL_CULL_FACE);
                    glCullFace(GL_FRONT);
                }
                else
                {
                    glDisable(GL_CULL_FACE);
                    m_FSQ->GetUniformSet()->SetLights(Lt);
                }
                
                RenderLight(m_PointLightMat->GetShader(),PointLights[i],m_PointLightObj);
            }
            
            m_PointLightMat->GetShader()->Disable();
        }
        if(SpotLights.size() > 0)
        {
            m_SpotLightMat->GetShader()->Enable();
            m_SceneOutput->EnableTexture(m_SpotLightMat);
            m_SpotLightMat->GetShader()->UseMaterial(m_SpotLightMat);
            
            for(i32 i = 0;i < SpotLights.size();i++)
            {
                vector<Light*>Lt; Lt.push_back(SpotLights[i]->GetLight());
                if(m_SpotLightObj)
                {
                    Mat4 T = SpotLights[i]->GetTransform();
                    SpotLights[i]->GetLight()->m_Position  = Vec4(T.GetTranslation(),1.0f);
                    SpotLights[i]->GetLight()->m_Direction = Vec4(T.GetZ(),1.0f);
        
					Vec3 Color = SpotLights[i]->GetLight()->m_Color.xyz();
                    Scalar Radius = (1.0 / sqrt(SpotLights[i]->GetLight()->m_Attenuation.Exponential * 0.001f));
                    Scalar BaseScale = tan(SpotLights[i]->GetLight()->m_Cutoff * PI_OVER_180) * Radius;
                    m_SpotLightObj->SetTransform(T * Scale(Vec3(BaseScale,BaseScale,Radius)));
                    
                    if(m_DebugDrawer) m_DebugDrawer->DrawMesh(T,m_SpotLightObj->GetMesh(),Vec4(0.5f,0.7f,1.0f,1.0f));
                   
                    m_SpotLightObj->GetUniformSet()->SetLights(Lt);
                
                    glEnable(GL_CULL_FACE);
                    glCullFace(GL_FRONT);
                }
                else
                {
                    glDisable(GL_CULL_FACE);
                    m_FSQ->GetUniformSet()->SetLights(Lt);
                }
                
                RenderLight(m_SpotLightMat->GetShader(),SpotLights[i],m_SpotLightObj);
            }
            
            m_SpotLightMat->GetShader()->Disable();
        }
        if(DirLights.size() > 0)
        {
            m_DirectionalLightMat->GetShader()->Enable();
            m_SceneOutput->EnableTexture(m_DirectionalLightMat);
            m_DirectionalLightMat->GetShader()->UseMaterial(m_DirectionalLightMat);
            
            for(i32 i = 0;i < DirLights.size();i++)
            {
                vector<Light*>Lt; Lt.push_back(DirLights[i]->GetLight());
                if(m_DirectionalLightObj)
                {
                    Mat4 T = DirLights[i]->GetTransform();
                    DirLights[i]->GetLight()->m_Position  = Vec4(T.GetTranslation(),1.0f);
                    DirLights[i]->GetLight()->m_Direction = Vec4(T.GetZ(),1.0f);
        
					Vec3 Color = DirLights[i]->GetLight()->m_Color.xyz();
                    
                    if(m_DebugDrawer) m_DebugDrawer->DrawMesh(T,m_DirectionalLightObj->GetMesh(),Vec4(Color,1.0f));
                   
                    m_DirectionalLightObj->GetUniformSet()->SetLights(Lt);
                
                    glEnable(GL_CULL_FACE);
                    glCullFace(GL_FRONT);
                }
                else
                {
                    glDisable(GL_CULL_FACE);
                    m_FSQ->GetUniformSet()->SetLights(Lt);
                }
                
                RenderLight(m_DirectionalLightMat->GetShader(),DirLights[i],m_DirectionalLightObj);
            }
            
            m_DirectionalLightMat->GetShader()->Disable();
        }
        m_LightAccumulationBuffer->DisableRTT();
        
        glCullFace(GL_BACK);
    }
    void DeferredRenderer::RenderLight(Shader* S,RenderObject* Lt,RenderObject* Obj)
    {
        if(Obj)
        {
            if(S->UsesUniformInput(ShaderGenerator::IUT_OBJECT_UNIFORMS))
            {
                Obj->UpdateUniforms();
                S->PassUniforms(Obj->GetUniformSet()->GetUniforms());
            }
            
            i32 Count = 0;
            if(Obj->m_Mesh->IsIndexed()) Count = Obj->m_Mesh->GetIndexCount();
            else Count = Obj->m_Mesh->GetVertexCount();
            
            PRIMITIVE_TYPE p = Obj->m_Mesh->PrimitiveType == PT_COUNT ? PT_TRIANGLES : Obj->m_Mesh->PrimitiveType;
            Obj->m_Object->Render(Obj,p,0,Count);
            
            i32 vc = Obj->m_Mesh->GetVertexCount();
            i32 tc = 0;
            if(p == PT_TRIANGLES     ) tc = vc / 3;
            if(p == PT_TRIANGLE_STRIP
            || p == PT_TRIANGLE_FAN  ) tc = vc - 2;
            
            m_Stats.DrawCalls     += 1;
            m_Stats.VertexCount   += vc;
            m_Stats.TriangleCount += tc;
        }
        else
        {
            m_FSQ->UpdateUniforms();
            S->PassUniforms(m_FSQ->GetUniformSet()->GetUniforms());
            RasterObject* O = m_FSQ->GetObject();
            O->Render(m_FSQ,PT_TRIANGLE_FAN,0,4);
            
            m_Stats.DrawCalls     += 1;
            m_Stats.VertexCount   += 4;
            m_Stats.TriangleCount += 2;
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
        m_LightAccumulationBuffer->CreateTexture(r.x,r.y,Texture::PT_FLOAT);
        m_LightAccumulationBuffer->UpdateTexture();
    }
};
