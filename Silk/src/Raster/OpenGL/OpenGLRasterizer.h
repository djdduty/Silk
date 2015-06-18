#pragma once
#include <Raster/Raster.h>
#include <Raster/OpenGL/PlatformSpecificOpenGL.h>

#include <vector>
using namespace std;

namespace Silk
{
    class OpenGLObjectIdentifier : public RasterObjectIdentifier
    {
        public:
            OpenGLObjectIdentifier();
            ~OpenGLObjectIdentifier();
            
            struct AttributeBuffer
            {
                GLuint BufferID;
                GLuint Index;
                GLint Size;
                GLint ComponentCount;
                GLenum Type;
                GLboolean Normalized;
                GLsizei Stride;
                GLvoid* Pointer;
                size_t TypeSize;
            };
        
            enum BUFFER_MAP_ACCESS
            {
                BMA_READ,
                BMA_WRITE,
                BMA_READWRITE,
            };
        
            void ClearData();
            virtual void SetMesh(Mesh* m);
        
            void AddAttribute    (GLuint AttributeIndex,GLint ComponentCount,GLint Size,GLenum Type,GLboolean Normalized,GLsizei Stride,GLvoid* Pointer);
            void SupplyBufferData(GLuint AttributeIndex,GLenum Target,GLint ComponentCount,GLsizeiptr Size,GLvoid* Data,GLenum Usage);
            void SetIndexBufferAttributeIndex(GLuint AttributeIndex);
        
            void* MapBuffer(GLuint AttributeIndex,BUFFER_MAP_ACCESS Access);
            void UnmapBuffer(GLuint AttributeIndex);
        
            virtual void Render(i32 PrimitiveType,i32 Start,i32 Count);
        
        protected:
            i32 GetAttributeBufferIndex(i32 AttribIndex) const;
            GLuint m_VAO;
            i32 m_IBIndex;
            vector<struct AttributeBuffer> m_Attributes;
    };
    
    class OpenGLRasterizerContext : public RasterContext
    {
        public:
            OpenGLRasterizerContext(Rasterizer* Parent) : RasterContext(Parent) { }
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
            virtual RasterContext* CreateContext()              { return new OpenGLRasterizerContext(this); }
        
        protected:
            GLuint m_ColorFormat;
            GLuint m_ClearBuffers;
    };
};

