#pragma once
#include <Raster/Raster.h>
#include <Raster/OpenGL/PlatformSpecificOpenGL.h>

#include <vector>
using namespace std;

namespace Silk
{
    class OpenGLObject : public RasterObject
    {
        public:
            OpenGLObject(Rasterizer* r);
            ~OpenGLObject();
            
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
        
            virtual bool IsInstanced();
            virtual i32 AddInstance();
            virtual i32 RemoveInstance(i32 InstanceID);
            virtual void SetInstanceTransform(i32 InstanceID,const Mat4& Transform);
            virtual void SetInstanceNormalTransform(i32 InstanceID,const Mat4& Transform);
            virtual void SetInstanceTextureTransform(i32 InstanceID,const Mat4& Transform);
            virtual void UpdateInstanceData();
        
            void* MapBuffer(GLuint AttributeIndex,BUFFER_MAP_ACCESS Access);
            void UnmapBuffer(GLuint AttributeIndex);

            virtual void Render(RenderObject* Obj,PRIMITIVE_TYPE PrimitiveType,i32 Start,i32 Count);
        
        protected:
            i32 GetAttributeBufferIndex(i32 AttribIndex) const;
            void SetIndexBufferAttributeIndex(GLuint AttributeIndex);
        
            Mesh* m_Mesh;
        
            GLuint m_VAO;
            i32 m_IBIndex;
        
            vector<struct AttributeBuffer> m_Attributes;
            bool m_IsInstanced          ;
            bool m_InstanceCountChanged ;
            i32 m_MiniTransformSet      ;
            i32 m_MaxiTransformSet      ;
            i32 m_MiniNTransformSet     ;
            i32 m_MaxiNTransformSet     ;
            i32 m_MiniTTransformSet     ;
            i32 m_MaxiTTransformSet     ;
        
            i32 m_InstanceTransformsID;
            i32 m_InstanceNormalTransformsID;
            i32 m_InstanceTextureTransformsID;
            
            vector<Mat4> m_InstanceTransforms;
            vector<Mat4> m_InstanceNormalTransforms;
            vector<Mat4> m_InstanceTextureTransforms;
    };
    
    class OpenGLUniformBuffer : public UniformBuffer
    {
        public:
            OpenGLUniformBuffer() : UniformBuffer(), m_Buffer(0) { }
            ~OpenGLUniformBuffer();
        
            virtual void InitializeBuffer();
            virtual void UpdateBuffer    ();
        
            GLuint m_Buffer;
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
        
            virtual bool SupportsInstanceTextureTransforms();
        
            /* Rasterizer configuration */
            void SetClearBuffers(GLuint Bitfield) { m_ClearBuffers = Bitfield; }
        
            /* Rasterizer functionality */
            virtual void EnableFramebuffer();
            virtual void ClearActiveFramebuffer()               { glClearColor(m_ClearColor.x,m_ClearColor.y,m_ClearColor.z,m_ClearColor.w);
                                                                  glClear(m_ClearBuffers); }
            virtual void SetViewport(i32 x,i32 y,i32 w,i32 h)   { glViewport(x,y,w,h);     }
            virtual RasterContext* CreateContext()              { return new OpenGLRasterizerContext(this); }

            virtual Shader                * CreateShader();
            virtual Texture               * CreateTexture();
            virtual UniformBuffer         * CreateUniformBuffer(ShaderGenerator::INPUT_UNIFORM_TYPE Type);
            virtual RasterObject          * CreateObject() { return new OpenGLObject(this); }

            virtual void Destroy(UniformBuffer* B);
            virtual void Destroy(Shader       * S);
            virtual void Destroy(Texture      * T);
            virtual void Destroy(RasterObject * O);
        
        protected:
            GLuint m_ColorFormat;
            GLuint m_ClearBuffers;
            
            bool m_SupportsInstanceTTrans;
    };
};

