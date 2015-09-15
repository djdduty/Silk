#pragma once
#include <System/SilkTypes.h>
#include <Math/Math.h>

#include <string>
#include <vector>
using namespace std;

namespace Silk
{
    class Shader;
    class Texture;
    class Renderer;
    class MaterialUniformSet;
    class UniformBuffer;
    
    class Material
    {
        public:
            enum MAP_TYPE
            {
                MT_NORMAL      ,
                MT_DIFFUSE     ,
                MT_SPECULAR    ,
                MT_EMISSIVE    ,
                MT_PARALLAX    ,
                MT_DISPLACEMENT,
                MT_MATERIAL0   ,
                MT_MATERIAL1   ,
                MT_CUSTOM0     ,
                MT_CUSTOM1     ,
                MT_CUSTOM2     ,
                MT_CUSTOM3     ,
                MT_CUSTOM4     ,
                MT_CUSTOM5     ,
                MT_CUSTOM6     ,
                MT_CUSTOM7     ,
                
                //Use these only for post processing
                MT_POST_PROCESSING_OUTPUT,
                
                //Use these only for gbuffer attachments (MUST BE SAME AS OUTPUT_FRAGMENT_TYPE)
                MT_FRAG_COLOR    ,
                MT_FRAG_POSITION ,
                MT_FRAG_NORMAL   ,
                MT_FRAG_TANGENT  ,
                MT_FRAG_MATERIAL0,
                MT_FRAG_MATERIAL1,
                MT_FRAG_CUSTOM0  ,
                MT_FRAG_CUSTOM1  ,
                MT_FRAG_CUSTOM2  ,
                MT_FRAG_CUSTOM3  ,
                MT_FRAG_CUSTOM4  ,
                MT_FRAG_CUSTOM5  ,
                MT_FRAG_CUSTOM6  ,
                MT_FRAG_CUSTOM7  ,
                
                MT_COUNT         ,
            };
        
            i32 AddRef() { m_RefCount++; return m_RefCount; }
            i32 Destroy();
        
            void SetMap(MAP_TYPE Type,Texture* Map);
            Texture* GetMap(MAP_TYPE Type) const { return m_Maps[Type]; }
        
            bool LoadMaterial(char* Data,i32* Offset = 0);
        
            void SetShader(Shader* Shdr);
            Shader* GetShader() const { return m_Shader; }
        
            void SetRoughness(f32 Roughness);
            f32 GetRoughness() const;
            void SetMetalness(f32 Metalness);
            f32 GetMetalness() const;
            void SetShininess(f32 Shininess);
            f32 GetShininess() const;
            
            void SetDiffuse (const Vec4& Color);
            Vec4 GetDiffuse () const;
            void SetSpecular(const Vec4& Color);
            Vec4 GetSpecular() const;
            void SetEmissive(const Vec4& Color);
            Vec4 GetEmissive() const;
            
            void SetMinParallaxLayers(f32 Min  );
            void SetMaxParallaxLayers(f32 Max  );
            void SetParallaxScale    (f32 Scale);
            f32 GetMinParallaxLayers() const;
            f32 GetMaxParallaxLayers() const;
            f32 GetParallaxScale    () const;
        
            UniformBuffer* GetUniforms();
            UniformBuffer* GetUserUniforms() const { return m_UserUniforms; }
            bool HasUpdated() const { return m_HasUpdated; }
            void UpdateUniforms();
        
        protected:
            friend class Renderer;
            Material(Renderer* r);
            ~Material();
        
            i32 m_RefCount;
            MaterialUniformSet* m_Uniforms;
            UniformBuffer* m_UserUniforms;
            Texture* m_Maps[MT_COUNT];
            Shader* m_Shader;
            Renderer* m_Renderer;
            bool m_HasUpdated;
        
            vector<string>VertShaderFuncs;
            vector<string>GeomShaderFuncs;
            vector<string>FragShaderFuncs;
    };
    string GetShaderMapName(Material::MAP_TYPE Type);
};

