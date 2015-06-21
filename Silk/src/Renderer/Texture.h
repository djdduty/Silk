#pragma once
#include <System/SilkTypes.h>
#include <Raster/Raster.h>

namespace Silk
{
    class Texture
    {
        public:
            Texture();
            virtual ~Texture();
        
            virtual void InitializeTexture() = 0;
        
            void CreateTexture(i32 Width,i32 Height);
            void SetPixel(const Vec2& Coord,const Vec4& Color);
        
        protected:
            i32  m_Width;
            i32  m_Height;
            u32* m_Pixels;
            i64  m_MemSize;
    };
};

