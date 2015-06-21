#pragma once
#include <Raster/Raster.h>
#include <Raster/OpenGL/PlatformSpecificOpenGL.h>

namespace Silk
{
    class OpenGLShader : public Shader
    {
        public:
            OpenGLShader(Renderer* r);
            ~OpenGLShader();
        
            virtual i32 Load(CString VertexCode,CString GeometryCode,CString FragmentCode);
        
            virtual void Enable();
            virtual void UseMaterial(Material* Mat);
            virtual void Disable();
        
        protected:
            GLuint m_PID;
            GLuint m_VS;
            GLuint m_GS;
            GLuint m_PS;
            GLuint m_SamplerLocations[Material::MT_COUNT];
    };
};