#include <Raster/Raster.h>

namespace Silk
{
    void RasterContext::SetResolution(const Vec2& Resolution) 
    {
        m_Resolution = Resolution;
        if(m_Parent)
            m_Parent->SetViewport(0,0,m_Resolution.x, m_Resolution.y);
    }

    Rasterizer::Rasterizer()
    {
    }
    Rasterizer::~Rasterizer()
    {
        if(m_GraphicsContext)
            delete m_GraphicsContext;
    }
    
    bool Rasterizer::ValidateContext(RasterContext *Ctx)
    {
        ERROR("Cannot validate graphics context: Null rasterizer.\n");
        return false;
    }
    bool Rasterizer::SetContext(RasterContext *Ctx)
    {
        if(ValidateContext(Ctx))
        {
            m_GraphicsContext = Ctx;
            LOG("Valid graphics context.\n");
            return true;
        }
        return false;
    }
};
