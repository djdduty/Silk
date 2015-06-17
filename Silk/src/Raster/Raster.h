#ifndef SILK_RASTERIZATION_H
#define SILK_RASTERIZATION_H

#include <Math/Math.h>

namespace Silk
{
    class RasterObjectIdentifier
    {
        public:
            RasterObjectIdentifier();
            ~RasterObjectIdentifier();
    };
    
    class ColorFormat
    {
        public:
            enum COLOR_CHANNELS
            {
                CC_RGB,
                CC_RGBA,
                CC_GRAYSCALE,
            };
        
            ColorFormat(COLOR_CHANNELS c = CC_RGBA,u8 r = 8,u8 g = 8,u8 b = 8,u8 a = 8) :
                Channels(c), rbits(r), gbits(g), bbits(b), abits(a), graybits(0)
                { }
        
            ColorFormat(u8 GrayscaleBits) :
                Channels(CC_GRAYSCALE), rbits(0), gbits(0), bbits(0), abits(0), graybits(GrayscaleBits)
                { }
        
            void SetRGB (u8 r,u8 g,u8 b)      { Channels = CC_RGB      ; rbits = r; gbits = g; bbits = b; abits = 0; graybits = 0; }
            void SetRGBA(u8 r,u8 g,u8 b,u8 a) { Channels = CC_RGBA     ; rbits = r; gbits = g; bbits = b; abits = a; graybits = 0; }
            void SetGrayscale(u8 g)           { Channels = CC_GRAYSCALE; rbits = 0; gbits = 0; bbits = 0; abits = 0; graybits = g; }
            
            COLOR_CHANNELS Channels;
            
            u8 rbits;
            u8 gbits;
            u8 bbits;
            u8 abits;
            u8 graybits;
    };
    
    class RasterContext
    {
        public:
            RasterContext();
            ~RasterContext();
        
            Vec2 m_Resolution;
            ColorFormat m_Format;
    };
    
    class Rasterizer
    {
        public:
            Rasterizer();
            ~Rasterizer();
        
            virtual bool ValidateContext(RasterContext* Ctx);
            bool SetContext(RasterContext* Ctx);
        
        protected:
            RasterContext* m_GraphicsContext;
    };
}

#endif