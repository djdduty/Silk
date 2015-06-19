#pragma once
#include <Renderer/Mesh.h>
#include <Renderer/Material.h>
#include <Raster/Raster.h>

namespace Silk
{
    class Renderer
    {
        public:
            Renderer();
            virtual ~Renderer();
        
            UniformBuffer* GetEngineUniformBuffer()   { return m_EngineUniforms; }
            UniformBuffer* GetRendererUniformBuffer() { return m_RendererUniforms; }
        
        protected:
            UniformBuffer* m_EngineUniforms;
            UniformBuffer* m_RendererUniforms;
    };
};