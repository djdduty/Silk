#pragma once
#include <System/SilkTypes.h>
#include <Math/Math.h>

#include <vector>
using namespace std;

namespace Silk
{
    class UniformBuffer;
    class Light;
    class Renderer;
    
    class ModelUniformSet
    {
        public:
            ModelUniformSet(Renderer* r);
            ~ModelUniformSet();
        
            void SetLights(const vector<Light*>& Lights);
        
            Mat4 MV;
            Mat4 MVP;
            Mat4 Model;
            Mat4 Normal;
            Mat4 Texture;
        
            void UpdateUniforms();
            UniformBuffer* GetUniforms() const { return m_UniformBuffer; }
        
        protected:
            Light* m_NullLight;
            vector<Light*>m_Lights;
            i32 m_LightCount;
        
            UniformBuffer* m_UniformBuffer;
            i32 m_iMV;                   //Mat4
            i32 m_iMVP;                  //Mat4
            i32 m_iModel;                //Mat4
            i32 m_iNormal;               //Mat4
            i32 m_iTexture;              //Mat4
            i32 m_iLightInfluenceCount;  //int
            i32 m_iLightInfluences;      //Light[]
        
            Renderer* m_Renderer;
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
            i32 m_iView           ; //Mat4
            i32 m_iCameraPosition ; //Vec3
            i32 m_iCameraDirection; //Vec3
            i32 m_iResolution     ; //Vec2
            i32 m_iExposire       ; //f32
            i32 m_iFieldOfView    ; //f32
            i32 m_iNearPlane      ; //f32
            i32 m_iFarPlane       ; //f32
            i32 m_iFocalPoint     ; //f32
        
            Renderer* m_Renderer;
    };
    
    class MaterialUniformSet
    {
        public:
            MaterialUniformSet(Renderer* r);
            ~MaterialUniformSet();
        
            void UpdateUniforms();
            UniformBuffer* GetUniforms() const { return m_UniformBuffer; }
        
        protected:
            UniformBuffer* m_UniformBuffer;
            
            Renderer* m_Renderer;
    };
};

