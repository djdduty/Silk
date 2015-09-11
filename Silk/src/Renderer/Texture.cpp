#include <Renderer/Texture.h>
#include <Raster/Raster.h>

namespace Silk
{
    Texture::Texture(Rasterizer* r) : m_Pixels(0), m_MemSize(0), m_RefCount(1), m_Type(PT_COUNT), m_FrameBuffer(0), m_Rasterizer(r)
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
            if(m_Pixels)
            {
                if(m_Type == PT_UNSIGNED_BYTE) delete [] (u32*)m_Pixels;
                else delete [] (Vec4*)m_Pixels;
            }
            m_Pixels = 0;
            m_Rasterizer->Destroy(this);
            return r;
        }
        return r;
    }
    void Texture::EnableRTT(bool UseDepth)
    {
        if(!m_FrameBuffer)
        {
            m_FrameBuffer = m_Rasterizer->CreateFrameBuffer();
            m_FrameBuffer->SetResolution(Vec2(m_Width,m_Height));
            m_FrameBuffer->SetUseDepthBuffer(UseDepth);
            m_FrameBuffer->AddAttachment(FragmentColorOutputIndex,m_Type,this);
        }
        m_FrameBuffer->EnableTarget();
    }
    void Texture::DisableRTT()
    {
        if(m_FrameBuffer) m_FrameBuffer->Disable();
    }
	
    void Texture::CreateTexture(i32 Width,i32 Height,PIXEL_TYPE Type)
    {
        if(Type < 0 || Type >= Texture::PT_COUNT) { ERROR("Invalid pixel type.\n"); return; }
        if(m_Pixels)
        {
            if(m_Type == PT_UNSIGNED_BYTE) delete [] (u32*)m_Pixels;
            else delete [] (Vec4*)m_Pixels;
        }
        
        m_Width   = Width;
        m_Height  = Height;
        if(m_Type == PT_UNSIGNED_BYTE)
        {
            m_Pixels  = new u32[(Width * Height) + 1];
            m_MemSize = Width * Height * sizeof(u32);
        }
        else
        {
            m_Pixels  = new Vec4[(Width * Height) + 1];
            m_MemSize = Width * Height * sizeof(Vec4);
        }
        m_Type = Type;
    }
    void Texture::CreateTextureb(i32 Width,i32 Height,Byte* Data)
    {
        if(m_Pixels)
        {
            if(m_Type == PT_UNSIGNED_BYTE) delete [] (u32*)m_Pixels;
            else delete [] (Vec4*)m_Pixels;
        }
        
        m_Width   = Width;
        m_Height  = Height;
        m_MemSize = Width * Height * sizeof(u32);
		m_Pixels = Data;
        m_Type = PT_UNSIGNED_BYTE;
    }
    void Texture::CreateTexturef(i32 Width,i32 Height,Vec4* Data)
    {
        if(m_Pixels)
        {
            if(m_Type == PT_UNSIGNED_BYTE) delete [] (u32*)m_Pixels;
            else delete [] (Vec4*)m_Pixels;
        }
        
        m_Width   = Width;
        m_Height  = Height;
        m_MemSize = Width * Height * sizeof(Vec4);
		m_Pixels  = Data;
        m_Type    = PT_FLOAT;
    }
    void Texture::SetPixel(const Vec2 &Coord,const Vec4 &Color)
    {
        if(!m_Pixels) return;
        i32 bIdx = ((i32)Coord.x) + (((i32)Coord.y) * m_Width);
        if(m_Type == PT_UNSIGNED_BYTE)
        {
            Byte* Pixel = (Byte*)&((u32*)m_Pixels)[bIdx];
            Pixel[0] = Color.x * 255.0f;
            Pixel[1] = Color.y * 255.0f;
            Pixel[2] = Color.z * 255.0f;
            Pixel[3] = Color.w * 255.0f;
        }
        else ((Vec4*)m_Pixels)[bIdx] = Color;
    }
    Vec4 Texture::GetPixel(const Vec2& Coord) const
    {
        if(!m_Pixels) return Vec4(0,0,0,0);
        i32 bIdx = ((i32)Coord.x) + (((i32)Coord.y) * m_Width);
        if(m_Type == PT_UNSIGNED_BYTE)
        {
            Vec4 p;
            const f32 Inv255 = 1.0f / 255.0f;
            p.x = ((f32)((uByte*)&((u32*)m_Pixels)[bIdx])[0]) * Inv255;
            p.y = ((f32)((uByte*)&((u32*)m_Pixels)[bIdx])[1]) * Inv255;
            p.z = ((f32)((uByte*)&((u32*)m_Pixels)[bIdx])[2]) * Inv255;
            p.w = ((f32)((uByte*)&((u32*)m_Pixels)[bIdx])[3]) * Inv255;
            return p;
        }
        else return ((Vec4*)m_Pixels)[bIdx];
        return Vec4(0,0,0,0);
    }
};
