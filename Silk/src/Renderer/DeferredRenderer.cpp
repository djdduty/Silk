#include <Renderer/DeferredRenderer.h>
#include <Renderer/Mesh.h>
#include <Renderer/Texture.h>

#include <Raster/Raster.h>
#include <Raster/OpenGL/OpenGLShader.h>

namespace Silk
{
    //Deferred
    void DeferredRenderer::RenderObjects(ObjectList *List,PRIMITIVE_TYPE PrimType)
    {
        m_SceneOutput->EnableTarget();
        Renderer::RenderObjects(List,PrimType,false);
        m_SceneOutput->Disable();
        
        RenderTexture(m_SceneOutput->GetAttachment(ShaderGenerator::OUTPUT_FRAGMENT_TYPE::OFT_NORMAL));
    }
};
