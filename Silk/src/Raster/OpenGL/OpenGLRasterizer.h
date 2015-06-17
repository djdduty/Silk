#pragma once
#include <Raster/Raster.h>
#include <GLFW/glfw3.h>

namespace Silk
{
    class OpenGLRasterizerContext : public RasterContext
    {
        public:
            OpenGLRasterizerContext();
            ~OpenGLRasterizerContext();
    };
    
    
    class OpenGLRasterizer : public Rasterizer
    {
        public:
            OpenGLRasterizer();
            ~OpenGLRasterizer();
        
            virtual bool ValidateContext(RasterContext* Ctx);
            virtual void InitializeContext();
        
            /* Rasterizer configuration */
            void SetClearBuffers(GLuint Bitfield) { m_ClearBuffers = Bitfield; }
        
            /* Rasterizer functionality */
            virtual void ClearActiveFramebuffer()               { glClearColor(m_ClearColor.x,m_ClearColor.y,m_ClearColor.z,m_ClearColor.w);
                                                                  glClear(m_ClearBuffers); }
            virtual void SetViewport(i32 x,i32 y,i32 w,i32 h)   { glViewport(x,y,w,h);     }
        
        protected:
            GLuint m_ColorFormat;
            GLuint m_ClearBuffers;
    };
};

