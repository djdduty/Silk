#pragma once
#include <System/SilkTypes.h>
#include <Raster/Raster.h>

namespace Silk
{
    class Texture
    {
        public:
            void FreeMemory();
            
            virtual void InitializeTexture() = 0;
            virtual void UpdateTexture() = 0;
        
            void CreateTexture(i32 Width,i32 Height);
            void SetPixel(const Vec2& Coord,const Vec4& Color);
            Vec4 GetPixel(const Vec2& Coord) const;
        
            u32* GetPixels() { return m_Pixels; }
        
        protected:
            Texture();
            virtual ~Texture();
        
            i32  m_Width;
            i32  m_Height;
            u32* m_Pixels;
            i64  m_MemSize;
    };
};

