#include <Raster/OpenGL/OpenGLRasterizer.h>
#include <Raster/OpenGL/OpenGLUniform.h>
#include <Renderer/Mesh.h>

namespace Silk
{
    OpenGLObjectIdentifier::OpenGLObjectIdentifier() : m_VAO(0), m_IBIndex(-1)
    {
    }
    OpenGLObjectIdentifier::~OpenGLObjectIdentifier()
    {
        ClearData();
    }
    void OpenGLObjectIdentifier::ClearData()
    {
        if(m_VAO) glDeleteVertexArrays(1,&m_VAO);
        m_VAO = 0;
        for(i32 i = 0;i < m_Attributes.size();i++)
        {
            glDeleteBuffers(1,&m_Attributes[i].BufferID);
        }
        m_Attributes.clear();
    }
    
    void OpenGLObjectIdentifier::SetMesh(Mesh* m)
    {
        ClearData();
        glGenVertexArrays(1, &m_VAO);
        glBindVertexArray(m_VAO);

        i32 AttribCount = m->GetAttributeCount();
        for(i32 i = 0;i < AttribCount;i++)
        {
            const Mesh::MeshAttribute* Attrib = m->GetAttribute(i);
            AddAttribute(Attrib->ShaderIndex   ,
                         Attrib->ComponentCount,
                         Attrib->Size          ,
                         Attrib->Type          ,
                         GL_TRUE               ,
                         Attrib->Stride        ,
                         0);
            
            SupplyBufferData(Attrib->ShaderIndex,GL_ARRAY_BUFFER,Attrib->ComponentCount,Attrib->Size,Attrib->Pointer,Attrib->IsStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
        }
        glBindVertexArray(0);
    }
    void OpenGLObjectIdentifier::AddAttribute(GLuint AttribIndex,GLint ComponentCount,GLint Size,GLenum Type,GLboolean Normalized,GLsizei Stride,GLvoid* Pointer)
    {
        if(GetAttributeBufferIndex(AttribIndex) != -1)
        {
            ERROR("Attribute with index %d already exists in buffer. (Object: 0x%lX)\n",AttribIndex,(intptr_t)this);
            return;
        }
        AttributeBuffer b;
        glGenBuffers(1,&b.BufferID);
        
        if(b.BufferID == 0)
        {
            ERROR("Unable to create attribute buffer for attribute %d. (Object: 0x%lX)\n",AttribIndex,(intptr_t)this);
            return;
        }
        
        b.Index          = AttribIndex   ;
        b.Size           = Size          ;
        b.ComponentCount = ComponentCount;
        b.Type           = Type          ;
        b.Normalized     = Normalized    ;
        b.Stride         = Stride        ;
        b.Pointer        = Pointer       ;
        
        switch(Type)
        {
            case Mesh::AT_BYTE  : { b.TypeSize = sizeof(GLbyte  );b.Type = GL_BYTE          ; break; }
            case Mesh::AT_UBYTE : { b.TypeSize = sizeof(GLubyte );b.Type = GL_UNSIGNED_BYTE ; break; }
            case Mesh::AT_SHORT : { b.TypeSize = sizeof(GLshort );b.Type = GL_SHORT         ; break; }
            case Mesh::AT_USHORT: { b.TypeSize = sizeof(GLushort);b.Type = GL_UNSIGNED_SHORT; break; }
            case Mesh::AT_INT   : { b.TypeSize = sizeof(GLint   );b.Type = GL_INT           ; break; }
            case Mesh::AT_UINT  : { b.TypeSize = sizeof(GLuint  );b.Type = GL_UNSIGNED_INT  ; break; }
            case Mesh::AT_FLOAT : { b.TypeSize = sizeof(GLfloat );b.Type = GL_FLOAT         ; break; }
            case Mesh::AT_DOUBLE: { b.TypeSize = sizeof(GLdouble);b.Type = GL_DOUBLE        ; break; }
            default:
            {
                ERROR("Invalid attribute type 0x%X. (Object: 0x%lX)\n",Type,(intptr_t)this);
                glDeleteBuffers(1,&b.BufferID);
                return;
            }
        }
        
        m_Attributes.push_back(b);
    }
    void OpenGLObjectIdentifier::SupplyBufferData(GLuint AttributeIndex,GLenum Target,GLint ComponentCount,GLsizeiptr Size,GLvoid* Data,GLenum Usage)
    {
        i32 Index = GetAttributeBufferIndex(AttributeIndex);
        if(Index == -1)
        {
            ERROR("Attribute with index %d does not exists in buffer. (Object: 0x%lX)\n",AttributeIndex,(intptr_t)this);
            return;
        }
        
        glBindBuffer(GL_ARRAY_BUFFER,m_Attributes[Index].BufferID);
        glEnableVertexAttribArray(AttributeIndex);
        glVertexAttribPointer(AttributeIndex,ComponentCount,m_Attributes[Index].Type,m_Attributes[Index].Normalized,m_Attributes[Index].Stride,0);
        glBufferData(Target,Size,Data,Usage);
    }
    void OpenGLObjectIdentifier::SetIndexBufferAttributeIndex(GLuint AttributeIndex)
    {
        i32 Index = GetAttributeBufferIndex(AttributeIndex);
        if(Index == -1)
        {
            ERROR("Attribute with index %d does not exists in buffer. (Object: 0x%lX)\n",AttributeIndex,(intptr_t)this);
            return;
        }
        m_IBIndex = Index;
    }
    void* OpenGLObjectIdentifier::MapBuffer(GLuint AttributeIndex,BUFFER_MAP_ACCESS Access)
    {
        i32 Index = GetAttributeBufferIndex(AttributeIndex);
        if(Index == -1)
        {
            ERROR("Attribute with index %d does not exists in buffer. (Object: 0x%lX)\n",AttributeIndex,(intptr_t)this);
            return 0;
        }
        
        glBindBuffer(GL_ARRAY_BUFFER,m_Attributes[Index].BufferID);
        switch(Access)
        {
            case BMA_READ:
            {
                return glMapBuffer(GL_ARRAY_BUFFER,GL_READ_ONLY);
                break;
            }
            case BMA_WRITE:
            {
                return glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
                break;
            }
            case BMA_READWRITE:
            {
                return glMapBuffer(GL_ARRAY_BUFFER,GL_READ_WRITE);
                break;
            }
        }
        return 0;
    }
    void OpenGLObjectIdentifier::UnmapBuffer(GLuint AttributeIndex)
    {
        i32 Index = GetAttributeBufferIndex(AttributeIndex);
        if(Index == -1)
        {
            ERROR("Attribute with index %d does not exists in buffer. (Object: 0x%lX)\n",AttributeIndex,(intptr_t)this);
            return;
        }
        glBindBuffer(GL_ARRAY_BUFFER,m_Attributes[Index].BufferID);
        glUnmapBuffer(GL_ARRAY_BUFFER);
        glBindBuffer(GL_ARRAY_BUFFER,0);
    }
    void OpenGLObjectIdentifier::Render(i32 PrimitiveType,i32 Start,i32 Count)
    {
        if(m_VAO == 0 || m_Attributes.size() == 0) return;

        glBindVertexArray(m_VAO);
        
        if(m_IBIndex != -1) glDrawElements(PrimitiveType,Count,m_Attributes[m_IBIndex].Type,(GLvoid*)(Start * m_Attributes[m_IBIndex].TypeSize));
        else glDrawArrays(PrimitiveType,Start,Count);
        
        glBindVertexArray(0);
    }
    i32 OpenGLObjectIdentifier::GetAttributeBufferIndex(i32 AttribIndex) const
    {
        for(i32 i = 0;i < m_Attributes.size();i++) { if(m_Attributes[i].Index == AttribIndex) { return i; } }
        return -1;
    }
    
    OpenGLUniformBuffer::~OpenGLUniformBuffer()
    {
        if(m_Buffer != 0) glDeleteBuffers(1,&m_Buffer);
        m_Buffer = 0;
    }
    void OpenGLUniformBuffer::InitializeBuffer()
    {
        return;
        if(m_Buffer) glDeleteBuffers(1,&m_Buffer);
        glGenBuffers(1,&m_Buffer);
        
        UpdateBuffer();
    }
    void OpenGLUniformBuffer::UpdateBuffer()
    {
        i32 TotalSize = GetPaddedBufferSize();
        
        Byte* UBO = new Byte[TotalSize];
        memset(UBO,0,TotalSize);
        for(i32 i = 0;i < m_UniformInfo.size();i++)
        {
            i32 Offset = GetPaddedUniformOffset(i);
            i32 Padding = m_UniformInfo[i].PaddedSize - m_UniformInfo[i].TypeSize;
            
            if(m_UniformInfo[i].ArraySize != -1 && Padding != 0)
            {
                i32 dOffset = 0;
                for(i32 u = 0;u < m_UniformInfo[i].ArraySize;u++)
                {
                    memcpy(&UBO[Offset + dOffset],&((Byte*)m_UniformBuffer[i])[u * m_UniformInfo[i].TypeSize],m_UniformInfo[i].TypeSize);
                    dOffset += m_UniformInfo[i].PaddedSize;
                }
            }
            else
            {
                //printf("v[%d]: %s | o: %d | s: %d | p: %d\n",i,m_UniformInfo[i].Name.c_str(),Offset,m_UniformInfo[i].TypeSize,Padding);
                memcpy(&UBO[Offset],m_UniformBuffer[i],m_UniformInfo[i].Size);
            }
        }
        
        /*
        f32* f = (f32*)UBO;
        for(i32 i = 0;i < TotalSize / 4;i += 4)
        {
            printf("[%d]%f,%f,%f,%f\n",4 * i,f[i],f[i + 1],f[i + 2],f[i + 3]);
        }
        */
        
        glBindBuffer(GL_UNIFORM_BUFFER,m_Buffer);
        glBufferData(GL_UNIFORM_BUFFER,TotalSize,UBO,GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER,0);
        
        delete UBO;
    }
    
    OpenGLRasterizerContext::~OpenGLRasterizerContext()
    {
    }

    OpenGLRasterizer::OpenGLRasterizer()
    {
    }
    OpenGLRasterizer::~OpenGLRasterizer()
    {
    }
    
    #define FormatCheck4(r,g,b,a) (f.rbits == (r) && f.gbits == (g) && f.bbits == (b) && f.abits == (a))
    #define FormatCheck3(r,g,b)   FormatCheck4(r,g,b,f.abits)
    #define FormatCheckG(g,a)     (f.graybits == (g) && f.abits == (a))
    
    bool OpenGLRasterizer::ValidateContext(RasterContext *Ctx)
    {
        ColorFormat &f = Ctx->m_Format;
        switch(f.Channels)
        {
            case ColorFormat::CC_RGB:
            {
                if     (FormatCheck3(3 ,3 ,2    )) m_ColorFormat = GL_R3_G3_B2              ;
                else if(FormatCheck3(4 ,4 ,4    )) m_ColorFormat = GL_RGB4                  ;
                else if(FormatCheck3(5 ,5 ,5    )) m_ColorFormat = GL_RGB5                  ;
                else if(FormatCheck3(8 ,8 ,8    )) m_ColorFormat = GL_RGB8                  ;
                else if(FormatCheck3(10,10,10   )) m_ColorFormat = GL_RGB10                 ;
                else if(FormatCheck3(12,12,12   )) m_ColorFormat = GL_RGB12                 ;
                else if(FormatCheck3(16,16,16   )) m_ColorFormat = GL_RGB16                 ;
                else
                {
                    ERROR("Incompatible color format CC_RGB (%d,%d,%d)\n",(i32)f.rbits,
                                                                          (i32)f.gbits,
                                                                          (i32)f.bbits);
                    return false;
                }
                break;
            }
            case ColorFormat::CC_RGBA:
            {
                if     (FormatCheck4(2 , 2, 2, 2)) m_ColorFormat = GL_RGBA2                 ;
                else if(FormatCheck4(4 , 4, 4, 4)) m_ColorFormat = GL_RGBA4                 ;
                else if(FormatCheck4(5 , 5, 5, 1)) m_ColorFormat = GL_RGB5_A1               ;
                else if(FormatCheck4(8 , 8, 8, 8)) m_ColorFormat = GL_RGB8                  ;
                else if(FormatCheck4(10,10,10, 2)) m_ColorFormat = GL_RGB10_A2              ;
                else if(FormatCheck4(12,12,12,12)) m_ColorFormat = GL_RGBA12                ;
                else if(FormatCheck4(16,16,16,16)) m_ColorFormat = GL_RGBA16                ;
                else
                {
                    ERROR("Incompatible color format CC_RGBA (%d,%d,%d,%d)\n",(i32)f.rbits,
                                                                              (i32)f.gbits,
                                                                              (i32)f.bbits,
                                                                              (i32)f.abits);
                    return false;
                }
                break;
            }
            case ColorFormat::CC_GRAYSCALE:
            {
                if     (FormatCheckG(4 ,0       )) m_ColorFormat = GL_LUMINANCE4            ;
                else if(FormatCheckG(8 ,0       )) m_ColorFormat = GL_LUMINANCE4            ;
                else if(FormatCheckG(12,0       )) m_ColorFormat = GL_LUMINANCE4            ;
                else if(FormatCheckG(16,0       )) m_ColorFormat = GL_LUMINANCE4            ;
                else if(FormatCheckG(4 ,4       )) m_ColorFormat = GL_LUMINANCE4_ALPHA4     ;
                else if(FormatCheckG(6 ,2       )) m_ColorFormat = GL_LUMINANCE6_ALPHA2     ;
                else if(FormatCheckG(8 ,8       )) m_ColorFormat = GL_LUMINANCE8_ALPHA8     ;
                else if(FormatCheckG(12,4       )) m_ColorFormat = GL_LUMINANCE12_ALPHA4    ;
                else if(FormatCheckG(12,12      )) m_ColorFormat = GL_LUMINANCE12_ALPHA12   ;
                else if(FormatCheckG(16,16      )) m_ColorFormat = GL_LUMINANCE16_ALPHA16   ;
                else
                {
                    if(f.abits == 0) ERROR("Incompatible color format CC_GRAYSCALE%d\n"        ,(i32)f.graybits);
                    else             ERROR("Incompatible color format CC_GRAYSCALE (%d,A:%d)\n",(i32)f.graybits,
                                                                                                (i32)f.abits);
                    return false;
                }
                break;
            }
            default:
            {
                ERROR("Incompatible color format: Invalid value for ColorFormat::Channels. (Memory corruption?)\n");
                return false;
            }
        }
        
        return true;
    }
    void OpenGLRasterizer::InitializeContext()
    {  
        glewExperimental = GL_TRUE;
        GLenum err = glewInit();
        if(err != GLEW_OK)
        {
            ERROR("Glew initialization failed: %s\n", glewGetErrorString(err));
        }
        glGetError();
        
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
    }
};
