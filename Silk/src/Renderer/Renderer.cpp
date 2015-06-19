#include <Renderer/Renderer.h>

namespace Silk
{
    Renderer::Renderer() : m_EngineUniforms(new UniformBuffer()), m_RendererUniforms(new UniformBuffer())
    {
    }
    Renderer::~Renderer()
    {
        delete m_EngineUniforms;
        delete m_RendererUniforms;
    }
};
