#include <Renderer/Texture.h>

namespace Silk
{
    Texture::Texture(Rasterizer* r) : m_Pixels(0), m_MemSize(0), m_RefCount(1), m_Rasterizer(r)
    {
    }
    Texture::~Texture()
    {
    }
    i32 Texture::Destroy()
    {
        m_RefCount--;
        i32 r = m_RefCount;
        if(m_RefCount == 0)
        {
            if(m_Pixels) delete [] m_Pixels;
            m_Pixels = 0;
            m_Rasterizer->Destroy(this);
            return r;
        }
        return r;
    }

    void Texture::CreateTexture(i32 Width,i32 Height)
    {
        if(m_Pixels) delete [] m_Pixels;
        m_Width   = Width;
        m_Height  = Height;
        m_Pixels  = new u32[(Width * Height) + 1];
        m_MemSize = Width * Height * sizeof(u32);
    }
    void Texture::SetPixel(const Vec2 &Coord,const Vec4 &Color)
    {
        if(!m_Pixels) return;
        i32 bIdx = ((i32)Coord.x) + (((i32)Coord.y) * m_Height);
        Byte* Pixel = (Byte*)&m_Pixels[bIdx];
        Pixel[0] = Color.x * 255.0f;
        Pixel[1] = Color.y * 255.0f;
        Pixel[2] = Color.z * 255.0f;
        Pixel[3] = Color.w * 255.0f;
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
