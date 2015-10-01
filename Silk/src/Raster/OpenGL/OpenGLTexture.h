#pragma once
#include <Renderer/Texture.h>
#include <Raster/OpenGL/PlatformSpecificOpenGL.h>

namespace Silk
{
    class FrameBuffer;
    class OpenGLTexture : public Texture
    {
        public:
            OpenGLTexture(Rasterizer* r);
            ~OpenGLTexture();
        
            virtual void InitializeTexture();
            virtual void UpdateTexture();
            virtual void AcquireFromVRAM();
        
            GLuint GetTextureID() const { return m_TexID; }
        
        protected:
            i32 m_LastWidth;
            i32 m_LastHeight;
            GLuint m_TexID;
    };
};

