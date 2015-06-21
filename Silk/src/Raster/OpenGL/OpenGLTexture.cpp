#include <Raster/OpenGL/OpenGLTexture.h>
#include <Raster/OpenGL/PlatformSpecificOpenGL.h>

namespace Silk
{
    OpenGLTexture::OpenGLTexture() : m_TexID(0)
    {
    }
    OpenGLTexture::~OpenGLTexture()
    {
        if(m_TexID != 0) glDeleteTextures(1,&m_TexID);
        m_TexID = 0;
    }
    void OpenGLTexture::InitializeTexture()
    {
        if(m_TexID != 0) glDeleteTextures(1,&m_TexID);
        m_TexID = 0;
        
        glGenTextures(1,&m_TexID);
        glBindTexture(GL_TEXTURE_2D,m_TexID);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,m_Width,m_Height,0,GL_RGBA,GL_UNSIGNED_BYTE,m_Pixels);
        glBindTexture(GL_TEXTURE_2D,0);
    }
};
