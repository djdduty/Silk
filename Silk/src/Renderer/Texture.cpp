#include <Renderer/Texture.h>

//From the good old days of PSP programming
#define GU_RGBA(r,g,b,a)   (((a) << 24)|((r) << 16)|((g) << 8)|(b))

namespace Silk
{
    Texture::Texture() : m_Pixels(0), m_MemSize(0)
    {
    }
    Texture::~Texture()
    {
        FreeMemory();
    }
    void Texture::FreeMemory()
    {
        if(m_Pixels) delete [] m_Pixels;
        m_Pixels = 0;
    }

    void Texture::CreateTexture(i32 Width,i32 Height)
    {
        if(m_Pixels) delete [] m_Pixels;
        m_Width   = Width;
        m_Height  = Height;
        m_Pixels  = new u32[Width * Height];
        m_MemSize = Width * Height * sizeof(u32);
    }
    void Texture::SetPixel(const Vec2 &Coord,const Vec4 &Color)
    {
        if(!m_Pixels) return;
        i32 bIdx = ((i32)Coord.x) + (((i32)Coord.y) * m_Height);
        m_Pixels[bIdx] = GU_RGBA(uByte(Color.x * 255.0f),uByte(Color.y * 255.0f),uByte(Color.z * 255.0f),uByte(Color.w * 255.0f));
    }
    Vec4 Texture::GetPixel(const Vec2& Coord) const
    {
        if(!m_Pixels) return Vec4(0,0,0,0);
        i32 bIdx = ((i32)Coord.x) + (((i32)Coord.y) * m_Height);
        Vec4 p;
        const f32 Inv255 = 1.0f / 255.0f;
        p.x = ((f32)((uByte*)&m_Pixels[bIdx])[0]) * Inv255;
        p.y = ((f32)((uByte*)&m_Pixels[bIdx])[1]) * Inv255;
        p.z = ((f32)((uByte*)&m_Pixels[bIdx])[2]) * Inv255;
        p.w = ((f32)((uByte*)&m_Pixels[bIdx])[3]) * Inv255;
        return p;
    }
};
