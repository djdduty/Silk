#pragma once
#include <Renderer/Texture.h>

namespace Silk
{
    class Shader;
    class Material
    {
        public:
            enum MAP_TYPE
            {
                MT_DIFFUSE,
                MT_NORMAL,
                MT_SPECULAR,
                MT_EMISSIVE,
                MT_DISPLACEMENT,
                MT_ROUGHNESS,
                MT_METALNESS,
                MT_CUSTOM0,
                MT_CUSTOM1,
                MT_CUSTOM2,
                MT_CUSTOM3,
                MT_CUSTOM4,
                MT_CUSTOM5,
                MT_CUSTOM6,
                MT_CUSTOM7,
                MT_CUSTOM8,
                MT_COUNT,
            };
            Material() {}
            ~Material() {}
        
            void SetMap(MAP_TYPE Type,Texture* Map) { m_Maps[Type] = Map; }
            Texture* GetMap(MAP_TYPE Type) const { return m_Maps[Type]; }
        
            void SetShader(Shader* Shdr) { m_Shader = Shdr; }
            Shader* GetShader() const { return m_Shader; }
        
        protected:
            Texture* m_Maps[MT_COUNT];
            Shader* m_Shader;
    };
};

