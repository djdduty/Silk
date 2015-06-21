#pragma once
#include <Renderer/Texture.h>
#include <Raster/OpenGL/PlatformSpecificOpenGL.h>

namespace Silk
{
    class OpenGLTexture : public Texture
    {
        public:
            OpenGLTexture();
            ~OpenGLTexture();
        
            virtual void InitializeTexture();
        
            GLuint GetTextureID() const { return m_TexID; }
        
        protected:
            GLuint m_TexID;
    };
};

