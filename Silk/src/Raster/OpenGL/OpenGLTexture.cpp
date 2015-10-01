#include <Raster/OpenGL/OpenGLTexture.h>
#include <Raster/OpenGL/PlatformSpecificOpenGL.h>
#include <Raster/Raster.h>

namespace Silk
{
    OpenGLTexture::OpenGLTexture(Rasterizer* r) : Texture(r), m_TexID(0)
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
        if(m_TexID == 0 || m_LastWidth != m_Width || m_LastHeight != m_Height) InitializeTexture();
        
        GLenum pType = GL_UNSIGNED_BYTE;
        GLenum cType = GL_RGBA;
        if(m_Type == PT_FLOAT) { pType = GL_FLOAT; cType = GL_RGBA32F; }
        float an = 0.0;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &an); 
        glBindTexture(GL_TEXTURE_2D,m_TexID);
        glTexImage2D(GL_TEXTURE_2D,0,cType,m_Width,m_Height,0,GL_RGBA,pType,m_Pixels);
        //To do: make the following code user accessable
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT, an); 
        auto err = glGetError();
        glBindTexture(GL_TEXTURE_2D,0);
    }
    void OpenGLTexture::AcquireFromVRAM()
    {
        if(m_TexID == 0) return;
        GLenum pType = GL_UNSIGNED_BYTE;
        GLenum cType = GL_RGBA;
        if(m_Type == PT_FLOAT) { pType = GL_FLOAT; cType = GL_RGBA32F; }
        if(!m_Pixels) CreateTexture(m_Width,m_Height,m_Type);
        
        glBindTexture(GL_TEXTURE_2D,m_TexID);
        glGetTexImage(GL_TEXTURE_2D,0,cType,pType,m_Pixels);
        glBindTexture(GL_TEXTURE_2D,0);
    }
};
