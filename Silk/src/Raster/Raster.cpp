#include <Raster/Raster.h>

namespace Silk
{
    Rasterizer::Rasterizer()
    {
    }
    Rasterizer::~Rasterizer()
    {
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
