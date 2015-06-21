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
        if(m_Pixels) delete [] m_Pixels;
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
};
