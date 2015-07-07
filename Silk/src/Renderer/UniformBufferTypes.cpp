#include <Renderer/UniformBufferTypes.h>
#include <Raster/Raster.h>

namespace Silk
{
    ModelUniformSet::ModelUniformSet(Renderer* r,RenderObject* Obj) : m_Renderer(r), m_Object(Obj)
    {
        m_UniformBuffer = r->GetRasterizer()->CreateUniformBuffer(ShaderGenerator::IUT_OBJECT_UNIFORMS);
        m_iMV                  = m_UniformBuffer->DefineUniform("u_MV"         );
        m_iMVP                 = m_UniformBuffer->DefineUniform("u_MVP"        );
        m_iModel               = m_UniformBuffer->DefineUniform("u_Model"      );
        m_iTexture             = m_UniformBuffer->DefineUniform("u_Texture"    );
        m_iNormal              = m_UniformBuffer->DefineUniform("u_Normal"     );
        m_iIsInstanced         = m_UniformBuffer->DefineUniform("u_IsInstanced");
        m_iLightInfluences     = m_UniformBuffer->DefineUniform("u_Lights"     );
        m_iLightInfluenceCount = m_UniformBuffer->DefineUniform("u_LightCount" );
        
        m_NullLight = new Light();
        m_NullLight->m_Power = 0.0f;
        m_NullLight->m_Color = Vec4(0,0,0,0);
        
        m_Texture =
        m_Model   =
        m_MV      =
        m_MVP     =
        m_Normal  = Mat4::Identity;
        
        m_TexMatrixUpdated = m_ModelMatrixUpdated = m_IsInstancedChanged = true;
        m_HasInstances = false;
        
        SetLights(vector<Light*>());
        
        m_UniformBuffer->SetUniform(m_iMV                 ,m_MV        );
        m_UniformBuffer->SetUniform(m_iMVP                ,m_MVP       );
        m_UniformBuffer->SetUniform(m_iNormal             ,m_Normal    );
        m_UniformBuffer->SetUniform(m_iModel              ,m_Model     );
        m_UniformBuffer->SetUniform(m_iTexture            ,m_Texture   );
        m_UniformBuffer->SetUniform(m_iIsInstanced        ,0           );
        m_UniformBuffer->SetUniform(m_iLightInfluenceCount,m_LightCount);
        m_UniformBuffer->SetUniform(m_iLightInfluences    ,m_Lights    );
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
        m_LightsChanged = true;
    }

    void ModelUniformSet::SetModelMatrix(const Mat4 &M)
    {
        m_ModelMatrixUpdated = true;
        m_Model = M;
        //m_Normal = m_Model.Inverse().Transpose();
        //m_Normal[0][3] = m_Normal[1][3] = m_Normal[2][3] = 0.0f;
    }
    void ModelUniformSet::SetTextureMatrix(const Mat4 &T)
    {
        m_TexMatrixUpdated = true;
        m_Texture = T;
    }
    void ModelUniformSet::SetIsInstanced(bool Flag)
    {
        m_HasInstances = Flag;
        m_IsInstancedChanged = true;
    }
    void ModelUniformSet::UpdateUniforms()
    {
        Camera* Cam = m_Renderer->GetActiveCamera();
        if(Cam)
        {
            Mat4 m = m_Model;
            m_MV  = Cam->GetInverseTransform() * m   ;
            m_MVP = Cam->GetProjection      () * m_MV;
            
            m_UniformBuffer->SetUniform(m_iMV ,m_MV) ;
            m_UniformBuffer->SetUniform(m_iMVP,m_MVP);
            
            if(m_ModelMatrixUpdated)
            {
                m_Normal = m_MV.Transpose().Inverse();
                m_Normal[0][3] = m_Normal[1][3] = m_Normal[2][3] = 0.0f;
                
                if(m_Object->IsInstanced())
                {
                    m_UniformBuffer->SetUniform(m_iModel ,Mat4::Identity);
                    m_UniformBuffer->SetUniform(m_iNormal,Mat4::Identity);
                    m = Mat4::Identity;
                }
                else
                {
                    m_UniformBuffer->SetUniform(m_iModel ,m_Model );
                    m_UniformBuffer->SetUniform(m_iNormal,m_Normal);
                }
                
                m_ModelMatrixUpdated = false;
            }
        }
        else if(m_ModelMatrixUpdated)
        {
            m_MV = m_MVP = m_Model;
            m_ModelMatrixUpdated = false;
        }
        
        if(m_TexMatrixUpdated)
        {
            m_UniformBuffer->SetUniform(m_iTexture,m_Texture);
            m_TexMatrixUpdated = false;
            if(m_Object->IsInstanced()) m_Object->m_Object->SetInstanceTextureTransform(m_Object->m_InstanceIndex,m_Texture);
        }
        
        if(m_LightsChanged)
        {
            m_UniformBuffer->SetUniform(m_iLightInfluenceCount,m_LightCount);
            m_UniformBuffer->SetUniform(m_iLightInfluences    ,m_Lights    );
            m_LightsChanged = false;
        }
        
        if(m_IsInstancedChanged)
        {
            m_UniformBuffer->SetUniform(m_iIsInstanced,(i32)m_HasInstances);
            m_IsInstancedChanged = false;
        }
        
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
        
        m_UniformBuffer->SetUniform(m_iCameraPosition ,Vec3()               );
        m_UniformBuffer->SetUniform(m_iCameraDirection,Vec3()               );
        m_UniformBuffer->SetUniform(m_iView           ,Mat4::Identity       );
        m_UniformBuffer->SetUniform(m_iProjection     ,Mat4::Identity       );
        m_UniformBuffer->SetUniform(m_iExposire       ,0.0f                 );
        m_UniformBuffer->SetUniform(m_iFieldOfView    ,Vec2(0,0)            );
        m_UniformBuffer->SetUniform(m_iNearPlane      ,0.0f                 );
        m_UniformBuffer->SetUniform(m_iFarPlane       ,0.0f                 );
        m_UniformBuffer->SetUniform(m_iFocalPoint     ,0.0f                 );
        m_UniformBuffer->SetUniform(m_iResolution,m_Renderer->GetRasterizer()->GetContext()->GetResolution());
    }
    RenderUniformSet::~RenderUniformSet()
    {
        m_Renderer->GetRasterizer()->Destroy(m_UniformBuffer);
    }

