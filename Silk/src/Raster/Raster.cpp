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
        fprintf(stderr,"Cannot validate graphics context: Null rasterizer.\n");
        return false;
    }
    bool Rasterizer::SetContext(RasterContext *Ctx)
    {
        if(ValidateContext(Ctx))
        {
            m_GraphicsContext = Ctx;
            //printf("Valid graphics context.\n");
            return true;
        }
        return false;
    }
};
