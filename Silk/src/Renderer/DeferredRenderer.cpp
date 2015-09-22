#include <Renderer/DeferredRenderer.h>
#include <Renderer/Mesh.h>
#include <Renderer/Texture.h>

#include <Raster/Raster.h>
#include <Raster/OpenGL/OpenGLShader.h>

namespace Silk
{
    DeferredRenderer::DeferredRenderer(Rasterizer* Raster,TaskManager* TaskMgr) :
        Renderer(Raster, TaskMgr), m_PointLightObj(0), m_SpotLightObj(0), m_DirectionalLightObj(0),
        m_PointLightMat(0), m_SpotLightMat(0), m_DirectionalLightMat(0)
    {
    }
    void DeferredRenderer::RenderObjects(ObjectList *List,PRIMITIVE_TYPE PrimType,bool SendLighting)
    {
        m_SceneOutput->EnableTarget();
        Renderer::RenderObjects(List,PrimType,false);
        m_SceneOutput->Disable();
        
        //RenderTexture(m_SceneOutput->GetAttachment(ShaderGenerator::OUTPUT_FRAGMENT_TYPE::OFT_NORMAL));
        
        SilkObjectVector Lights = List->GetLightList();
        //RenderTexture(m_SceneOutput->GetAttachment(ShaderGenerator::OFT_COLOR));
        glEnable(GL_BLEND); //TODO: Abstract this
        glDisable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        for(i32 l = 0;l < Lights.size();l++)
        {
            LightPass(Lights[l]);
        }
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
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
                Obj->GetUniformSet()->SetLights(Lt);
                RenderTexture(0,m_PointLightMat,Obj);
                break;
            }
            case LT_SPOT:
            {
                if(!m_SpotLightMat) return;
                m_SceneOutput->EnableTexture(m_SpotLightMat);
                RenderObject* Obj = m_SpotLightObj ? m_SpotLightObj : m_FSQ;
                Obj->GetUniformSet()->SetLights(Lt);
                RenderTexture(0,m_SpotLightMat,Obj);
                break;
            }
            case LT_DIRECTIONAL:
            {
                if(!m_DirectionalLightMat) return;
                m_SceneOutput->EnableTexture(m_DirectionalLightMat);
                RenderObject* Obj = m_DirectionalLightObj ? m_DirectionalLightObj : m_FSQ;
                Obj->GetUniformSet()->SetLights(Lt);
                RenderTexture(0,m_DirectionalLightMat,Obj);
                break;
            }
            default:
            {
                break;
            }
        }
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
};