    void RenderUniformSet::UpdateUniforms()
    {
        Camera* Cam = m_Renderer->GetActiveCamera();
        if(!Cam) return;
        Mat4 cTrans = Cam->GetTransform();
        bool DidTrans = Cam->DidTransformUpdate();
        Vec3 cPos = Vec3(cTrans[0][3],cTrans[1][3],cTrans[2][3]);
        Vec3 cFwd = Vec3(cTrans[0][2],cTrans[1][2],cTrans[2][2]);
        
        if(DidTrans)                    m_UniformBuffer->SetUniform(m_iCameraPosition ,cPos                 );
        if(DidTrans)                    m_UniformBuffer->SetUniform(m_iCameraDirection,cFwd                 );
        if(DidTrans)                    m_UniformBuffer->SetUniform(m_iView           ,cTrans               );
        if(Cam->DidProjectionUpdate ()) m_UniformBuffer->SetUniform(m_iProjection     ,Cam->GetProjection ());
        if(Cam->DidExposureUpdate   ()) m_UniformBuffer->SetUniform(m_iExposire       ,Cam->GetExposure   ());
        if(Cam->DidFieldOfViewUpdate()) m_UniformBuffer->SetUniform(m_iFieldOfView    ,Cam->GetFieldOfView());
        if(Cam->DidNearPlaneUpdate  ()) m_UniformBuffer->SetUniform(m_iNearPlane      ,Cam->GetNearPlane  ());
        if(Cam->DidFarPlaneUpdate   ()) m_UniformBuffer->SetUniform(m_iFarPlane       ,Cam->GetFarPlane   ());
        if(Cam->DidFocalPointUpdate ()) m_UniformBuffer->SetUniform(m_iFocalPoint     ,Cam->GetFocalPoint ());
        m_UniformBuffer->SetUniform(m_iResolution,m_Renderer->GetRasterizer()->GetContext()->GetResolution());
        //m_UniformBuffer->UpdateBuffer();
    }
    
    
    MaterialUniformSet::MaterialUniformSet(Renderer* r) : m_Renderer(r)
    {
        m_UniformBuffer = m_Renderer->GetRasterizer()->CreateUniformBuffer(ShaderGenerator::IUT_MATERIAL_UNIFORMS);
        
        m_iRoughness = m_UniformBuffer->DefineUniform("u_Roughness");
        m_iMetalness = m_UniformBuffer->DefineUniform("u_Metalness");
        m_iShininess = m_UniformBuffer->DefineUniform("u_Shininess");
        
        m_iSpecular  = m_UniformBuffer->DefineUniform("u_Specular");
        m_iDiffuse   = m_UniformBuffer->DefineUniform("u_Diffuse" );
        m_iEmissive  = m_UniformBuffer->DefineUniform("u_Emissive");
        
        SetMetalness(0.5f);
        SetRoughness(0.1f);
        SetShininess(1.0f);
        SetSpecular(Vec4(1,1,1,1));
        SetDiffuse (Vec4(1,1,1,1));
        SetEmissive(Vec4(1,1,1,1));
        
        UpdateUniforms();
    }
    MaterialUniformSet::~MaterialUniformSet()
    {
        m_Renderer->GetRasterizer()->Destroy(m_UniformBuffer);
    }
        
    void MaterialUniformSet::UpdateUniforms()
    {
        if(m_MetalnessUpdated) { m_UniformBuffer->SetUniform(m_iMetalness,m_Metalness); m_MetalnessUpdated = false; }
        if(m_RoughnessUpdated) { m_UniformBuffer->SetUniform(m_iRoughness,m_Roughness); m_RoughnessUpdated = false; }
        if(m_ShininessUpdated) { m_UniformBuffer->SetUniform(m_iShininess,m_Shininess); m_ShininessUpdated = false; }
        
        if(m_SpecularUpdated)  { m_UniformBuffer->SetUniform(m_iSpecular,m_Specular);   m_SpecularUpdated  = false; }
        if(m_DiffuseUpdated )  { m_UniformBuffer->SetUniform(m_iDiffuse ,m_Diffuse );   m_DiffuseUpdated   = false; }
        if(m_EmissiveUpdated)  { m_UniformBuffer->SetUniform(m_iEmissive,m_Emissive);   m_EmissiveUpdated  = false; }
        //m_UniformBuffer->UpdateBuffer();
    }
};
