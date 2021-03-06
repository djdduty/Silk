#pragma once
#include <System/SilkTypes.h>
#include <Math/Math.h>
#include <Renderer/Material.h>

#include <vector>
using namespace std;

namespace Silk
{
    class UniformBuffer;
    class Light;
    class Renderer;
    class RenderObject;
    
    class ModelUniformSet
    {
        public:
            ModelUniformSet(Renderer* r,RenderObject* Obj);
            ~ModelUniformSet();
        
            void SetLights(const vector<Light*>& Lights);
        
            void SetModelMatrix(const Mat4& M);
            const Mat4& GetModelMatrix() const { return m_Model; }
            void SetTextureMatrix(const Mat4& T);
            const Mat4& GetTextureMatrix() const { return m_Texture; }
            const Mat4& GetNormalMatrix () const { return m_Normal;  }
            void SetObjectID(i32 ID) { m_ObjectID = ID; m_ObjectIDUpdated = true; }
            i32  GetObjectID() const { return m_ObjectID; }
        
            void SetIsInstanced(bool Flag);
        
            void UpdateUniforms();
            UniformBuffer* GetUniforms() const { return m_UniformBuffer; }
        
        protected:
            Light* m_NullLight;
            vector<Light*>m_Lights;
            i32 m_LightCount;
            i32 m_ObjectID;
        
            bool m_ObjectIDUpdated   ;
            bool m_ModelMatrixUpdated;
            bool m_TexMatrixUpdated  ;
            bool m_LightsChanged     ;
            bool m_IsInstancedChanged;
        
            Mat4 m_MV;
            Mat4 m_MVP;
            Mat4 m_Model;
            Mat4 m_Normal;
            Mat4 m_Texture;
            bool m_HasInstances;
        
            UniformBuffer* m_UniformBuffer;
            i32 m_iMV;                   //Mat4
            i32 m_iMVP;                  //Mat4
            i32 m_iModel;                //Mat4
            i32 m_iNormal;               //Mat4
            i32 m_iTexture;              //Mat4
            i32 m_iIsInstanced;          //int
            i32 m_iLightInfluenceCount;  //int
            i32 m_iObjectID;             //int
            i32 m_iLightInfluences;      //Light[]
        
            Renderer* m_Renderer;
            RenderObject* m_Object;
    };
    
    class Camera;
    class RenderUniformSet
    {
        public:
            RenderUniformSet(Renderer* r);
            ~RenderUniformSet();
        
            void UpdateUniforms();
            UniformBuffer* GetUniforms() const { return m_UniformBuffer; }
        
        protected:
            UniformBuffer* m_UniformBuffer;
            i32 m_iProjection     ; //Mat4
            i32 m_iInvProjection  ; //Mat4
            i32 m_iView           ; //Mat4
            i32 m_iViewport       ; //Vec4
            i32 m_iCameraPosition ; //Vec3
            i32 m_iCameraDirection; //Vec3
            i32 m_iResolution     ; //Vec2
            i32 m_iExposire       ; //f32
            i32 m_iFieldOfView    ; //f32
            i32 m_iNearPlane      ; //f32
            i32 m_iFarPlane       ; //f32
            i32 m_iFocalPoint     ; //f32
            i32 m_iGamma          ; //f32
        
            Renderer* m_Renderer;
    };
    
    class MaterialUniformSet
    {
        public:
            MaterialUniformSet(Renderer* r);
            ~MaterialUniformSet();
        
            void SetRoughness(f32 Roughness) { m_Roughness = Roughness; m_RoughnessUpdated = true; }
            void SetMetalness(f32 Metalness) { m_Metalness = Metalness; m_MetalnessUpdated = true; }
            void SetShininess(f32 Shininess) { m_Shininess = Shininess; m_ShininessUpdated = true; }
            f32 GetRoughness() const { return m_Roughness; }
            f32 GetMetalness() const { return m_Metalness; }
            f32 GetShininess() const { return m_Shininess; }
            
            void   SetDiffuse (const Vec4& Color) { m_Diffuse  = Color ; m_DiffuseUpdated  = true; }
            void   SetSpecular(Scalar Factor)     { m_Specular = Factor; m_SpecularUpdated = true; }
            void   SetEmissive(const Vec4& Color) { m_Emissive = Color ; m_EmissiveUpdated = true; }
            Vec4   GetDiffuse () const { return m_Diffuse ; }
            Scalar GetSpecular() const { return m_Specular; }
            Vec4   GetEmissive() const { return m_Emissive; }
        
            void SetMinParallaxLayers(f32 Min  ) { m_MinParallaxLayers = Min  ; m_MinParallaxLayersUpdated = true; }
            void SetMaxParallaxLayers(f32 Max  ) { m_MaxParallaxLayers = Max  ; m_MaxParallaxLayersUpdated = true; }
            void SetParallaxScale    (f32 Scale) { m_ParallaxScale     = Scale; m_ParallaxScaleUpdated     = true; }
            f32 GetMinParallaxLayers() const { return m_MinParallaxLayers; }
            f32 GetMaxParallaxLayers() const { return m_MaxParallaxLayers; }
            f32 GetParallaxScale    () const { return m_ParallaxScale    ; }
        
            void SetTextureMapUsed(Material::MAP_TYPE Type,bool Used) { m_TextureMapsUsed[Type] = Used; m_TextureMapsUsedUpdated[Type] = true; }
            bool GetTextureMapUsed(Material::MAP_TYPE Type)           { return m_TextureMapsUsed[Type]; }
            void UpdateUniforms();
            UniformBuffer* GetUniforms() const { return m_UniformBuffer; }
        
        protected:
            UniformBuffer* m_UniformBuffer;
            i32 m_iRoughness;
            i32 m_iMetalness;
            i32 m_iShininess;
            i32 m_iDiffuse;
            i32 m_iSpecular;
            i32 m_iEmissive;
            i32 m_iMinParallaxLayers;
            i32 m_iMaxParallaxLayers;
            i32 m_iParallaxScale;
            i32 m_iTextureMapsUsed[Material::MT_COUNT];
        
            f32    m_Roughness;
            bool   m_RoughnessUpdated;
            f32    m_Metalness;
            bool   m_MetalnessUpdated;
            f32    m_Shininess;
            bool   m_ShininessUpdated;
            Vec4   m_Diffuse;
            bool   m_DiffuseUpdated;
            Scalar m_Specular;
            bool   m_SpecularUpdated;
            Vec4   m_Emissive;
            bool   m_EmissiveUpdated;
            f32    m_MinParallaxLayers;
            bool   m_MinParallaxLayersUpdated;
            f32    m_MaxParallaxLayers;
            bool   m_MaxParallaxLayersUpdated;
            f32    m_ParallaxScale;
            bool   m_ParallaxScaleUpdated;
            i32    m_TextureMapsUsed[Material::MT_COUNT];
            bool   m_TextureMapsUsedUpdated[Material::MT_COUNT];
        
            Renderer* m_Renderer;
    };
};

