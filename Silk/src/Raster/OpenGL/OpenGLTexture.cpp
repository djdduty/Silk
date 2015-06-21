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
        
        m_LastWidth = m_Width;
        m_LastHeight = m_Height;
    }
    void OpenGLTexture::UpdateTexture()
    {
        if(!m_Pixels) return;
        if(m_TexID == 0 || m_LastWidth != m_Width || m_LastHeight != m_Height) InitializeTexture();
        
        glBindTexture(GL_TEXTURE_2D,m_TexID);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,m_Width,m_Height,0,GL_RGBA,GL_UNSIGNED_BYTE,m_Pixels);
        //To do: make the following code user accessable
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D,0);
    }
};
