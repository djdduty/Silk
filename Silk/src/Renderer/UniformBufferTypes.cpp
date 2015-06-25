#include <Renderer/UniformBufferTypes.h>
#include <Raster/Raster.h>

namespace Silk
{
    ModelUniformSet::ModelUniformSet(Renderer* r) : m_Renderer(r)
    {
        m_UniformBuffer = r->GetRasterizer()->CreateUniformBuffer(ShaderGenerator::IUT_OBJECT_UNIFORMS);
        m_iMV                  = m_UniformBuffer->DefineUniform("u_MV"        );
        m_iMVP                 = m_UniformBuffer->DefineUniform("u_MVP"       );
        m_iModel               = m_UniformBuffer->DefineUniform("u_Model"     );
        m_iTexture             = m_UniformBuffer->DefineUniform("u_Texture"   );
        m_iNormal              = m_UniformBuffer->DefineUniform("u_Normal"    );
        m_iLightInfluences     = m_UniformBuffer->DefineUniform("u_Lights"    );
        m_iLightInfluenceCount = m_UniformBuffer->DefineUniform("u_LightCount");
        
        m_NullLight = new Light();
        m_NullLight->m_Power = 0.0f;
        m_NullLight->m_Color = Vec4(0,0,0,0);
        
        SetLights(vector<Light*>());
        UpdateUniforms(); //Must be called here to set the uniform types
    }
    ModelUniformSet::~ModelUniformSet()
    {
        m_Renderer->GetRasterizer()->Destroy(m_UniformBuffer);
        delete m_NullLight;
    }

    void ModelUniformSet::SetLights(const vector<Light*>& Lights)
    {
        m_Lights = Lights;
        if(m_Lights.size() > m_Renderer->GetMaxLights())
        {
            ERROR("Cannot pass more than %d lights to shader, increase max lights or decrease light count.\n",m_Renderer->GetMaxLights());
            ERROR("Using first %d lights for render.\n",m_Renderer->GetMaxLights());
            
            m_Lights.erase(m_Lights.begin() + m_Renderer->GetMaxLights(),m_Lights.end());
        }
        else if(m_Lights.size() < m_Renderer->GetMaxLights())
        {
            /*
             * Since the shaders always expect the "u_Lights" array to
             * be filled, we must pad the array with null values here.
             */
             m_LightCount = m_Lights.size();
             while(m_Lights.size() < m_Renderer->GetMaxLights()) m_Lights.push_back(m_NullLight);
        }
    }

    void ModelUniformSet::UpdateUniforms()
    {
        m_UniformBuffer->SetUniform(m_iMV                 ,MV          );
        m_UniformBuffer->SetUniform(m_iMVP                ,MVP         );
        m_UniformBuffer->SetUniform(m_iNormal             ,Normal      );
        m_UniformBuffer->SetUniform(m_iModel              ,Model       );
        m_UniformBuffer->SetUniform(m_iTexture            ,Texture     );
        m_UniformBuffer->SetUniform(m_iLightInfluenceCount,m_LightCount);
        m_UniformBuffer->SetUniform(m_iLightInfluences    ,m_Lights    );
        //m_UniformBuffer->UpdateBuffer();
    }
    
    RenderUniformSet::RenderUniformSet(Renderer* r) : m_Renderer(r)
    {
        m_UniformBuffer = r->GetRasterizer()->CreateUniformBuffer(ShaderGenerator::IUT_RENDERER_UNIFORMS);
        m_iProjection      = m_UniformBuffer->DefineUniform("u_Projection"     );
        m_iView            = m_UniformBuffer->DefineUniform("u_View"           );
        m_iCameraPosition  = m_UniformBuffer->DefineUniform("u_CameraPosition" );
        m_iCameraDirection = m_UniformBuffer->DefineUniform("u_CameraDirection");
        m_iResolution      = m_UniformBuffer->DefineUniform("u_Resolution"     );
        m_iFieldOfView     = m_UniformBuffer->DefineUniform("u_FieldOfView"    );
        m_iExposire        = m_UniformBuffer->DefineUniform("u_Exposure"       );
        m_iNearPlane       = m_UniformBuffer->DefineUniform("u_NearPlane"      );
        m_iFarPlane        = m_UniformBuffer->DefineUniform("u_FarPlane"       );
        m_iFocalPoint      = m_UniformBuffer->DefineUniform("u_FocalPoint"     );
        
        UpdateUniforms(); //Must be called here to set the uniform types
    }
    RenderUniformSet::~RenderUniformSet()
    {
        m_Renderer->GetRasterizer()->Destroy(m_UniformBuffer);
    }

    void RenderUniformSet::UpdateUniforms()
    {
        Camera* Cam = m_Renderer->GetActiveCamera();
        if(Cam)
        {
            Mat4 cTrans = Cam->GetTransform();
            Vec3 cPos = Vec3(cTrans[0][3],cTrans[1][3],cTrans[2][3]);
            Vec3 cFwd = Vec3(cTrans[0][2],cTrans[1][2],cTrans[2][2]);
            
            m_UniformBuffer->SetUniform(m_iCameraPosition ,cPos                 );
            m_UniformBuffer->SetUniform(m_iCameraDirection,cFwd                 );
            m_UniformBuffer->SetUniform(m_iView           ,cTrans               );
            m_UniformBuffer->SetUniform(m_iProjection     ,Cam->GetProjection ());
            m_UniformBuffer->SetUniform(m_iExposire       ,Cam->GetExposure   ());
            m_UniformBuffer->SetUniform(m_iFieldOfView    ,Cam->GetFieldOfView());
            m_UniformBuffer->SetUniform(m_iNearPlane      ,Cam->GetNearPlane  ());
            m_UniformBuffer->SetUniform(m_iFarPlane       ,Cam->GetFarPlane   ());
            m_UniformBuffer->SetUniform(m_iFocalPoint     ,Cam->GetFocalPoint ());
        }
        else
        {
            m_UniformBuffer->SetUniform(m_iProjection     ,Mat4::Identity);
            m_UniformBuffer->SetUniform(m_iView           ,Mat4::Identity);
            m_UniformBuffer->SetUniform(m_iCameraPosition ,Vec3(0,0,0)   );
            m_UniformBuffer->SetUniform(m_iCameraDirection,Vec3(0,0,0)   );
            m_UniformBuffer->SetUniform(m_iFieldOfView    ,Vec2(0,0)     );
            m_UniformBuffer->SetUniform(m_iExposire       ,(Scalar)0.0f  );
            m_UniformBuffer->SetUniform(m_iNearPlane      ,0.0f);
            m_UniformBuffer->SetUniform(m_iFarPlane       ,1.0f);
            m_UniformBuffer->SetUniform(m_iFocalPoint     ,0.0f);
        }
        m_UniformBuffer->SetUniform(m_iResolution,m_Renderer->GetRasterizer()->GetContext()->GetResolution());
        //m_UniformBuffer->UpdateBuffer();
    }
    
    
    MaterialUniformSet::MaterialUniformSet(Renderer* r) : m_Renderer(r)
    {
        m_UniformBuffer = m_Renderer->GetRasterizer()->CreateUniformBuffer(ShaderGenerator::IUT_MATERIAL_UNIFORMS);
    }
    MaterialUniformSet::~MaterialUniformSet()
    {
        m_Renderer->GetRasterizer()->Destroy(m_UniformBuffer);
    }
        
    void MaterialUniformSet::UpdateUniforms()
    {
        //m_UniformBuffer->UpdateBuffer();
    }
};
