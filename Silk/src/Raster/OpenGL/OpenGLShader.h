#pragma once
#include <Raster/Raster.h>
#include <Raster/OpenGL/PlatformSpecificOpenGL.h>

namespace Silk
{
    class OpenGLShader : public Shader
    {
        public:
            OpenGLShader();
            ~OpenGLShader();
        
            virtual i32 Load(CString VertexCode,CString GeometryCode,CString FragmentCode);
        
            virtual void Enable();
            virtual void Disable();
        
        protected:
            GLuint m_PID;
            GLuint m_VS;
            GLuint m_GS;
            GLuint m_PS;
    };
};