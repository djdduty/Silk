#pragma once
#include <System/SilkTypes.h>
#include <Math/Math.h>

namespace Silk
{
    class Rasterizer;
    class FrameBuffer;
    class Texture
    {
        public:
            enum PIXEL_TYPE
            {
                PT_UNSIGNED_BYTE,
                PT_FLOAT,
                PT_COUNT,
            };
            i32 AddRef() { m_RefCount++; return m_RefCount; }
            bool IsFree() const { return m_RefCount == 0; }
            i32 GetRefCount() const { return m_RefCount; }
            i32 Destroy();
            
            virtual void InitializeTexture() = 0;
            virtual void UpdateTexture() = 0;
        
            void EnableRTT(bool UseDepth);
            void DisableRTT();
            void FreePixels();
            virtual void AcquireFromVRAM() = 0;
        
            void CreateTexture(i32 Width,i32 Height,PIXEL_TYPE Type = PT_UNSIGNED_BYTE);
			void CreateTextureb(i32 Width,i32 Height,Byte* Data);
			void CreateTexturef(i32 Width,i32 Height,Vec4* Data);
            void SetPixel(const Vec2& Coord,const Vec4& Color);
            Vec4 GetPixel(const Vec2& Coord) const;
            PIXEL_TYPE GetPixelType() const { return m_Type; }
            void* GetPixels() { return m_Pixels; }
        
        protected:
            Texture(Rasterizer* r);
            virtual ~Texture();
        
            i32  m_Width;
            i32  m_Height;
            void* m_Pixels;
            i64  m_MemSize;
            i32  m_RefCount;
            PIXEL_TYPE m_Type;
            FrameBuffer* m_FrameBuffer;
            Rasterizer* m_Rasterizer;
    };
};

